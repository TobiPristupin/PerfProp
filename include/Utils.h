#ifndef BAYESPERF_CPP_UTILS_H
#define BAYESPERF_CPP_UTILS_H

#include <vector>
#include <string>

std::vector<std::string> splitString(const std::string& str, char delimiter);

std::string ltrim(std::string s, const char* t = " \t\n\r\f\v");
std::string rtrim(std::string s, const char* t = " \t\n\r\f\v");
std::string trim(std::string s, const char* t = " \t\n\r\f\v");

#endif