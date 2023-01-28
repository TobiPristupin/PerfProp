#ifndef PERFPROP_PMUEVENT_H
#define PERFPROP_PMUEVENT_H

#include <string>
#include <utility>
#include <chrono>
#include <sstream>
#include <linux/perf_event.h>
#include "PerfStats.h"

using Statistic = long double;

class PmuEvent {
public:

    using Type = perf_type_id;

    struct Stats {

        Stats() = default;

        std::string toString() const {
            std::stringstream ss;
            ss << "meanCountPerMillis=" << meanCountsPerMillis << " " << "varPerMillis=" << varianceCountPerMillis
            << " count=" << count << " timeEnabled=" << timeEnabled.count() << " samples=" << samples
            << " sampleSumSquared=" << sampleSumSquared  << " propagations=" << propagations;
            return ss.str();
        }

        EventCount count{};
        EventCount samples{};
        EventCount propagations{};
        Nanosecs timeEnabled{};
        Statistic meanCountsPerMillis{};
        Statistic varianceCountPerMillis{};
        Statistic sampleSumSquared{}; //Useful for variance calculations
    };

    /*
     * name may include modifiers. Accepted format would be eventName[:modifiers]
     */
    PmuEvent(std::string name, PmuEvent::Type type);

    std::string getName() const;
    std::string getModifiers() const;
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
