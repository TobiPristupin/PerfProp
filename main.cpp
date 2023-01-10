#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <string>
#include "include/PmuEvent.h"
#include "include/PmuEventParser.h"
#include "include/EventGraph.h"

#define TOBI_DEBUG

static const std::string usageString = "Usage: bayesperf stat -e {events} program\n";
static std::vector<PmuEvent> events;

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
                for (const PmuEvent &e: events) {
                    std::cout << e.name << " " << e.modifiers << "\n";
                }
                break;
            }
            default:
                printUsage();
                return exit(0);
        }
    }

    optind += 1; //skip command to bayesperf
    while (optind < argc){
        std::cout << argv[optind++] << " ";
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

    EventGraph graph;
    for (const PmuEvent& event : pmuEvents){
        graph.addNode(event);
    }


//    graph.addEdge(pmuEvents[0], pmuEvents[1], [] (double to_update, double related) {
//        return 0.1;
//    });

}

int main(int argc, char *argv[]) {
    handleCmdArgs(argc, argv);
#ifdef TOBI_DEBUG
    EventGraph graph = populateEventGraph(events);
#else
    EventGraph graph = populateEventGraph(events);
#endif

    return 0;
}




