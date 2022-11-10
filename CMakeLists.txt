cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 17)


if(NOT PERF)
    message(FATAL_ERROR "Linux perf tools not installed")
endif()

set( SOURCE_FILES
        main.cpp
        ProbabilityNode.cpp
        EventNode.cpp
        Node.cpp
        FactorGraph.cpp
        PmuEvent.cpp
        PmuEventParser.cpp
        Utils.cpp
        )

include_directories(
        include/
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

add_subdirectory(test)

