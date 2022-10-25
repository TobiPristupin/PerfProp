#include "include/FactorGraph.h"

void insert_into_graph(Node &u, Node &v, FactorGraph &graph){
    if (graph.find(u) == graph.end()){
        graph[u] = std::set<Node>();
    }
    graph[u].insert(v);
}

std::set<Node> markovBlanket(std::initializer_list<Node> eventNodes){
    std::set<Node> blanket;
    return blanket;
}