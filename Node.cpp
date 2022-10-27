#include <stdexcept>
#include "include/Node.h"

Node::Node(const ProbabilityNode *probabilityFunction) {
    this->probabilityFunction = probabilityFunction;
    this->event = nullptr;
}
Node::Node(const Event *event) {
    this->event = event;
    this->probabilityFunction = nullptr;
}

bool Node::isFactorNode() const {
    return probabilityFunction != nullptr;
}

bool Node::isEventNode() const {
    return event != nullptr;
}

const Event *Node::getEvent() const {
    if (event == nullptr){
        throw std::runtime_error("Attempting to access the event of a non-event node");
    }
    return event;
}

const ProbabilityNode *Node::getProbabilityFunction() const {
    if (probabilityFunction == nullptr){
        throw std::runtime_error("Attempting to access the probability function of a non-event node");
    }
    return probabilityFunction;
}
bool Node::operator==(const Node &e) const {
    if ((isEventNode() && !e.isEventNode()) || (isFactorNode() && !e.isFactorNode())){
        return false;
    }

    if (isEventNode()){
        return *getEvent() == *e.getEvent();
    }

    return *getProbabilityFunction() == *e.getProbabilityFunction();
}






