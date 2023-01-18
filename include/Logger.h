#ifndef BAYESPERF_CPP_LOGGER_H
#define BAYESPERF_CPP_LOGGER_H

#define BAYESPERF_LOG_LEVEL 1 //0 for OFF, 1 for DEBUG, 2 for INFO

#include <string>
#include <iostream>

/*
 * Very simple hacky logger. Change BAYESPERF_LOG_LEVEL to set the level. A more robust implementation that allows to
 * set the logging level via a cmd flag can be added in the future if necessary.
 */
namespace Logger {
    enum Level {
        OFF, DEBUG, INFO //Do not change order
    };

    constexpr Level level = static_cast<Level>(BAYESPERF_LOG_LEVEL);

    inline void debug(const std::string& message) {
        if (level == DEBUG){
            std::cout << "[DEBUG]\t"<< message << std::endl;
        }
    }

    inline void info(const std::string& message) {
        if (level == DEBUG || level == INFO){
            std::cout << "[INFO]\t" << message << std::endl;
        }
    }

    /*
     * Error messages will always be shown regardless of debug level
     */
    inline void error(const std::string& message) {
        std::cerr << "[ERROR]\t" << message << std::endl;
    }
}


#endif
