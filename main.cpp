/*
 * TODO:
 *  Name migration
 *  Number of programmable HPCs
 *  Grouping algorithm, and handle if a group can't be scheduled? + unit tests
 *  Refactor includes using include-what-you-use
 *  Fix problem where certain tests produce error logs when testing failures.
 *  Write README, link style guide
 */

#include <vector>
#include <iostream>
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


std::unique_ptr<SampleCollector> initDebugCollector(const std::vector<PmuEvent>& pmuEvents){
    auto collector = std::make_unique<SampleCollector>(pmuEvents);
    //TODO: read in statistical dependencies and add them.

    collector->addRelationship(pmuEvents.at(0), pmuEvents.at(1), [&] (const PmuEvent::Stats& relatedEventStats,
                                                              PmuEvent::Stats &eventToUpdate) {
        std::cout << "[Stat Propagation] from=" << pmuEvents.at(0).getName() << " to=" << pmuEvents.at(1).getName() << "\n";
        std::cout << "[Stat Propagation] Before=" << eventToUpdate << "\n";
        eventToUpdate.meanCountsPerMillis = Updater::linearCorrection(
                2*relatedEventStats.meanCountsPerMillis, eventToUpdate.meanCountsPerMillis, 0.2);
        std::cout << "[Stat Propagation] After=" << eventToUpdate << "\n";

    });

    return collector;
}

void printCurrentStats(const std::map<int, PmuEvent>& fdsToEvent, SampleCollector* sampleCollector){
    //TODO: would be nice if this printed as a formatted table
    for (const auto& [fd, event] : fdsToEvent){
        std::optional<PmuEvent::Stats> stats = sampleCollector->getEventStatistics(event);
        if (stats.has_value()){
            std::cout << "[Stat] Event=" << event.getName() << " " << stats.value() << "\n";
        }
    }
    std::cout << "\n";
}

void readAndProcessSamplesOneRound(const std::map<int, PmuEvent>& fdsToEvent, SampleCollector* sampleCollector){
    for (const auto& [fd, event] : fdsToEvent){
        try {
            Perf::Sample sample = Perf::readSample(fd);
            if (sample.timeEnabled.count() == 0){ //we may get empty samples if the child process has not yet been scheduled
                continue;
            }

            Logger::info("Read sample for event " + event.getName() + ": val=" + std::to_string(sample.value)
                         + " enabled=" + std::to_string(sample.timeEnabled.count()) + " running=" + std::to_string(sample.timeRunning.count()));

            sampleCollector->pushSample(event, sample);
        } catch (const std::runtime_error &e){
            Logger::error("read failed for event " + event.getName() + ": " + e.what());
        }
    }

    printCurrentStats(fdsToEvent, sampleCollector);
}

int handleStatCommand(const std::string& unparsedEventsList, const std::vector<std::string>& programToTrace){
    PfmLib::Instance pfmlib;
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
    std::unique_ptr<SampleCollector> sampleCollector = initDebugCollector(events);

    //Parent setup done, notify child that they can begin execution
    try {
        tracedProcess->beginExecution();
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << "\n";
        return ECHILD;
    }

    while (!tracedProcess->hasTerminated()){
        readAndProcessSamplesOneRound(fdsToEvent, sampleCollector.get());
        int millis = 200;
        usleep(millis * 1000);
    }

    Logger::debug("Child process finalized executing");
    Perf::disableEvents(allFds);
    Perf::closeFds(fdsToEvent);

    return 0;
}

int handleListCommand(){
    PfmLib::Instance pfmLib;
    pfmLib.initialize();
    PfmLib::printAllEventsInfo();
    return 0;
}

int main(int argc, char *argv[]) {
    CommandParser::CmdArgs cmdArgs;
    try {
        cmdArgs = CommandParser::parseCmdArgs(argc, argv);
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        printUsage();
        return EINVAL;
    } catch (const std::invalid_argument& e){
        std::cerr << e.what() << std::endl;
        printUsage();
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