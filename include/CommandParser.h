#ifndef BAYESPERF_CPP_COMMANDPARSER_H
#define BAYESPERF_CPP_COMMANDPARSER_H

#include <string>
#include <vector>
#include <optional>

namespace CommandParser {

    enum class Command {
        STAT, LIST, STAT_HELP
    };

    struct CmdArgs {
        Command command{};

        //only valid if command is STAT
        std::optional<std::string> unparsedEventsList;
        std::optional<std::vector<std::string>> programToTrace;
    };

    /*
     * May throws std::invalid_argument
     */
    CmdArgs parseCmdArgs(int argc, char* argv[]);

}

#endif
