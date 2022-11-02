cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 17)


if(NOT PERF)
    message(FATAL_ERROR "Linux perf tools not installed")
endif()

add_executable(bayesperf_cpp main.cpp include/PmuEvent.h include/ProbabilityNode.h ProbabilityNode.cpp include/ProbabilityNode.h include/Node.h include/FactorGraph.h EventNode.cpp Node.cpp FactorGraph.cpp PmuEvent.cpp include/PmuEventParser.h PmuEventParser.cpp include/Utils.h Utils.cpp)
