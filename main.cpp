#include <vector>
#include "include/Node.h"
#include "include/Event.h"
#include "include/ProbabilityFunction.h"
#include "include/FactorGraph.h"

FactorGraph generateGraph(const std::vector<Event> &events, std::vector<ProbabilityFunction> &factors){
    FactorGraph graph;
    for (const ProbabilityFunction &factor : factors){
        Node factorNode(&factor);
        for (auto const &event : events){
            if (factor.eventInInput(event)){
                Node eventNode(&event);
                insert_into_graph(factorNode, eventNode, graph);
                insert_into_graph(eventNode, factorNode, graph);
            }
        }
    }
}


int main() {
    return 0;
}


