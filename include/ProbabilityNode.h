#ifndef BAYESPERF_CPP_PROBABILITYNODE_H
#define BAYESPERF_CPP_PROBABILITYNODE_H

#include <vector>
#include <functional>
#include "Event.h"

using ProbabilityFunction = double(*)(std::vector<Event> events);

class ProbabilityNode {
    std::vector<Event> eventsInput;
    ProbabilityFunction function;

public:
    ProbabilityNode(std::initializer_list<Event> events, ProbabilityFunction function);
    bool eventInInput(const Event& event) const;
    std::vector<Event> getEvents() const;
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
