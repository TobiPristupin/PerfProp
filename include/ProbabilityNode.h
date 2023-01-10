//#ifndef BAYESPERF_CPP_PROBABILITYNODE_H
//#define BAYESPERF_CPP_PROBABILITYNODE_H
///*
// * FacttorNode needs to know how to update variance
// */
//
//#include <vector>
//#include <functional>
//#include "PmuEvent.h"
//
//using StatisticUpdaterFunc = double(*)(double curr_stat_1, double old_stat_2);
//
//class ProbabilityNode {
//    std::vector<PmuEvent> eventsInput;
//    StatisticUpdaterFunc function;
//
//public:
//    ProbabilityNode(std::initializer_list<PmuEvent> events, StatisticUpdaterFunc function);
//    bool eventInInput(const PmuEvent& event) const;
//    std::vector<PmuEvent> getEvents() const;
//    StatisticUpdaterFunc getUpdaterFunction() const;
//    double call();
//
//    bool operator==(const ProbabilityNode& node) const;
//};
//
//namespace std {
//    template <>
//    struct hash<ProbabilityNode> {
//        std::size_t operator()(const ProbabilityNode& node) const {
//            return std::hash<StatisticUpdaterFunc>()(node.getUpdaterFunction());
//        }
//    };
//}
//
//#endif //BAYESPERF_CPP_PROBABILITYNODE_H
