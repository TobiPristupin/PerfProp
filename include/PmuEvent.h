#ifndef BAYESPERF_CPP_PMUEVENT_H
#define BAYESPERF_CPP_PMUEVENT_H

#include <string>
#include <utility>
#include <chrono>
#include <sstream>
#include "PerfStats.h"

using Statistic = long double;

class PmuEvent {
public:

    enum Type {
        SOFTWARE,
        HARDWARE
    };

    struct Stats {

        Stats(EventCount count, Nanosecs &timeEnabled, Statistic meanCountsPerMillis,
              Statistic varianceCountPerMillis, EventCount samples, EventCount propagations) : count(count), timeEnabled(timeEnabled),
                                                                  meanCountsPerMillis(meanCountsPerMillis),
                                                                  varianceCountPerMillis(varianceCountPerMillis),
                                                                  samples(samples), propagations(propagations) {}
        Stats() = default;

        std::string toString() const{
            std::stringstream ss;
            ss << "meanCountPerMillis=" << meanCountsPerMillis << " " << "varPerMillis=" << varianceCountPerMillis
            << " count=" << count << " timeEnabled=" << timeEnabled.count() << " samples=" << samples << " propagations=" << propagations;
            return ss.str();
        }

        EventCount count{};
        EventCount samples{};
        EventCount propagations{};
        Nanosecs timeEnabled{};
        Statistic meanCountsPerMillis{};
        Statistic varianceCountPerMillis{};
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
    friend std::ostream& operator<< (std::ostream& os, const PmuEvent::Stats& obj) {
        return os << obj.toString();
    }

    friend class StatPrinter;
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
