#include <stdexcept>
#include "EventGrouper.h"

namespace PmuGrouper {

    std::pair<EventVector, EventVector> splitHardwareSoftware(const EventVector &events) {
        EventVector hardware, software;
        for (const PmuEvent& event : events){
            if (event.getType() == PmuEvent::Type::PERF_TYPE_SOFTWARE){
                software.push_back(event);
            } else {
                hardware.push_back(event);
            }
        }

        return std::make_pair(hardware, software);
    }

    /*
     * Group events as they come in.
     * TODO: modify grouping algorithm to take into account statistical dependencies
     * Note for future grouping algorithm: need to take into account if an event group cannot be scheduled because
     * it does not meet the hardware's constraints (for example, two events in the same group can only be measured
     * in the same counter). When that happens, perf_events_open will return EINVAL. To account for that, the logic of
     * grouping and opening events, which is now separate, will probably have to be joined.
     */
    std::vector<std::vector<PmuEvent>> group(const std::vector<PmuEvent> &events, size_t maxGroupSize) {
        std::vector<std::vector<PmuEvent>> groups;
        std::vector<PmuEvent> softwareGroup;
        size_t i = 0;
        while (i < events.size()) {
            std::vector<PmuEvent> currGroup;
            while (currGroup.size() < maxGroupSize && i < events.size()){
                if (events[i].getType() == PmuEvent::Type::PERF_TYPE_SOFTWARE){
                    softwareGroup.push_back(events[i]);
                } else {
                    currGroup.push_back(events[i]);
                }

                i++;
            }

            if (!currGroup.empty()){
                groups.push_back(currGroup);
            }
        }

        if (!softwareGroup.empty()){
            groups.push_back(softwareGroup);
        }

        return groups;
    }

}