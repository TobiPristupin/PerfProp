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
#include "expected.h"

static const std::string usageString = "Usage: bayesperf stat -e {events} {program}\n";

void printUsage(){
    std::cout << usageString;
}

/*
 * Parses cmd args and returns a string of events, and a vector with the shell commands for bayesperf to trace.
 * Throws std::invalid_argument
 * Will call exit(0) if the user calls -h
 */
std::pair<std::string, std::vector<std::string>> handleCmdArgs(int argc, char* argv[]){
    if (strcmp(argv[1], "stat") != 0){
        throw std::invalid_argument("Invalid command '" + std::string(argv[1]) + "' to bayesperf");
    }

    std::string events;
    std::vector<std::string> programToTrace;

    int optionsIndex = 0;
    static struct option long_options[] = {
            {"help",     no_argument, nullptr,  'h'},
            {"events",    required_argument,nullptr,  'e'},
            //last element of options array has to be filled with zeros
            {nullptr,      0,                 nullptr,    0}
    };

    /*we start at argv[1] because we want to skip the bayesperf command. That is, if we call `bayesperf stat -e ...`,
    * we want to skip the "stat"
    * */
    int opt;
    while ((opt = getopt_long(argc - 1, &argv[1], "+e:", long_options, &optionsIndex)) != -1){
        switch(opt){
            case '?':
                //getopt will automatically print the error to stderr
                throw std::invalid_argument("Unrecognized argument");
            case 'h':
                printUsage();
                exit(0);
            case 'e': {
                events = optarg;
                break;
            }
            default:
                printUsage();
                exit(0);
        }
    }

    optind += 1; //skip command to bayesperf

    int numCommandArgs = argc - optind;
    if (numCommandArgs == 0){
        throw std::invalid_argument("Please input a program to trace");
    }

    while (optind < argc){
        programToTrace.emplace_back(argv[optind++]);
    }

    return std::make_pair(events, programToTrace);
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

int main(int argc, char *argv[]) {
    PfmLib pfmlib;
    pfmlib.initialize();

    std::string eventsString;
    std::vector<std::string> programToTrace;
    try {
        std::tie(eventsString, programToTrace) = handleCmdArgs(argc, argv);
    } catch (const std::invalid_argument& e){
        std::cerr << e.what() << "\n";
        printUsage();
        return EINVAL;
    }

    std::vector<PmuEvent> events = PmuParser::parseEvents(eventsString);
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


/*
 * Hide idea of event graoh, instead just have an event manager
 * remove mean/std dev from event, have event manager store it
 * you can add events to the manager, and register connections between events, with a callback
 * when a new sample is obtained, push the sample to the manager
 * manager should update mean/variances, and then propagate changes
 */



