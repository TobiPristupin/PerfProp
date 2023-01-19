/*
 * TODO:
 *  Add a way to generate libpfm_events.txt programatically.
 *  Name migration
 *  Figure out updaters
 *  Number of programmable HPCs
 *  Grouping algorithm, and handle if a group can't be scheduled? + unit tests
 *  Read frequency as param?
 *  Refactor includes using include-what-you-use
 *  Fix problem where certain tests produce error logs when testing failures.
 *  Consider using std::async when reading perf samples to increase read frequency?
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
#include "EventGraph.h"
#include "PmuGrouper.h"
#include "Perf.h"
#include "Logger.h"
#include "PfmLib.h"
#include "TraceableProcess.h"
#include "CommandParser.h"

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


EventGraph populateEventGraph(const std::vector<PmuEvent>& pmuEvents){
    EventGraph graph;
    for (const PmuEvent& event : pmuEvents){
        graph.addNode(event);
    }

    //TODO: read in statistical dependencies and add them.

    return graph;
}

void readAndProcessSamples(const std::unordered_map<int, PmuEvent>& fdsToEvent){
    for (const auto& [fd, event] : fdsToEvent){
        uint64_t sample;
        try {
            sample = Perf::readSample(fd);
        } catch (const std::runtime_error &e){
            Logger::error("read failed for event " + event.getName() + ": " + e.what());
        }

        Logger::info("Read sample for event " + event.getName() + ": " + std::to_string(sample));
    }
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

    //Parent setup done, notify child that they can begin execution
    try {
        tracedProcess->beginExecution();
    } catch (const std::runtime_error& e){
        std::cerr << e.what() << "\n";
        return ECHILD;
    }

    Perf::enableEvents(Utils::keysToVector(fdsToEvent));

    while (!tracedProcess->hasTerminated()){
        readAndProcessSamples(fdsToEvent);
        int millis = 200;
        usleep(millis * 1000);
    }

    Logger::debug("Child process finalized executing");
    Perf::disableEvents(groupLeaderFds); //Disabling only group leaders causes all events to be disabled
    Perf::closeFds(fdsToEvent);

    return 0;
}

int handleListCommand(){
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


/*
 * Hide idea of event graoh, instead just have an event manager
 * remove mean/std dev from event, have event manager store it
 * you can add events to the manager, and register connections between events, with a callback
 * when a new sample is obtained, push the sample to the manager
 * manager should update mean/variances, and then propagate changes
 */



