#ifndef BAYESPERF_CPP_EVENTGRAPH_H
#define BAYESPERF_CPP_EVENTGRAPH_H

#include <unordered_map>
#include <unordered_set>
#include "PmuEvent.h"
#include "Edge.h"


class EventGraph {
public:
    void addNode(const PmuEvent &event, const Edge &edge);
private:
    std::unordered_map<PmuEvent, std::unordered_set<Edge>> graph;
};


#endif
