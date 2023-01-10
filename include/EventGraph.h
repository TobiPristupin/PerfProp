#ifndef BAYESPERF_CPP_EVENTGRAPH_H
#define BAYESPERF_CPP_EVENTGRAPH_H

#include <unordered_map>
#include <unordered_set>
#include "PmuEvent.h"
#include "Utils.h"


/*
 * Function to update statistic takes in the value of the stat to be updated, and the value for another
 * stat that is statistically related to it.
 */
using StatUpdaterFunc = double(*)(double stat_to_update, double related_stat);


class EventGraph {
public:
    void addNode(const PmuEvent &event);

    /*
     * Adds an edge with `func` from `eventFrom` to `eventTo`. If the events have not been added to the graph yet,
     * this method will also add them.
     */
    void addEdge(const PmuEvent &eventFrom, const PmuEvent &eventTo, StatUpdaterFunc func);

private:

    class Edge {
    public:
        PmuEvent eventTo;
        StatUpdaterFunc updaterFunc;
        Edge(PmuEvent eventTo, StatUpdaterFunc updaterFunc) : eventTo{std::move(eventTo)}, updaterFunc{updaterFunc} {};

        bool operator==(const Edge &e) const {
            return this->eventTo == e.eventTo && this->updaterFunc == e.updaterFunc;
        }
    };

    /*
     * These functors are required by unordered_set.
     * Why didn't we specialize std::hash? See: https://stackoverflow.com/a/23188120/8402038
     */
    struct HashEdge { std::size_t operator()(const Edge& edge) const {
        std::size_t h = 0;
        hash_combine(h,edge.eventTo, edge.updaterFunc);
        return h;
    }};

    struct EqualsEdge { std::size_t operator()(const Edge& e1, const Edge& e2) const {
            return e1.eventTo == e2.eventTo && e1.updaterFunc == e2.updaterFunc;
    }};

    std::unordered_map<PmuEvent, std::unordered_set<Edge, HashEdge, EqualsEdge>> graph;
};





#endif
