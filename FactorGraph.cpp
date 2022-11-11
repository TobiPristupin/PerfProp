#include <stdexcept>
#include <iostream>
#include "include/FactorGraph.h"

void insert_into_graph(Node &u, Node &v, FactorGraph &graph){
    if (graph.find(u) == graph.end()){
        graph[u] = std::unordered_set<Node>();
    }
    graph[u].insert(v);
}

std::unordered_set<Node> markovBlanket(std::initializer_list<Node> eventNodes, const FactorGraph &graph){
    //Reference for Markov Blanket: https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf p. 1747
    std::unordered_set<Node> blanket;
    for (const Node& node : eventNodes){
        if (!node.isEventNode()){
            throw std::runtime_error("eventNodes must be only event nodes.");
        }

        for (const Node& factorChild : graph.at(node)){
            for (const Node& eventFactorChild : graph.at(factorChild)){
                blanket.insert(eventFactorChild);
            }
        }
    }

    //the blanket might now contain the eventNodes, which we don't want to include. We must remove them from the set.
    for (const Node& node : eventNodes){
        blanket.erase(node);
    }

    return blanket;
}

bool eventsOverlap(std::initializer_list<Node> e1, std::initializer_list<Node> e2, const FactorGraph &graph) {
    std::unordered_set<Node> b1 = markovBlanket(e1, graph), b2 = markovBlanket(e2, graph);
    for (const Node &node : b1){
        if (b2.find(node) != b2.end()){
            return true;
        }
    }

    return false;
}

FactorGraph generateGraph(const std::vector<PmuEvent> &events, std::vector<ProbabilityNode> &factors){
    FactorGraph graph;
    for (const ProbabilityNode &factor : factors){
        Node factorNode(&factor);
        for (auto const &event : events){
            if (factor.eventInInput(event)){
                Node eventNode(&event);
                insert_into_graph(factorNode, eventNode, graph);
                insert_into_graph(eventNode, factorNode, graph);
            }
        }
    }

    return graph;
}
