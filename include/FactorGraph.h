#ifndef BAYESPERF_CPP_FACTORGRAPH_H
#define BAYESPERF_CPP_FACTORGRAPH_H


#include <unordered_map>
#include <unordered_set>
#include <variant>
#include "PmuEvent.h"
#include "ProbabilityNode.h"

using FactorGraphNode = std::variant<PmuEvent, ProbabilityNode>;

using FactorGraph = std::unordered_map<FactorGraphNode, std::unordered_set<FactorGraphNode>>;

/*
 * Generates a graph given a list of events and a list of factors that map to events
 */
FactorGraph generateGraph(const std::vector<PmuEvent> &events, const std::vector<ProbabilityNode> &factors);

void insert_into_graph(FactorGraphNode &u, FactorGraphNode &v, FactorGraph &graph);

/* Computes the markov blanket for a set of event nodes. If one of the nodes passed in is not an event node,
 * this function will raise a runtime_exception.
 * */
std::unordered_set<PmuEvent> markovBlanket(std::initializer_list<PmuEvent> eventNodes, const FactorGraph &graph);

/**
 * Returns true if there is a statistical dependence between c1 and c2. There is overlap
 * if the intersection of their markov blankets is non-empty.
 */
bool eventGroupsRelated(std::initializer_list<PmuEvent> c1, std::initializer_list<PmuEvent> c2, const FactorGraph &graph);

#endif //BAYESPERF_CPP_FACTORGRAPH_H
