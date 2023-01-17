#ifndef BAYESPERF_CPP_PMUARCH_H
#define BAYESPERF_CPP_PMUARCH_H

/**
 * Collection of methods that deal with the underlying PMU architecture
 */
#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
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

    /*
     * For every event group, calls perf_event_open on every event, and returns a mapping of file descriptor -> event.
     * perf_event_open is called with disabled=1, so no sampling will be initiated by this function.
     * This function will simply register events with the perf interface for later usage.
     *
     * pid is the process id that events should be registered to.
     *
     * Since an event may be repeated across groups, this function can generate multiple fds mapping to the same
     * event.
     *
     * NOTE: A PmuEvent may fail to be encoded into a valid event for perf_event_open. In that case, this function
     * will log an error message and skip placing that event in the map.
     */
    std::unordered_map<int, PmuEvent> perfOpenEvents(const std::vector<std::vector<PmuEvent>>& eventGroups, pid_t pid);
}

#endif
