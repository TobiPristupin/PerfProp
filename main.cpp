#include <vector>
#include <iostream>
#include "include/Node.h"
#include "include/Event.h"
#include "include/ProbabilityNode.h"
#include "include/FactorGraph.h"

FactorGraph generateGraph(const std::vector<Event> &events, std::vector<ProbabilityNode> &factors){
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

//Generates factor graph corresponding to fig. 1 pg. 1746 in https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf
FactorGraph figure1ExampleGraph(){
    std::vector<Event> events = {
            {1, "X1"}, {2, "X2"}, {3, "X3"},
            {4, "X4"}, {5, "X5"}, {6, "X6"},
            {7, "X7"}, {8, "X8"}, {9, "X9"},
    };

    ProbabilityFunction dummyFunc = [](auto p) -> double {return 0;};
    std::vector<ProbabilityNode> factors = {
            {{events[0], events[1], events[2]}, dummyFunc},

            {{events[0], events[1]}, dummyFunc},
            {{events[1], events[2]}, dummyFunc},

            {{events[0], events[3]}, dummyFunc},
            {{events[1], events[4]}, dummyFunc},
            {{events[2], events[5]}, dummyFunc},

            {{events[3], events[4]}, dummyFunc},
            {{events[4], events[5]}, dummyFunc},

            {{events[3], events[6]}, dummyFunc},
            {{events[4], events[7]}, dummyFunc},
            {{events[5], events[8]}, dummyFunc},

            {{events[6], events[7]}, dummyFunc},
            {{events[7], events[8]}, dummyFunc},
    };

    FactorGraph graph = generateGraph(events, factors);
    std::unordered_set<Node> blanket = markovBlanket({Node(&events[4])}, graph);
    for (const Node& n : blanket){
        std::cout << n.getEvent()->name << "\n";
    }

    return graph;
}


int main() {
    FactorGraph graph = figure1ExampleGraph();
    return 0;
}


