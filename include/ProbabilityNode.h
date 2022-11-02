#ifndef BAYESPERF_CPP_PROBABILITYNODE_H
#define BAYESPERF_CPP_PROBABILITYNODE_H

#include <vector>
#include <functional>
#include "PmuEvent.h"

using ProbabilityFunction = double(*)(std::vector<PmuEvent> events);

class ProbabilityNode {
    std::vector<PmuEvent> eventsInput;
    ProbabilityFunction function;

public:
    ProbabilityNode(std::initializer_list<PmuEvent> events, ProbabilityFunction function);
    bool eventInInput(const PmuEvent& event) const;
    std::vector<PmuEvent> getEvents() const;
    ProbabilityFunction getFunction() const;
    double call();

    bool operator==(const ProbabilityNode& node) const;
};

namespace std {
    template <>
    struct hash<ProbabilityNode> {
        std::size_t operator()(const ProbabilityNode& node) const {
            return std::hash<ProbabilityFunction>()(node.getFunction());
        }
    };
}

#endif //BAYESPERF_CPP_PROBABILITYNODE_H
