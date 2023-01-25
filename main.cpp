/*
 * TODO:
 *  Number of programmable HPCs
 *  Grouping algorithm, and handle if a group can't be scheduled? + unit tests
 *  Refactor includes using include-what-you-use
 *  Fix problem where certain tests produce error logs when testing failures.
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
Usage: perfprop {command} {args}

The stat command traces given PMU events on a program.
Usage: perfprop stat -e {events} {program}
Where events is a comma delimited list of events with no spaces

The list command outputs the available PMU events
Usage: perfprop list
)";

#include "StatPrinter.h"


void printUsage(){
    std::cout << usageString;
}


std::unique_ptr<SampleCollector> initCollector(const std::vector<PmuEvent>& pmuEvents){
    auto collector = std::make_unique<SampleCollector>(pmuEvents);
    //TODO: read in statistical dependencies and add them.

    collector->addRelationship(pmuEvents.at(0), pmuEvents.at(1), [&] (const PmuEvent::Stats& relatedEventStats,
                                                              PmuEvent::Stats &eventToUpdate) {
        /*
         * Here we have to modify eventToUpdate. We can do something simple like
         * ->   eventToUpdate.meanCountsPerMillis = relatedEventStats.meanCountPerMillis;
         * which would set the mean of the event to update to be the same as the event that received the new sample.
         *
         * Or, we can employ more advanced techniques such as using a linear corrector, as seen below
         *
         * In this function, only update mean and/or variance for eventToUpdate.
         */
        eventToUpdate.meanCountsPerMillis = Updater::linearCorrection(
                2*relatedEventStats.meanCountsPerMillis, eventToUpdate.meanCountsPerMillis, 0.2);
    });

    return collector;
}

void printCurrentStats(const std::map<int, PmuEvent>& fdsToEvent, SampleCollector* sampleCollector){
    StatPrinter table;
    for (const auto& [fd, event] : fdsToEvent){
        std::optional<PmuEvent::Stats> stats = sampleCollector->getEventStatistics(event);
        if (stats.has_value()){
            table.addRow(event.getName(), stats.value());
        }
    }
    table.print();
    std::cout << "\n";
}

void readAndProcessSamplesOneRound(const std::map<int, PmuEvent>& fdsToEvent, SampleCollector* sampleCollector){
    bool atLeastOneValidSample = false;
    for (const auto& [fd, event] : fdsToEvent){
        try {
            Perf::Sample sample = Perf::readSample(fd);
            if (sample.timeEnabled.count() == 0){ //we may get empty samples if the child process has not yet been scheduled
                continue;
            }

            atLeastOneValidSample = true;
            Logger::info("Read sample for event " + event.getName() + ": val=" + std::to_string(sample.value)
                         + " enabled=" + std::to_string(sample.timeEnabled.count()) + " running=" + std::to_string(sample.timeRunning.count()));

            sampleCollector->pushSample(event, sample);
        } catch (const std::runtime_error &e){
            Logger::error("read failed for event " + event.getName() + ": " + e.what());
        }
    }

    if (atLeastOneValidSample){
        printCurrentStats(fdsToEvent, sampleCollector);
    }
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
    std::unique_ptr<SampleCollector> sampleCollector = initCollector(events);

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