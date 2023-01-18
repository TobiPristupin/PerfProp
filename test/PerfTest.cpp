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
    PmuEvent valid = PmuEvent("RETIRED_INSTRUCTIONS", PmuEvent::HARDWARE);
    ASSERT_TRUE(Perf::getPerfEventAttr(valid).has_value());

    PmuEvent validPerfStandardEvent = PmuEvent("cycles", PmuEvent::SOFTWARE);
    ASSERT_TRUE(Perf::getPerfEventAttr(validPerfStandardEvent).has_value());

    PmuEvent invalid = PmuEvent("caca", PmuEvent::HARDWARE);
    ASSERT_FALSE(Perf::getPerfEventAttr(invalid).has_value());
}

/*
 * Will generate error logs when an event's encoding cannot be found. Those should be ignored.
 */
TEST_F(PerfTest, perfOpenEventsTest){
    std::vector<PmuEvent> validEvents = {
            {"branches", PmuEvent::Type::HARDWARE},
            {"branch-misses:u", PmuEvent::Type::HARDWARE},
            {"RETIRED_INSTRUCTIONS", PmuEvent::Type::HARDWARE},
            {"bpf-output", PmuEvent::Type::SOFTWARE},
    };
    PmuEvent invalid = {"caca", PmuEvent::HARDWARE};

    std::vector<std::vector<PmuEvent>> groups = {
            {validEvents[0], validEvents[1], validEvents[2]},
            {validEvents[0], validEvents[1], invalid}
            };

    auto [fds, groupLeaderFds] = Perf::perfOpenEvents(groups, 0);
    ASSERT_EQ(fds.size(), 5); //should have a fd for every event but the invalid one
//    ASSERT_EQ(groupLeaderFds.size(), 2);
}