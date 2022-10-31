#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <string>
#include <sstream>
#include "include/Node.h"
#include "include/Event.h"
#include "include/ProbabilityNode.h"
#include "include/FactorGraph.h"

FactorGraph generateGraph(const std::vector<Event> &events, std::vector<ProbabilityNode> &factors){
    FactorGraph graph;
    for (const ProbabilityNode &factor : factors){
        Node factorNode(&factor);
        for (auto const &event : events){
            if (factor.eventInInput(event)){
                Node eventNode(&event);
                insert_into_graph(factorNode, eventNode, graph);
                insert_into_graph(eventNode, factorNode, graph);
            }
        }
    }

    return graph;
}

//Generates factor graph corresponding to fig. 1 pg. 1746 in https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf
FactorGraph figure1ExampleGraph(){
    std::vector<Event> events = {
            {"X1"}, {"X2"}, {"X3"},
            {"X4"}, {"X5"}, {"X6"},
            {"X7"}, {"X8"}, {"X9"},
    };

    ProbabilityFunction dummyFunc = [](auto p) -> double {return 0;};
    std::vector<ProbabilityNode> factors = {
            {{events[0], events[1], events[2]}, dummyFunc},

            {{events[0], events[1]}, dummyFunc},
            {{events[1], events[2]}, dummyFunc},

            {{events[0], events[3]}, dummyFunc},
            {{events[1], events[4]}, dummyFunc},
            {{events[2], events[5]}, dummyFunc},

            {{events[3], events[4]}, dummyFunc},
            {{events[4], events[5]}, dummyFunc},

            {{events[3], events[6]}, dummyFunc},
            {{events[4], events[7]}, dummyFunc},
            {{events[5], events[8]}, dummyFunc},

            {{events[6], events[7]}, dummyFunc},
            {{events[7], events[8]}, dummyFunc},
    };

    FactorGraph graph = generateGraph(events, factors);
    std::unordered_set<Node> blanket = markovBlanket({Node(&events[4])}, graph);
    for (const Node& n : blanket){
        std::cout << n.getEvent()->name << "\n";
    }

    return graph;
}

//TODO
void printUsage(){
    std::cout << "Usage: bayesperf stat -e {events} program\n";
}

/**
 * Parses a list of events from the command line. The input format for the events is the same as `perf stat`.
 * Events must be in a comma separated list with no spaces. Event names can be appended with a : and a modifier, which
 * can be one or more of {u, k, h, H, G}. Refer to perf's documentation for explanations on their meanings.
 *
 * Example input: cycles,instructions:k,cache-misses:ku
 */
std::vector<Event> parseEvents(const std::string& cmdEventString){
    std::vector<Event> events;
    std::stringstream ss(cmdEventString);
    std::string item;
    while (std::getline(ss, item, ',')){
        std::string::size_type colonPos = item.find(':');
        if (colonPos != std::string::npos){
            events.emplace_back(item.substr(0, colonPos), item.substr(colonPos+1));
        } else {
            events.emplace_back(item);
        }
    }

    return events;
}

int main(int argc, char **argv) {
    int opt;

    if (strcmp(argv[1], "stat") != 0){
        printUsage();
        return 0;
    }

    int optionsIndex = 0;
    static struct option long_options[] = {
                    {"help",     no_argument, nullptr,  'h'},
                    {"events",    required_argument,nullptr,  'e'},
                    //last element of options array has to be filled with zeros
                    {nullptr,      0,                 nullptr,    0}
            };

    while ((opt = getopt_long(argc, argv, "e:", long_options, &optionsIndex)) != -1){
        switch(opt){
            case '?':
                break;
            case 'h':
                printUsage();
                return 0;
            case 'e': {
                std::vector<Event> events = parseEvents(optarg);
                for (const Event &e: events) {
                    std::cout << e.name << " " << e.modifiers << "\n";
                }
                break;
            }
            default:
                printUsage();
                return 0;
        }
    }

    while (optind < argc){
        std::cout << argv[optind++] << "\n";
    }


    return 0;
}


