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
        std::vector<PmuEvent> softwareGroup;
        size_t i = 0;
        while (i < events.size()) {
            std::vector<PmuEvent> currGroup;
            while (currGroup.size() < maxGroupSize && i < events.size()){
                if (events[i].getType() == PmuEvent::SOFTWARE){
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