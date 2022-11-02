#ifndef BAYESPERF_CPP_PMUEVENT_H
#define BAYESPERF_CPP_PMUEVENT_H

#include <string>
#include <utility>

class PmuEvent {
public:

    enum Type {
        SOFTWARE,
        HARDWARE
    };

    std::string name;
    std::string modifiers;
    PmuEvent::Type type;

    PmuEvent(std::string name, PmuEvent::Type type, std::string modifiers = "");

    bool operator==(const PmuEvent& e) const;
};

namespace std {
    template <>
    struct hash<PmuEvent> {
        std::size_t operator()(const PmuEvent& event) const {
            return std::hash<string>()(event.name);
        }
    };
}
#endif //BAYESPERF_CPP_PMUEVENT_H
