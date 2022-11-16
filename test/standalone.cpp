//standalone file to test cpp code. This file compilers to its own executable that does not touch any bayesperf code.

#include <iostream>
#include <unordered_set>
#include <variant>
#include <map>

class Unhash {

};

int main(int argc, char **argv){
    std::map<std::variant<int, std::string>, std::unordered_set<std::variant<int, std::string>>> m;
    m.insert({1, {2, "hey"}});
    for (const std::variant<int, std::string>& v : m[1]){
        std::cout << std::holds_alternative<int>(v) << "\n";
    }

    return 0;

}