#ifndef BAYESPERF_CPP_UTILS_H
#define BAYESPERF_CPP_UTILS_H

#include <vector>
#include <string>

std::vector<std::string> splitString(const std::string& str, char delimiter);

std::string ltrim(std::string s, const char* t = " \t\n\r\f\v");
std::string rtrim(std::string s, const char* t = " \t\n\r\f\v");
std::string trim(std::string s, const char* t = " \t\n\r\f\v");

//Taken from https://stackoverflow.com/a/54728293/8402038
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hash_combine(seed, rest), ...);
}

#endif