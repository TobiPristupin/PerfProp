#include <stdexcept>
#include "PmuGrouper.h"

namespace PmuGrouper {

    std::pair<EventVector, EventVector> splitHardwareSoftware(const EventVector &events) {
        EventVector hardware, software;
        for (const PmuEvent& event : events){
            if (event.getType() == PmuEvent::HARDWARE){
                hardware.push_back(event);
            } else {
                software.push_back(event);
            }
        }

        return std::make_pair(hardware, software);
    }

    /*
     * Group events as they come in.
     * TODO: modify grouping algorithm to take into account statistical dependencies
     */
    std::vector<std::vector<PmuEvent>> group(const std::vector<PmuEvent> &events, size_t maxGroupSize) {
        std::vector<std::vector<PmuEvent>> groups;
        size_t i = 0;
        while (i < events.size()) {
            std::vector<PmuEvent> currGroup;
            size_t nextGroupEnd = std::min(events.size(), i + maxGroupSize);
            for (; i < nextGroupEnd; i++){
                if (events[i].getType() == PmuEvent::SOFTWARE){
                    throw std::runtime_error("Attempting to group a software event. See header function comment.");
                }
                currGroup.push_back(events[i]);
            }

            groups.push_back(currGroup);
        }

        return groups;
    }

}