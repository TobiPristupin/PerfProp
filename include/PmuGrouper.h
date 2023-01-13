#ifndef BAYESPERF_CPP_PMUGROUPER_H
#define BAYESPERF_CPP_PMUGROUPER_H

#include <vector>
#include "PmuEvent.h"

namespace PmuGrouper {
    std::vector<std::vector<PmuEvent>> group(const std::vector<PmuEvent> &events, size_t maxGroupSize);
}


#endif
