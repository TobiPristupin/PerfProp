#include <algorithm>
#include <functional>
#include <utility>
#include "include/ProbabilityNode.h"

bool ProbabilityNode::eventInInput(const Event& event) const {
    return std::find(eventsInput.begin(), eventsInput.end(), event) != eventsInput.end();
}

std::vector<Event> ProbabilityNode::getEvents() const {
    return eventsInput;
}

ProbabilityNode::ProbabilityNode(std::initializer_list<Event> events, ProbabilityFunction function) : function(function) {
    this->eventsInput = std::vector<Event>{events};
}

bool ProbabilityNode::operator==(const ProbabilityNode &node) const {
    return this->eventsInput == node.eventsInput && function == node.function;
}

double ProbabilityNode::call() {
    return function(eventsInput);
}
ProbabilityFunction ProbabilityNode::getFunction() const {
    return function;
}


