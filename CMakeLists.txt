cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 14)

add_executable(bayesperf_cpp main.cpp include/Event.h include/ProbabilityFunction.h ProbabilityFunction.cpp include/ProbabilityFunction.h include/Node.h Node.cpp include/Node.h)
