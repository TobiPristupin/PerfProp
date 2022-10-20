#ifndef BAYESPERF_CPP_NODE_H
#define BAYESPERF_CPP_NODE_H


#include "ProbabilityFunction.h"
//TODO: Edit this to use smart pointers

class Node {

    ProbabilityFunction *probabilityFunction;
    Event *event;
public:
    explicit Node(ProbabilityFunction *probabilityFunction1);
    explicit Node(Event *event);

    bool isFactorNode() const;
    bool isEventNode() const;

    Event* getEvent() const;
    ProbabilityFunction* getProbabilityFunction() const;
};


#endif //BAYESPERF_CPP_NODE_H
