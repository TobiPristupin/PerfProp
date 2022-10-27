#ifndef BAYESPERF_CPP_NODE_H
#define BAYESPERF_CPP_NODE_H


//TODO: Edit this to use smart pointers

#include "ProbabilityNode.h"

class Node {

    const ProbabilityNode *probabilityFunction;
    const Event *event;
public:
    explicit Node(const ProbabilityNode *probabilityFunction1);
    explicit Node(const Event *event);

    bool isFactorNode() const;
    bool isEventNode() const;

    const Event* getEvent() const;
    const ProbabilityNode* getProbabilityFunction() const;

    bool operator==(const Node& e) const;
};

namespace std {
    template <>
    struct hash<Node> {
        std::size_t operator()(const Node& node) const {
            if (node.isEventNode()){
                return std::hash<Event>()(*node.getEvent());
            } else {
                return std::hash<ProbabilityNode>()(*node.getProbabilityFunction());
            }
        }
    };
}


#endif //BAYESPERF_CPP_NODE_H
