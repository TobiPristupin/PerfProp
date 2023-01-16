#ifndef BAYESPERF_CPP_PMUGROUPER_H
#define BAYESPERF_CPP_PMUGROUPER_H

#include <vector>
#include "PmuEvent.h"

namespace PmuGrouper {

    using EventVector = std::vector<PmuEvent>;

    /*
     * Splits events into a vector of hardware events and a vector of software events. Returned in that order.
     */
    std::pair<EventVector, EventVector> splitHardwareSoftware(const EventVector& events);

    /*
     * Groups events into an arbitrary number of groups, where each group has size at most maxGroupSize. Events are
     * grouped to maximize effective statistical dependencies across groups. This means that an event may be repeated
     * across groups.
     *
     * NOTE: Only call with PmuEvent::Type::HARDWARE events. There is no reason to group software events, as those
     * have no constraints and can be scheduled all at once.
     */
    std::vector<EventVector> group(const EventVector& events, size_t maxGroupSize);
}


#endif
