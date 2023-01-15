#include <sstream>
#include <utility>
#include "include/Utils.h"

namespace Utils {

    std::vector<std::string> splitString(const std::string &str, char delimiter) {
        std::vector<std::string> vec;
        std::string item;
        std::stringstream ss(str);
        while (std::getline(ss, item, delimiter)) {
            if (item.empty()) {
                continue;
            }
            vec.push_back(item);
        }

        return vec;
    }

    //https://stackoverflow.com/a/25829233/8402038
    // trim from left
    std::string ltrim(std::string s, const char *t) {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }

    // trim from right
    std::string rtrim(std::string s, const char *t) {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }

    // trim from left & right
    std::string trim(std::string s, const char *t) {
        return ltrim(rtrim(std::move(s), t), t);
    }
}