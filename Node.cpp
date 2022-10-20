#include <stdexcept>
#include "include/Node.h"

Node::Node(ProbabilityFunction *probabilityFunction) {
    this->probabilityFunction = probabilityFunction;
    this->event = nullptr;
}
Node::Node(Event *event) {
    this->event = event;
    this->probabilityFunction = nullptr;
}

bool Node::isFactorNode() const {
    return probabilityFunction != nullptr;
}

bool Node::isEventNode() const {
    return event != nullptr;
}
Event *Node::getEvent() const {
    if (event == nullptr){
        throw std::runtime_error("Attempting to access the event of a non-event node");
    }
    return event;
}

ProbabilityFunction *Node::getProbabilityFunction() const {
    if (probabilityFunction == nullptr){
        throw std::runtime_error("Attempting to access the probability function of a non-event node");
    }
    return probabilityFunction;
}


