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

/**
 * In a factor graph, every event node is connected to only probability nodes, which in turn are connected to
 * only event nodes (i.e. the graph is bipartite for event and probability nodes). Sometimes for a given event
 * node e we want all event nodes that are "neighbors" of e, where we skip probability nodes (treat them as edges).
 */
std::unordered_set<PmuEvent> getEventNeighbors(const PmuEvent &event, const FactorGraph &graph);

/**
 * Given a list of events, this function will group them into multiple groups of size {groupSize}, such that
 * consecutive groups have a statistical dependence. Explained in more detail in dropbox doc linked in readme.
 */
std::vector<std::unordered_set<PmuEvent>> performGrouping(const std::unordered_set<PmuEvent>& events, int groupSize, int overlapSize, const FactorGraph &graph);

/* Computes the markov blanket for a set of event nodes. If one of the nodes passed in is not an event node,
 * this function will raise a runtime_exception.
 * */
std::unordered_set<PmuEvent> markovBlanket(const std::unordered_set<PmuEvent>& eventNodes, const FactorGraph &graph);

/**
 * Returns the statistical dependence between c1 and c2. This is given by the intersection of the markov blankets of
 * c1 and c2
 */
std::unordered_set<PmuEvent> statisticalDependenceSet(const std::unordered_set<PmuEvent>& c1, const std::unordered_set<PmuEvent>& c2, const FactorGraph &graph);



#endif //BAYESPERF_CPP_FACTORGRAPH_H
