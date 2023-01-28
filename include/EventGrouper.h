#ifndef PERFPROP_EVENTGROUPER_H
#define PERFPROP_EVENTGROUPER_H

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
     * PmuEvent::Type::SOFTWARE events will be grouped all together in one group, which may be larger than maxGroupSize.
     * This will be the last group in the returned vector.
     * This is because there is no need to split software events into groups. Software events can be scheduled all at
     * once since they are measured in software and don't physically use any PMU counter.
     *
     */
    std::vector<EventVector> group(const EventVector& events, size_t maxGroupSize);
}


#endif
