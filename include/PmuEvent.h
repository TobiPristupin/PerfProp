#ifndef BAYESPERF_CPP_PMUEVENT_H
#define BAYESPERF_CPP_PMUEVENT_H

#include <string>
#include <utility>
#include <optional>

class PmuEvent {
public:

    enum Type {
        SOFTWARE,
        HARDWARE
    };

    const std::string &getName() const;

    const std::string &getModifiers() const;

    Type getType() const;

    std::optional<double> getMean() const;

    std::optional<double> getVariance() const;

    PmuEvent(std::string name, PmuEvent::Type type, std::string modifiers = "");

    bool operator==(const PmuEvent& e) const;

    void setMean(double mean);

    void setVariance(double variance);

private:
    std::string name;
    std::string modifiers;
    PmuEvent::Type type;

    //optional because they may be uninitialized until we receive our first sample
    std::optional<double> mean;
    std::optional<double> variance;
};

namespace std {
    template <>
    struct hash<PmuEvent> {
        std::size_t operator()(const PmuEvent& event) const {
            return std::hash<string>()(event.getName());
        }
    };
}
#endif //BAYESPERF_CPP_PMUEVENT_H
