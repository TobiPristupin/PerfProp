cmake_minimum_required(VERSION 3.21)
project(bayesperf_cpp)

set(CMAKE_CXX_STANDARD 17)

include(CheckIncludeFile)
include(CheckLibraryExists)

if(NOT PERF)
    message(FATAL_ERROR "Linux perf tools not installed")
endif()

#check_library_exists(pfm pfm_initialize "" HAVE_LIBPFM_INITIALIZE)
#check_include_file(perfmon/perf_event.h HAVE_PERFMON_PERF_EVENT_H)
#check_include_file(perfmon/pfmlib.h HAVE_PERFMON_PFMLIB_H)
#check_include_file(perfmon/pfmlib_perf_event.h HAVE_PERFMON_PFMLIB_PERF_EVENT_H)
#if(NOT (HAVE_LIBPFM_INITIALIZE AND HAVE_PERFMON_PERF_EVENT_H AND HAVE_PERFMON_PFMLIB_H AND
#        HAVE_PERFMON_PFMLIB_PERF_EVENT_H))
##    message(FATAL_ERROR "libpfm4 not installed")
#endif()

set( SOURCE_FILES
        main.cpp
        PmuEvent.cpp
        PmuEventParser.cpp
        Utils.cpp
        EventGraph.cpp Updaters.cpp PmuGrouper.cpp PmuArch.cpp)

include_directories(
        include/
)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

find_library(PFM_LIB pfm REQUIRED)
target_link_libraries(${PROJECT_NAME} "${PFM_LIB}")

add_subdirectory(test)

