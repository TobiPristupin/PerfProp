#ifndef BAYESPERF_CPP_NODE_H
#define BAYESPERF_CPP_NODE_H


//TODO: Edit this to use smart pointers

#include "ProbabilityFunction.h"
#include "Node.h"

class Node {

    const ProbabilityFunction *probabilityFunction;
    const Event *event;
public:
    explicit Node(const ProbabilityFunction *probabilityFunction1);
    explicit Node(const Event *event);

    bool isFactorNode() const;
    bool isEventNode() const;

    const Event* getEvent() const;
    const ProbabilityFunction* getProbabilityFunction() const;
};


#endif //BAYESPERF_CPP_NODE_H
