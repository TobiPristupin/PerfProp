//TODO: Figure out updaters
//TODO: Grouping algorithm
//TODO: Number of programmable HPCs
//TODO: Refactor PmuEventParser to use a namespace
//TODO: Refactor includes using include-what-you-use

#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <string>
#include <csignal>
#include <memory>
#include <sys/wait.h>
#include "include/PmuEvent.h"
#include "include/PmuEventParser.h"
#include "include/EventGraph.h"
#include "include/Updaters.h"
#include "include/PmuGrouper.h"
#include "PmuArch.h"

#define BAYESPERF_DEBUG

static const std::string usageString = "Usage: bayesperf stat -e {events} program\n";
static std::vector<PmuEvent> events;
static std::vector<std::string> cmdProgram;

void printUsage(){
    std::cout << usageString;
}

/*
 * Parses cmd args. This function will either call exit() and never return, or initialize the global variable
 * events.
 */
void handleCmdArgs(int argc, char* argv[]){
    int opt;

    if (strcmp(argv[1], "stat") != 0){
        printUsage();
        exit(0);
    }

    int optionsIndex = 0;
    static struct option long_options[] = {
            {"help",     no_argument, nullptr,  'h'},
            {"events",    required_argument,nullptr,  'e'},
            //last element of options array has to be filled with zeros
            {nullptr,      0,                 nullptr,    0}
    };

    /*we advance argv by 1 because we want to skip the bayesperf command. That is, if we call `bayesperf stat -e ...`,
    * we want to skip the "stat"
    * */
    while ((opt = getopt_long(argc - 1, &argv[1], "+e:", long_options, &optionsIndex)) != -1){
        switch(opt){
            case '?':
                exit(1); //getopt will automatically print the error to stderr
            case 'h':
                printUsage();
                exit(0);
            case 'e': {
                events = parseEvents(optarg);
                break;
            }
            default:
                printUsage();
                exit(0);
        }
    }

    optind += 1; //skip command to bayesperf
    while (optind < argc){
        cmdProgram.emplace_back(argv[optind++]);
    }

    if (cmdProgram.empty()){
        std::cout << "Please input a program to execute\n";
        printUsage();
    }
}

EventGraph populateEventGraph(const std::vector<PmuEvent>& pmuEvents){
    EventGraph graph;
    for (const PmuEvent& event : pmuEvents){
        graph.addNode(event);
    }

    //TODO: read in statistical dependencies and add them.

    return graph;
}

EventGraph generateDebugGraph(){
    std::vector<PmuEvent> pmuEvents = {
            {"E1", PmuEvent::Type::SOFTWARE},
            {"E2", PmuEvent::Type::HARDWARE},
            {"E3", PmuEvent::Type::SOFTWARE},
            {"E4", PmuEvent::Type::HARDWARE},
            {"E5", PmuEvent::Type::SOFTWARE},
    };

    events = pmuEvents;

    EventGraph graph;
    for (const PmuEvent& event : pmuEvents){
        graph.addNode(event);
    }


//    TODO? UNSURE graph.addEdge(pmuEvents[0], pmuEvents[1],
//                  linearCorrectionUpdater(0.1, 0.11, 0.1)
//                  );

    return graph;
}

int reportError(const std::string& msg){
    int error = errno; //perror may modify errno, so we save it beforehand
    perror(msg.c_str());
    return error;
}

int main(int argc, char *argv[]) {
    handleCmdArgs(argc, argv);
#ifdef BAYESPERF_DEBUG
    EventGraph graph = generateDebugGraph();
#else
    EventGraph graph = populateEventGraph(events);
#endif

    size_t groupSize = PmuArch::numProgrammableHPCs();
    std::vector<std::vector<PmuEvent>> groups = PmuGrouper::group(events, groupSize);

    pid_t pid = fork();
    if (pid < 0){
        return reportError("fork()");
    } else if (pid == 0){ //Child process
        auto args = std::make_unique<char*[]>(cmdProgram.size() + 1);
        int i = 0;
        for (const std::string& arg : cmdProgram){
            /*
             * NOTE: Here we allocate memory for each char* by using new.
             */
            args[i] = new char[arg.size() + 1];
            std::strcpy(args[i], arg.c_str());
            i++;
        }
        //Last element must be NULL by convention
        args[cmdProgram.size()] = nullptr;

        std::cout << "from thre child!!" << std::endl;
        if (execvp(args[0], args.get()) < 0){
            return reportError("execvpe()");
        }
    } else { //Parent process
        std::cout << "pid of child " << pid << "\n";
        if (waitpid(pid, nullptr, 0) < 0){
            return reportError("waitpid()");
        }

        std::cout << "Child finalized\n";
    }


    return 0;
}

/*
 * Hide idea of event graoh, instead just have an event manager
 * you can add events to the manager, and register connections between events, with a callback
 * when a new sample is obtained, push the sample to the manager
 * manager should update mean/variances, and then propagate changes
 */



