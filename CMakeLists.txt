cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(bayesperf_cpp main.cpp include/Event.h include/ProbabilityNode.h ProbabilityNode.cpp include/ProbabilityNode.h include/Node.h include/FactorGraph.h EventNode.cpp Node.cpp FactorGraph.cpp Event.cpp)
