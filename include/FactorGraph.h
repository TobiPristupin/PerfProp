#ifndef BAYESPERF_CPP_FACTORGRAPH_H
#define BAYESPERF_CPP_FACTORGRAPH_H


#include <unordered_map>
#include <unordered_set>
#include "Node.h"

using FactorGraph = std::unordered_map<Node, std::unordered_set<Node>>;

/*
 * Generates a graph given a list of events and a list of factors that map to events
 */
FactorGraph generateGraph(const std::vector<PmuEvent> &events, std::vector<ProbabilityNode> &factors);

void insert_into_graph(Node &u, Node &v, FactorGraph &graph);

/* Computes the markov blanket for a set of event nodes. If one of the nodes passed in is not an event node,
 * this function will raise a runtime_exception.
 * */
std::unordered_set<Node> markovBlanket(std::initializer_list<Node> eventNodes, const FactorGraph &graph);

/**
 * Returns true if there is a statistical overlap between e1 and e2. e1 and e2 must be event nodes. There is overlap
 * if the intersection of their markov blankets is non-empty.
 */
bool eventsOverlap(std::initializer_list<Node> e1, std::initializer_list<Node> e2, const FactorGraph &graph);

#endif //BAYESPERF_CPP_FACTORGRAPH_H
