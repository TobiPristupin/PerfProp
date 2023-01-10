#include <iostream>
#include "EventGraph.h"

void EventGraph::addNode(const PmuEvent &event) {
    if (graph.find(event) == graph.end()){
        graph[event] = std::unordered_set<Edge, HashEdge, EqualsEdge>();
    } else {
        std::cerr << "Warning: Attempting to add an already existing event. This has no effect\n";
    }
}

void EventGraph::addEdge(const PmuEvent &eventFrom, const PmuEvent &eventTo, StatUpdaterFunc func) {
    if (graph.find(eventFrom) == graph.end()){
        graph[eventFrom] = std::unordered_set<Edge, HashEdge, EqualsEdge>();
    }

    if (graph.find(eventTo) == graph.end()){
        graph[eventFrom] = std::unordered_set<Edge, HashEdge, EqualsEdge>();
    }

    graph[eventFrom].insert(Edge(eventTo, func));
}


