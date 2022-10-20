#include <algorithm>
#include "include/ProbabilityFunction.h"

ProbabilityFunction::ProbabilityFunction(std::initializer_list<Event> events) {
    this->eventsInput = std::vector<Event>{events};
}

bool ProbabilityFunction::eventInInput(const Event& event) const {
    return std::find(eventsInput.begin(), eventsInput.end(), event) != eventsInput.end();
}
std::vector<Event> ProbabilityFunction::getEvents() const {
    return eventsInput;
}
