//standalone file to test cpp code. This file compilers to its own executable that does not touch any bayesperf code.

#include <iostream>
#include <unordered_set>
#include <variant>
#include <map>

class NoCopy {

};

class Obj {
public:
    Obj(const int& x) : s(x) {
        s += 1;
    };

    int getS(){
        return s;
    }

private:
    int s;
};

int main(int argc, char **argv){
    int i = 2;
    for (i; i < i + 5; i++){
        std::cout << i << "\n";
    }

}