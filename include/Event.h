#ifndef BAYESPERF_CPP_EVENT_H
#define BAYESPERF_CPP_EVENT_H

#include <string>
#include <utility>

class Event {
public:

    int id;
    std::string name;

    Event(int id, std::string name);

    bool operator==(const Event& e) const;
};

namespace std {
    template <>
    struct hash<Event> {
        std::size_t operator()(const Event& event) const {
            return std::hash<int>()(event.id);
        }
    };
}
#endif //BAYESPERF_CPP_EVENT_H
