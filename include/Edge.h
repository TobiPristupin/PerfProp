#ifndef BAYESPERF_CPP_EDGE_H
#define BAYESPERF_CPP_EDGE_H

#include <utility>
#include "PmuEvent.h"
#include "Utils.h"

/*
 * Function to update statistic takes in the value of the stat to be updated, and the value for another
 * stat that is statistically related to it.
 */
using StatUpdaterFunc = double(*)(double stat_to_update, double related_stat);

class Edge {
public:
    Edge(PmuEvent eventTo, StatUpdaterFunc updaterFunc) : eventTo{std::move(eventTo)}, updaterFunc{updaterFunc} {};

    PmuEvent getEventTo() const {
        return eventTo;
    }

    StatUpdaterFunc getUpdaterFunction() const {
        return updaterFunc;
    }

    bool operator==(const Edge& e) const {
        return this->eventTo == e.eventTo && this->updaterFunc == e.updaterFunc;
    }

private:
    PmuEvent eventTo;
    StatUpdaterFunc updaterFunc;
};

namespace std {
    template <>
    struct hash<Edge> {
        std::size_t operator()(const Edge& edge) const {
            std::size_t h = 0;
            hash_combine(h, edge.getEventTo(), edge.getUpdaterFunction());
            return h;
        }
    };
}

#endif
