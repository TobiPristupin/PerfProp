#include <cinttypes>
#include <err.h>
#include <perfmon/pfmlib.h>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <string>
#include <perfmon/pfmlib_perf_event.h>
#include <iostream>
#include <chrono>

template<class K, class V>
std::vector<K> keysToVector(std::unordered_map<K,V> map){
    std::vector<K> vec;
    for (const auto& [key, val] : map){
        vec.push_back(key);
    }
    return vec;
}

int main(int argc, char **argv) {
    std::unordered_map<std::string, int> m;
    m["hey"] = 2;
    m["caca"] = 3;
    std::vector<std::string> vec = keysToVector(m);
    for (auto s : vec){
        std::cout << s << "\n";
    }
//    int ret;
//
//    ret = pfm_initialize();
//    if (ret != PFM_SUCCESS)
//        errx(1, "cannot initialize library %s", pfm_strerror(ret));
//
//    pfm_perf_encode_arg_t arg;
//    memset(&arg, 0, sizeof(arg));
//    perf_event_attr attr{0};
//    arg.attr = &attr;
//    arg.size = sizeof(pfm_perf_encode_arg_t);
//
//    ret = pfm_get_os_event_encoding("cs", PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
//    if (ret != PFM_SUCCESS)
//        printf("cannot get encoding %s", pfm_strerror(ret));
//
//    return 0;
    uint64_t x = 5;
    long double y = x;
}
