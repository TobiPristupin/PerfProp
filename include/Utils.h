#ifndef BAYESPERF_CPP_UTILS_H
#define BAYESPERF_CPP_UTILS_H

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace Utils {

    template<class K, class V>
    std::vector<K> keysToVector(std::map<K,V> map){
        std::vector<K> vec;
        for (const auto& [key, val] : map){
            vec.push_back(key);
        }
        return vec;
    }

    //Taken from https://stackoverflow.com/a/54728293/8402038
    template <typename T, typename... Rest>
    void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
    {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hash_combine(seed, rest), ...);
    }
}


#endif