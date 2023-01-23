#include <cstring>
#include <stdexcept>
#include <getopt.h>
#include "CommandParser.h"
#include "Logger.h"

namespace CommandParser {

    namespace {
        CmdArgs parseStatArgs(int argc, char **argv){
            std::string events;
            std::vector<std::string> programToTrace;

            int optionsIndex = 0;
            static struct option long_options[] = {
                    {"help",     no_argument, nullptr,  'h'},
                    {"events",    required_argument,nullptr,  'e'},
                    //last element of options array has to be filled with zeros
                    {nullptr,      0,                 nullptr,    0}
            };

            /*we start at argv[1] because we want to skip the bayesperf stat command. That is, if we call `bayesperf stat -e ...`,
            * we want to skip the "stat"
            * */
            int opt;
            while ((opt = getopt_long(argc - 1, &argv[1], "+e:h", long_options, &optionsIndex)) != -1){
                switch(opt){
                    case '?':
                        //getopt will automatically print the error to stderr
                        throw std::invalid_argument("Unrecognized argument");
                    case 'h':
                        return {Command::STAT_HELP, {}, {}};
                    case 'e': {
                        events = optarg;
                        break;
                    }
                    default:
                        throw std::runtime_error("getopt returned unhandled code " + std::to_string(opt));
                }
            }

            optind += 1; //skip stat command

            if (events.empty()){
                throw std::invalid_argument("Please input at least one event");
            }

            int numCommandArgs = argc - optind;
            if (numCommandArgs == 0){
                throw std::invalid_argument("Please input a program to trace");
            }

            while (optind < argc){
                programToTrace.emplace_back(argv[optind++]);
            }

            return {Command::STAT, events, programToTrace};
        }
    }


    CmdArgs parseCmdArgs(int argc, char **argv) {
        if (argc <= 1){
            throw std::invalid_argument("No command to bayesperf");
        }

        if (strcmp(argv[1], "list") == 0){
            return {Command::LIST, {}, {}};
        } else if (strcmp(argv[1], "stat") == 0){
            return parseStatArgs(argc, argv);
        } else {
            throw std::invalid_argument("Invalid command '" + std::string(argv[1]) + "' to bayesperf");
        }
    }
}