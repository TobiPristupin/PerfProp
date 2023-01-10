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
    int x = 10;
    Obj obj(x);
    std::cout << x << "\n";
    std::cout << obj.getS() << "\n";

}