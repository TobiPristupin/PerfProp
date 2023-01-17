#ifndef BAYESPERF_CPP_PMUARCH_H
#define BAYESPERF_CPP_PMUARCH_H

/**
 * Collection of methods that deal with the underlying PMU architecture
 */
#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include <linux/perf_event.h>
#include "PmuEvent.h"

namespace PmuArch {

    size_t numProgrammableHPCs();

    /*
     * Obtains the perf_event_attr from an event, which can then be used to call perf_event_open for this
     * event. Returns optional because the conversion might fail.
     *
     */
    std::optional<perf_event_attr> getPerfEventAttr(const PmuEvent& event);
}

#endif
