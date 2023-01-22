#ifndef BAYESPERF_CPP_PMUEVENT_H
#define BAYESPERF_CPP_PMUEVENT_H

#include <string>
#include <utility>

using Statistic = long double;
using SampleCount = uint64_t;

class PmuEvent {
public:

    enum Type {
        SOFTWARE,
        HARDWARE
    };

    struct Stats {

        Stats(Statistic mean, Statistic variance, SampleCount samples)
        : mean(mean), variance(variance), samples(samples) {}

        Stats() = default;

        Statistic mean{};
        Statistic variance{};
        SampleCount samples{};
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
