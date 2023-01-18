cmake_minimum_required(VERSION 3.21)

set( TEST_SOURCE_FILES
        testMain.cpp
        PmuEventParserTest.cpp
        PmuGrouperTest.cpp PerfTest.cpp)

set ( MAIN_SOURCE_FILES
        ../PmuEvent.cpp
        ../PmuEventParser.cpp
        ../Utils.cpp
        ../EventGraph.cpp
        ../Updaters.cpp
        ../PmuGrouper.cpp
        ../Perf.cpp ../TraceableProcess.cpp)

add_executable( bayesperf_cpp_tests ${MAIN_SOURCE_FILES} ${TEST_SOURCE_FILES})

target_include_directories(bayesperf_cpp_tests PUBLIC
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
        bayesperf_cpp_tests
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(bayesperf_cpp_tests)

add_executable( standalone_cpp standalone.cpp)
find_library(PFM_LIB pfm REQUIRED)
target_link_libraries(bayesperf_cpp_tests "${PFM_LIB}")
target_link_libraries(standalone_cpp "${PFM_LIB}")