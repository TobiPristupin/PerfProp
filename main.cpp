#include <iostream>
#include <set>
#include <map>
#include "include/Node.h"

//Could be using the unordered variants, but that would require writing a hash function for Node, which is not trivial.
//Since our set of events is always very small, the added complexity of using the ordered variants does not matter.
using FactorGraph = std::map<Node, std::set<Node>>;

FactorGraph generateGraph(const std::vector<Event> &events, const std::vector<ProbabilityFunction> &factors){
    FactorGraph graph;
    for (auto const &factor : factors){
        Node factorNode(factor);
        for (auto const &event : events){
            if (factor.eventInInput(event)){

            }
        }
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}


