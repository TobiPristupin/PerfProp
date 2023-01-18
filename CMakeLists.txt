cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 17)


include(CheckIncludeFile)
include(CheckLibraryExists)

if(NOT PERF)
    message(FATAL_ERROR "Linux perf tools not installed")
endif()


set( SOURCE_FILES
        main.cpp
        PmuEvent.cpp
        PmuEventParser.cpp
        Utils.cpp
        EventGraph.cpp Updaters.cpp PmuGrouper.cpp Perf.cpp)

include_directories(
        include/
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

find_library(PFM_LIB pfm REQUIRED)
target_link_libraries(${PROJECT_NAME} "${PFM_LIB}")

add_subdirectory(test)

