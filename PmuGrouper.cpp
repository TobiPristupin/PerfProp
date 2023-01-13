#include "PmuGrouper.h"

namespace PmuGrouper {

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
                currGroup.push_back(events[i]);
            }

            groups.push_back(currGroup);
        }

        return groups;
    }
}