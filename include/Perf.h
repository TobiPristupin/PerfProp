#ifndef BAYESPERF_CPP_PERF_H
#define BAYESPERF_CPP_PERF_H

/**
 * Collection of objects that interface with the perf subsystem in the kernel
 */
#include <cstddef>
#include <cstdint>
#include <string>
#include <optional>
#include <map>
#include <vector>
#include <linux/perf_event.h>
#include <perfmon/pfmlib.h>
#include "PmuEvent.h"
#include "PerfStats.h"

namespace Perf {

    struct Sample {
        EventCount value;
        Nanosecs timeEnabled;
        Nanosecs timeRunning;
    };

    pfm_pmu_info_t getDefaultPmu();

    size_t numProgrammableHPCs();

    /*
     * Obtains the perf_event_attr from an event, which can then be used to call perf_event_open for this
     * event. Returns optional because the conversion might fail.
     *
     */
    std::optional<perf_event_attr> getPerfEventAttr(const PmuEvent& event);

    /*
     * For every event group, calls perf_event_open on every event.
     * RETURNS a mapping of file descriptor -> event, as well as a vector of the file descriptors for every group leader
     * pid is the process id that events should be registered to.
     *
     * perf_event_open is called with disabled=1, so no sampling will be initiated by this function.
     * This function will simply register events with the perf interface for later usage.
     *
     * Since an event may be repeated across groups, this function can generate multiple fds mapping to the same
     * event.
     *
     * NOTE: A PmuEvent may fail to be encoded into a valid event for perf_event_open. In that case, this function
     * will log an error message and skip placing that event in the map.
     */
    std::pair<std::map<int, PmuEvent>, std::vector<int>> perfOpenEvents(const std::vector<std::vector<PmuEvent>>& eventGroups, pid_t pid);

    /*
     * Events are enabled by default by setting enable_on_exec=1 on perfOpenEvents, so there should be no reason
     * to call this function.
     */
    void enableEvents(const std::vector<int>& fds);

    void disableEvents(const std::vector<int>& fds);

    void resetCounter(int fd);

    void closeFds(std::map<int, PmuEvent>& fds);

    /*
     * Might throw std::runtime_error
     */
    Sample readSample(int groupLeaderFd);
}

#endif
