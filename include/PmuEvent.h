#ifndef BAYESPERF_CPP_PMUEVENT_H
#define BAYESPERF_CPP_PMUEVENT_H

#include <string>
#include <utility>
#include <chrono>
#include "PerfStats.h"

using Statistic = long double;

class PmuEvent {
public:

    enum Type {
        SOFTWARE,
        HARDWARE
    };

    struct Stats {

        Stats(EventCount count, Nanosecs &timeEnabled, Statistic meanCountsPerNs,
              Statistic varianceCountPerNs, EventCount samples) : count(count), timeEnabled(timeEnabled),
                                                                  meanCountsPerNs(meanCountsPerNs),
                                                                  varianceCountPerNs(varianceCountPerNs),
                                                                  samples(samples) {}
        Stats() = default;

        EventCount count{};
        EventCount samples{};
        Nanosecs timeEnabled{};
        Statistic meanCountsPerNs{};
        Statistic varianceCountPerNs{};
    };

    /*
     * name may include modifiers. Accepted format would be eventName[:modifiers]
     */
    PmuEvent(std::string name, PmuEvent::Type type);

    const std::string &getName() const;
    const std::string &getModifiers() const;
    Type getType() const;

    bool operator==(const PmuEvent& e) const;
    bool operator<(const PmuEvent &e) const;

private:
    std::string name;
    std::string modifiers;
    PmuEvent::Type type;
};

namespace std {
    template <>
    struct hash<PmuEvent> {
        std::size_t operator()(const PmuEvent& event) const {
            return std::hash<string>()(event.getName());
        }
    };
}
#endif
