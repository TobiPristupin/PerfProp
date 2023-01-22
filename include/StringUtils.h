#ifndef BAYESPERF_CPP_STRINGUTILS_H
#define BAYESPERF_CPP_STRINGUTILS_H

#include <string>
#include <vector>

namespace StringUtils {

    [[nodiscard]] std::vector<std::string> split(const std::string& str, char delimiter);

    [[nodiscard]] std::string ltrim(std::string s, const char* t = " \t\n\r\f\v");
    [[nodiscard]] std::string rtrim(std::string s, const char* t = " \t\n\r\f\v");
    [[nodiscard]] std::string trim(std::string s, const char* t = " \t\n\r\f\v");
}

#endif
