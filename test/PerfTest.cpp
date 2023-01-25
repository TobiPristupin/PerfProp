#include <gtest/gtest.h>
#include <perfmon/pfmlib.h>
#include "Perf.h"

class PerfTest : public ::testing::Test {
protected:
    void SetUp() override {
      pfm_initialize();
    }

     void TearDown() override {
        pfm_terminate();
    }
};

/*
 * Test that we can find encodings for valid events, and we correctly fail with invalid events. The actual logic to
 * obtain the event encoding is performed by libpfm and is therefore not tested by us.
 *
 * getPerfEventAttr will generate error logs when an event's encoding cannot be found. Those should be ignored.
 */
TEST_F(PerfTest, getEventEncodingTest){
    ASSERT_NO_THROW({
        PmuEvent valid = PmuEvent("RETIRED_INSTRUCTIONS", PmuEvent::Type::PERF_TYPE_RAW);
        ASSERT_EQ(Perf::getPerfEventAttr(valid).type, PmuEvent::Type::PERF_TYPE_RAW);

        valid = PmuEvent("instructions", PmuEvent::Type::PERF_TYPE_HARDWARE);
        ASSERT_EQ(Perf::getPerfEventAttr(valid).type, PmuEvent::Type::PERF_TYPE_HARDWARE);

        PmuEvent validPerfStandardEvent = PmuEvent("context-switches", PmuEvent::Type::PERF_TYPE_SOFTWARE);
        ASSERT_EQ(Perf::getPerfEventAttr(validPerfStandardEvent).type, PmuEvent::Type::PERF_TYPE_SOFTWARE);
    });


    PmuEvent invalid = PmuEvent("caca", PmuEvent::Type::PERF_TYPE_HARDWARE);
    ASSERT_ANY_THROW(Perf::getPerfEventAttr(invalid));
}

/*
 * Will generate error logs when an event's encoding cannot be found. Those should be ignored.
 */
TEST_F(PerfTest, perfOpenEventsTest){
    std::vector<PmuEvent> validEvents = {
            {"branches", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"branch-misses:u", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"RETIRED_INSTRUCTIONS", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"bpf-output", PmuEvent::Type::PERF_TYPE_SOFTWARE},
    };
    PmuEvent invalid = {"caca", PmuEvent::Type::PERF_TYPE_HARDWARE};

    std::vector<std::vector<PmuEvent>> groups = {
            {validEvents[0], validEvents[1], validEvents[2]},
            {validEvents[0], validEvents[1], invalid}
            };

    auto [fds, groupLeaderFds] = Perf::perfOpenEvents(groups, 0);
    ASSERT_EQ(fds.size(), 5); //should have a fd for every event but the invalid one
//    ASSERT_EQ(groupLeaderFds.size(), 2);
}