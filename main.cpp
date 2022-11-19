#include <vector>
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <string>
#include "include/PmuEvent.h"
#include "include/ProbabilityNode.h"
#include "include/PmuEventParser.h"
#include "FactorGraph.h"


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

    PmuEvent e;

    return 0;
}




