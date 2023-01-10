//#include <stdexcept>
//#include "include/FactorGraph.h"
//
//void insert_into_graph(const FactorGraphNode &u, const FactorGraphNode &v, FactorGraph &graph){
//    if (graph.find(u) == graph.end()){
//        graph[u] = std::unordered_set<FactorGraphNode>();
//    }
//    graph[u].insert(v);
//}
//
//std::unordered_set<PmuEvent> getEventNeighbors(const PmuEvent &event, const FactorGraph &graph){
//    std::unordered_set<PmuEvent> neighbors;
//    for (const FactorGraphNode &factorNode : graph.at(event)){
//        for (const FactorGraphNode &eventNode : graph.at(factorNode)){
//            if (!std::holds_alternative<PmuEvent>(eventNode)){
//                throw std::runtime_error("Expected an event node. Factor graph may be malformed.");
//            }
//
//            PmuEvent neighbor = std::get<PmuEvent>(eventNode);
//            if (neighbor == event){
//                continue;
//            }
//            neighbors.insert(neighbor);
//        }
//    }
//
//    return neighbors;
//}
//
///*
// * Given a set of events, this function will greedily choose elements from {events} that are not in blanketAvoid until it reaches
// * subsetSize elements. This will find a subset of events that maximizes the size of the markov blanket (not necessarily optimal).
// */
//std::unordered_set<PmuEvent> subsetMaximizingBlanket(const std::unordered_set<PmuEvent> &events, int subsetSize, const FactorGraph &graph, std::unordered_set<PmuEvent> blanketAvoid){
//    if (subsetSize > events.size()){
//        throw std::runtime_error("Subset size cannot be greater than events given");
//    }
//
//    if (subsetSize == events.size()){
//        return events;
//    }
//
//    std::unordered_set<PmuEvent> subset;
//    subset.reserve(subsetSize);
//    for (const PmuEvent &event : events){
//        if (subset.size() == subsetSize){
//            return subset;
//        }
//
//        //Intuition: The blanket of a set C of elements cannot contain any element in C by definition. So If the blanket
//        //of C contains event e_j, and we add e_j to C, then the new blanket of C will lose e_j. We want to avoid
//        //"losing" elements in our blanket, because we want our blanket to be as big as possible.
//        if (subset.find(event) == subset.end() && blanketAvoid.find(event) == blanketAvoid.end()){
//            subset.insert(event);
//            blanketAvoid = markovBlanket(subset, graph);
//        }
//    }
//
//
//    if (subset.size() < subsetSize){
//        //still have to add elements, even if they are already in the blanket.
//        for (const PmuEvent &event : events){
//            if (subset.size() == subsetSize){
//                break;
//            }
//
//            if (subset.find(event) == subset.end()){
//                subset.insert(event);
//            }
//        }
//    }
//
//    return subset;
//}
//
//std::unordered_set<PmuEvent> subsetMaximizingBlanket(const std::unordered_set<PmuEvent> &events, int subsetSize, const FactorGraph &graph){
//    return subsetMaximizingBlanket(events, subsetSize, graph,  std::unordered_set<PmuEvent>());
//}
//
////TODO: Algorithm still needs improvements when handling cases with a disconnected graph.
//std::vector<std::unordered_set<PmuEvent>> performGrouping(const std::unordered_set<PmuEvent> &events, int groupSize, int overlapSize, const FactorGraph &graph) {
//    if (groupSize <= 1){
//        throw std::runtime_error("Group size must be >= 1");
//    }
//
//    if (overlapSize >= groupSize){
//        throw std::runtime_error("Need overlapSize < groupSize. Otherwise the grouping algorithm will not terminate");
//    }
//
//    if (events.size() < groupSize){
//        return {events};
//    }
//
//    std::vector<std::unordered_set<PmuEvent>> groups;
//    std::unordered_set<PmuEvent> groupedEvents;
//    std::unordered_set<PmuEvent> initialGroup = subsetMaximizingBlanket(events, groupSize, graph);
//    groupedEvents.insert(initialGroup.begin(), initialGroup.end());
//    groups.push_back(initialGroup);
//
//    while (groupedEvents.size() < events.size()){
//        std::unordered_set<PmuEvent> prevGroup = groups.back();
//        std::unordered_set<PmuEvent> newGroup;
//        newGroup.reserve(groupSize);
//        std::unordered_set<PmuEvent> overlapSubset = subsetMaximizingBlanket(prevGroup, overlapSize, graph);
//        newGroup.insert(overlapSubset.begin(), overlapSubset.end());
//
//        std::unordered_set<PmuEvent> currentBlanket = markovBlanket(newGroup, graph);
//        //First pass: add ungrouped events that are not in the current blanket.
//        for (auto it = events.begin(); it != events.end() && newGroup.size() < groupSize; ++it){
//            if (groupedEvents.find(*it) == groupedEvents.end() && currentBlanket.find(*it) == currentBlanket.end()){
//                newGroup.insert(*it);
//                groupedEvents.insert(*it);
//                currentBlanket = markovBlanket(newGroup, graph);
//            }
//        }
//
//        //Second pass: add ungrouped events, regardless of if they are in the current blanket, since we need to fill
//        //our group anyways.
//        for (auto it = events.begin(); it != events.end() && newGroup.size() < groupSize; ++it){
//            if (groupedEvents.find(*it) == groupedEvents.end()){
//                newGroup.insert(*it);
//                groupedEvents.insert(*it);
//            }
//        }
//
//        groups.push_back(newGroup);
//    }
//
//    return groups;
//}
//
//std::unordered_set<PmuEvent> markovBlanket(const std::unordered_set<PmuEvent>& eventNodes, const FactorGraph &graph){
//    //Reference for Markov Blanket: https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf p. 1747
//    std::unordered_set<PmuEvent> blanket;
//    for (const PmuEvent& eventNode : eventNodes){
//        std::unordered_set<PmuEvent> neighbors = getEventNeighbors(eventNode, graph);
//        blanket.insert(neighbors.begin(), neighbors.end());
//    }
//
//    //the blanket might now contain the eventNodes, which we don't want to include. We must remove them from the set.
//    for (const PmuEvent& node : eventNodes){
//        blanket.erase(node);
//    }
//
//    return blanket;
//}
//
//
//
//std::unordered_set<PmuEvent> statisticalDependenceSet(const std::unordered_set<PmuEvent> &c1, const std::unordered_set<PmuEvent> &c2, const FactorGraph &graph) {
//    std::unordered_set<PmuEvent> b1 = markovBlanket(c1, graph), b2 = markovBlanket(c2, graph);
//    std::unordered_set<PmuEvent> dependence;
//    for (const PmuEvent &node : b1){
//        if (b2.find(node) != b2.end()){
//            dependence.insert(node);
//        }
//    }
//
//    return dependence;
//}
//
//FactorGraph generateGraph(const std::vector<PmuEvent> &events, const std::vector<ProbabilityNode> &factors){
//    FactorGraph graph;
//    for (const ProbabilityNode &factor : factors){
//        for (const PmuEvent &event : events){
//            if (factor.eventInInput(event)){
//                insert_into_graph(factor, event, graph);
//                insert_into_graph(event, factor, graph);
//            }
//        }
//    }
//
//    return graph;
//}
