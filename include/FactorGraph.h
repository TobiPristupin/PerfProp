#ifndef BAYESPERF_CPP_FACTORGRAPH_H
#define BAYESPERF_CPP_FACTORGRAPH_H


#include <map>
#include <set>
#include "Node.h"

//Could be using the unordered variants, but that would require writing a hash function for Node, which is not trivial.
//Since our set of events is always very small, the added complexity of using the ordered variants does not matter.
using FactorGraph = std::map<Node, std::set<Node>>;

void insert_into_graph(Node &u, Node &v, FactorGraph &graph);

/* Computes the markov blanket for a set of event nodes. If one of the nodes passed in is not an event node,
 * this function will raise a runtime_exception.
 * */
std::set<Node> markovBlanket(std::initializer_list<Node> eventNodes);

#endif //BAYESPERF_CPP_FACTORGRAPH_H
