cmake_minimum_required(VERSION 3.21)

set( TEST_SOURCE_FILES
        testMain.cpp
        EventParserTest.cpp
        EventGrouperTest.cpp
        PerfTest.cpp
        SampleCollectorTest.cpp)

set ( MAIN_SOURCE_FILES
        ../PmuEvent.cpp
        ../EventParser.cpp
        ../Utils.cpp
        ../SampleCollector.cpp
        ../Updaters.cpp
        ../EventGrouper.cpp
        ../Perf.cpp
        ../TraceableProcess.cpp
        ../StringUtils.cpp
        ../CommandParser.cpp
        ../showevtinfo.c)

add_executable( perfprop_tests ${MAIN_SOURCE_FILES} ${TEST_SOURCE_FILES})

target_include_directories(perfprop_tests PUBLIC
        ../include/
        ../lib/expected/
)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)

enable_testing()

target_link_libraries(
        perfprop_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(perfprop_tests)

add_executable( standalone_cpp standalone.cpp)
find_library(PFM_LIB pfm REQUIRED)
target_link_libraries(perfprop_tests "${PFM_LIB}")
target_link_libraries(standalone_cpp "${PFM_LIB}")