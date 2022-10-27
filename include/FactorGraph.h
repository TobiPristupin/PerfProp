#ifndef BAYESPERF_CPP_FACTORGRAPH_H
#define BAYESPERF_CPP_FACTORGRAPH_H


#include <unordered_map>
#include <unordered_set>
#include "Node.h"

//Could be using the unordered variants, but that would require writing a hash function for Node, which is not trivial.
//Since our set of events is always very small, the added complexity of using the ordered variants does not matter.
using FactorGraph = std::unordered_map<Node, std::unordered_set<Node>>;

void insert_into_graph(Node &u, Node &v, FactorGraph &graph);

/* Computes the markov blanket for a set of event nodes. If one of the nodes passed in is not an event node,
 * this function will raise a runtime_exception.
 * */
std::unordered_set<Node> markovBlanket(std::initializer_list<Node> eventNodes, const FactorGraph &graph);

#endif //BAYESPERF_CPP_FACTORGRAPH_H
