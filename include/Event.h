#ifndef BAYESPERF_CPP_EVENT_H
#define BAYESPERF_CPP_EVENT_H

#include <string>
#include "Node.h"

class Event : public Node {
    int id;
    std::string name;
};

#endif //BAYESPERF_CPP_EVENT_H
