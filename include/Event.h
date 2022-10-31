#ifndef BAYESPERF_CPP_EVENT_H
#define BAYESPERF_CPP_EVENT_H

#include <string>
#include <utility>

class Event {
public:

    std::string name;
    std::string modifiers;

    Event(std::string name, std::string modifiers = "");

    bool operator==(const Event& e) const;
};

namespace std {
    template <>
    struct hash<Event> {
        std::size_t operator()(const Event& event) const {
            return std::hash<string>()(event.name);
        }
    };
}
#endif //BAYESPERF_CPP_EVENT_H
