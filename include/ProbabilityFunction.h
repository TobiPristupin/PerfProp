#ifndef BAYESPERF_CPP_PROBABILITYFUNCTION_H
#define BAYESPERF_CPP_PROBABILITYFUNCTION_H

#include <vector>
#include "Event.h"

class ProbabilityFunction {
    std::vector<Event> eventsInput;

public:
    ProbabilityFunction(std::initializer_list<Event> events);
    bool eventInInput(const Event& event) const;
    std::vector<Event> getEvents() const;

    virtual double compute() const = 0;
};

#endif //BAYESPERF_CPP_PROBABILITYFUNCTION_H
