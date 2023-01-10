//#include <algorithm>
//#include <functional>
//#include <utility>
//#include "include/ProbabilityNode.h"
//
//bool ProbabilityNode::eventInInput(const PmuEvent& event) const {
//    return std::find(eventsInput.begin(), eventsInput.end(), event) != eventsInput.end();
//}
//
//std::vector<PmuEvent> ProbabilityNode::getEvents() const {
//    return eventsInput;
//}
//
//ProbabilityNode::ProbabilityNode(std::initializer_list<PmuEvent> events, ProbabilityFunction function) : function(function) {
//    this->eventsInput = std::vector<PmuEvent>{events};
//}
//
//bool ProbabilityNode::operator==(const ProbabilityNode &node) const {
//    return this->eventsInput == node.eventsInput && function == node.function;
//}
//
//double ProbabilityNode::call() {
//    return function(eventsInput);
//}
//ProbabilityFunction ProbabilityNode::getUpdaterFunction() const {
//    return function;
//}
//
//
