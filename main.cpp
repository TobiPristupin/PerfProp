#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <string>
#include "include/Node.h"
#include "include/PmuEvent.h"
#include "include/ProbabilityNode.h"
#include "include/FactorGraph.h"
#include "include/PmuEventParser.h"

FactorGraph generateGraph(const std::vector<PmuEvent> &events, std::vector<ProbabilityNode> &factors){
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
    std::vector<PmuEvent> events = {
            {"X1", PmuEvent::Type::HARDWARE}, {"X2", PmuEvent::Type::HARDWARE}, {"X3", PmuEvent::Type::HARDWARE},
            {"X4", PmuEvent::Type::HARDWARE}, {"X5", PmuEvent::Type::HARDWARE}, {"X6", PmuEvent::Type::HARDWARE},
            {"X7", PmuEvent::Type::HARDWARE}, {"X8", PmuEvent::Type::HARDWARE}, {"X9", PmuEvent::Type::HARDWARE},
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

void printUsage(){
    std::cout << "Usage: bayesperf stat -e {events} program\n";
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

    /*we advance argv by 1 because we want to skip the bayesperf command. That is, if we call `bayesperf stat -e ...`,
    * we want to skip the "stat"
    * */
    while ((opt = getopt_long(argc - 1, &argv[1], "+e:", long_options, &optionsIndex)) != -1){
        switch(opt){
            case '?':
                return 1; //getopt will automatically print the error to stderr
            case 'h':
                printUsage();
                return 0;
            case 'e': {
                std::vector<PmuEvent> events = parseEvents(optarg);
                for (const PmuEvent &e: events) {
                    std::cout << e.name << " " << e.modifiers << "\n";
                }
                break;
            }
            default:
                printUsage();
                return 0;
        }
    }

    optind += 1; //skip command to bayesperf
    while (optind < argc){
        std::cout << argv[optind++] << " ";
    }
    std::cout << "\n";


    return 0;
}


