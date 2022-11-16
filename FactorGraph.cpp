#include <stdexcept>
#include "include/FactorGraph.h"

void insert_into_graph(const FactorGraphNode &u, const FactorGraphNode &v, FactorGraph &graph){
    if (graph.find(u) == graph.end()){
        graph[u] = std::unordered_set<FactorGraphNode>();
    }
    graph[u].insert(v);
}

std::unordered_set<PmuEvent> markovBlanket(std::initializer_list<PmuEvent> eventNodes, const FactorGraph &graph){
    //Reference for Markov Blanket: https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf p. 1747
    std::unordered_set<PmuEvent> blanket;
    for (const PmuEvent& eventNode : eventNodes){
        for (const FactorGraphNode &factorChild : graph.at(eventNode)){
            for (const FactorGraphNode& eventFactorChild : graph.at(factorChild)){
                if (!std::holds_alternative<PmuEvent>(eventFactorChild)){
                    throw std::runtime_error("Markov blanket contains non-event nodes. Graph may be malformed.");
                }

                blanket.insert(std::get<PmuEvent>(eventFactorChild));
            }
        }
    }

    //the blanket might now contain the eventNodes, which we don't want to include. We must remove them from the set.
    for (const PmuEvent& node : eventNodes){
        blanket.erase(node);
    }

    return blanket;
}

bool eventGroupsRelated(std::initializer_list<PmuEvent> c1, std::initializer_list<PmuEvent> c2, const FactorGraph &graph) {
    std::unordered_set<PmuEvent> b1 = markovBlanket(c1, graph), b2 = markovBlanket(c2, graph);
    for (const PmuEvent &node : b1){
        if (b2.find(node) != b2.end()){
            return true;
        }
    }

    return false;
}

FactorGraph generateGraph(const std::vector<PmuEvent> &events, const std::vector<ProbabilityNode> &factors){
    FactorGraph graph;
    for (const ProbabilityNode &factor : factors){
        for (const PmuEvent &event : events){
            if (factor.eventInInput(event)){
                insert_into_graph(factor, event, graph);
                insert_into_graph(event, factor, graph);
            }
        }
    }

    return graph;
}
