#include "EventGraph.h"



void EventGraph::addNode(const PmuEvent &event, const Edge &edge) {
    if (graph.find(event) == graph.end()){
        graph[event] = std::unordered_set<Edge>();
    }
}
