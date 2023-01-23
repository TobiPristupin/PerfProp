cmake_minimum_required(VERSION 3.21)

project(perfprop)

set(CMAKE_CXX_STANDARD 17)

#if(NOT PERF)
#    message(FATAL_ERROR "Linux perf tools not installed")
#endif()


set( SOURCE_FILES
        main.cpp
        PmuEvent.cpp
        PmuParser.cpp
        Utils.cpp
        SampleCollector.cpp
        Updaters.cpp
        PmuGrouper.cpp
        Perf.cpp
        TraceableProcess.cpp
        StringUtils.cpp
        CommandParser.cpp
        showevtinfo.c)

add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )

target_include_directories( ${PROJECT_NAME} PUBLIC
        include/
        lib/expected/
        lib/VariadicTable/
        )

find_library(PFM_LIB pfm REQUIRED)
target_link_libraries(${PROJECT_NAME} "${PFM_LIB}")

add_subdirectory(test)

