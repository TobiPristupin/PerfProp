/*
 * TODO:
 *  Add a way to generate libpfm_events.txt programatically.
 *  Name migration
 *  Number of programmable HPCs
 *  Grouping algorithm, and handle if a group can't be scheduled? + unit tests
 *  Read frequency as param?
 *  Refactor includes using include-what-you-use
 *  Fix problem where certain tests produce error logs when testing failures.
 */

#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <string>
#include <memory>
#include <sys/wait.h>
#include "PmuEvent.h"
#include "PmuParser.h"
#include "SampleCollector.h"
#include "PmuGrouper.h"
#include "Perf.h"
#include "Logger.h"
#include "PfmLib.h"
#include "TraceableProcess.h"
#include "CommandParser.h"
#include "Updaters.h"

static const std::string usageString = R"(
Usage: bayesperf {command} {args}

The stat command traces given PMU events on a program.
Usage: bayesperf stat -e {events} {program}
Where events is a comma delimited list of events with no spaces

The list command outputs the available PMU events
Usage: bayesperf list
)";

void printUsage(){
    std::cout << usageString;
}


SampleCollector initDebugCollector(const std::vector<PmuEvent>& pmuEvents){
    SampleCollector collector(pmuEvents);
    //TODO: read in statistical dependencies and add them.

//    collector.addRelationship(pmuEvents.at(0), pmuEvents.at(1), [] (const PmuEvent::Stats& relatedEventStats,
//                                                              PmuEvent::Stats &eventToUpdate) {
//        eventToUpdate.mean = Updater::linearCorrection(2*relatedEventStats.mean, eventToUpdate.mean, 0.2);
//        std::cout << 1;
//    });

    return collector;
}

void readAndProcessSamplesOneRound(const std::map<int, PmuEvent>& fdsToEvent, SampleCollector& sampleCollector){
    for (const auto& [fd, event] : fdsToEvent){
        Perf::Sample sample{};
        try {
            sample = Perf::readSample(fd);
            //sampleCollector.pushSample(event, sample);
        } catch (const std::runtime_error &e){
            Logger::error("read failed for event " + event.getName() + ": " + e.what());
        }

        Logger::info("Read sample for event " + event.getName() + ": val=" + std::to_string(sample.value)
        + " enabled=" + std::to_string(sample.timeEnabled) + " running=" + std::to_string(sample.timeRunning));
    }

    //TODO:Print


}

int handleStatCommand(const std::string& unparsedEventsList, const std::vector<std::string>& programToTrace){
    PfmLib pfmlib;
    pfmlib.initialize();

    std::vector<PmuEvent> events = PmuParser::parseEvents(unparsedEventsList);
    size_t groupSize = Perf::numProgrammableHPCs();
    std::vector<std::vector<PmuEvent>> groups = PmuGrouper::group(events, groupSize);

    std::unique_ptr<TraceableProcess> tracedProcess;
    try {
        tracedProcess = TraceableProcessFactory::create(programToTrace);
    } catch (const std::runtime_error &e){
        std::cerr << e.what() << "\n";
        return ECHILD;
    }

    //Parent setup, code that follows is code that we want to run before we start our traced program
    Logger::debug("Child process created with pid " + std::to_string(tracedProcess->getPid()));
    auto [fdsToEvent, groupLeaderFds] = Perf::perfOpenEvents(groups, tracedProcess->getPid());
    std::vector<int> allFds = Utils::keysToVector(fdsToEvent);
    SampleCollector sampleCollector = initDebugCollector(events);

    //Parent setup done, notify child that they can begin execution
    try {
        tracedProcess->beginExecution();
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << "\n";
        return ECHILD;
    }

    while (!tracedProcess->hasTerminated()){
        readAndProcessSamplesOneRound(fdsToEvent, sampleCollector);
        int millis = 200;
        usleep(millis * 1000);
    }

    Logger::debug("Child process finalized executing");
    Perf::disableEvents(allFds);
    Perf::closeFds(fdsToEvent);

    return 0;
}

int handleListCommand(){
    PfmLib pfmLib;
    pfmLib.initialize();


    return 0;
}

int main(int argc, char *argv[]) {
    CommandParser::CmdArgs cmdArgs;
    try {
        cmdArgs = CommandParser::parseCmdArgs(argc, argv);
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << "\n";
        return EINVAL;
    } catch (const std::invalid_argument& e){
        std::cerr << e.what() << "\n";
        return EINVAL;
    }

    switch (cmdArgs.command) {
        case CommandParser::Command::STAT_HELP:
            printUsage();
            return 0;
        case CommandParser::Command::STAT:
            return handleStatCommand(cmdArgs.unparsedEventsList.value(), cmdArgs.programToTrace.value());
        case CommandParser::Command::LIST:
            return handleListCommand();
    }
}

//need to reset counter after each read.


