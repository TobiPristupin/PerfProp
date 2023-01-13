#ifndef BAYESPERF_CPP_EVENTGRAPH_H
#define BAYESPERF_CPP_EVENTGRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "PmuEvent.h"
#include "Utils.h"
#include "Updaters.h"


/*
 * Graph where events are nodes, and directed edges represent statistical relationships between two events. If there
 * is a (directed) edge from e1 to e2, that means that the measured values of e1 should affect the values of e2. Thus,
 * if we obtain a sample for e1, we can use that sample to update the statistics of e2.
 *
 * Every edge in the graph contains a StatUpdaterFunc, a function that takes in the two events that touch that edge
 * as parameters, and updates the values of the destination event based on the value of the origin event.
 */
class EventGraph {
public:
    void addNode(const PmuEvent &event);

    /*
     * Adds an edge with `func` from `eventFrom` to `eventTo`. If the events have not been added to the graph yet,
     * this method will also add them.
     */
    void addEdge(const PmuEvent &eventFrom, const PmuEvent &eventTo, const StatUpdaterFunc& func);


private:

    class Edge {
    public:
        PmuEvent eventFrom, eventTo;
        StatUpdaterFunc updaterFunc;
        Edge(PmuEvent eventFrom, PmuEvent eventTo, StatUpdaterFunc updaterFunc)
        : eventFrom(std::move(eventFrom)), eventTo{std::move(eventTo)}, updaterFunc{std::move(updaterFunc)} {};

        bool operator==(const Edge &e) const {
            return this->eventTo == e.eventTo && this->eventFrom == e.eventFrom;
        }
    };

    /*
     * These functors are required by unordered_set.
     * Why didn't we specialize std::hash? See: https://stackoverflow.com/a/23188120/8402038
     */
    struct HashEdge { std::size_t operator()(const Edge& edge) const {
        std::size_t h = 0;
        hash_combine(h,edge.eventTo, edge.eventFrom);
        return h;
    }};

    struct EqualsEdge { std::size_t operator()(const Edge& e1, const Edge& e2) const {
            return e1.eventTo == e2.eventTo && e1.eventFrom == e2.eventFrom;
    }};

    std::unordered_map<PmuEvent, std::unordered_set<Edge, HashEdge, EqualsEdge>> graph;
};





#endif
