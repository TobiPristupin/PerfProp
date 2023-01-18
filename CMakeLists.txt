cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 17)

if(NOT PERF)
    message(FATAL_ERROR "Linux perf tools not installed")
endif()


set( SOURCE_FILES
        main.cpp
        PmuEvent.cpp
        PmuParser.cpp
        Utils.cpp
        EventGraph.cpp Updaters.cpp PmuGrouper.cpp Perf.cpp TraceableProcess.cpp)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

target_include_directories( ${PROJECT_NAME} PUBLIC
        include/
        lib/expected/
        )

find_library(PFM_LIB pfm REQUIRED)
target_link_libraries(${PROJECT_NAME} "${PFM_LIB}")

add_subdirectory(test)

