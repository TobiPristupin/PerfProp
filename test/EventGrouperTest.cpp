#include <gtest/gtest.h>
#include "EventGrouper.h"

class EventGrouperTest : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}

    std::vector<PmuEvent> events = {
            {"branches", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"branch-misses:u", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"instructions:uv", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"bpf-output", PmuEvent::Type::PERF_TYPE_SOFTWARE},
            {"duration_time", PmuEvent::Type::PERF_TYPE_SOFTWARE},
            {"L1-dcache-loads", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"context-switches", PmuEvent::Type::PERF_TYPE_SOFTWARE},
            {"cpu-clock", PmuEvent::Type::PERF_TYPE_SOFTWARE},
    };

    static int numSoftwareEvents(const std::vector<PmuEvent>& evs){
        int count = 0;
        for (const auto& e : evs) {
            count += e.getType() == PmuEvent::Type::PERF_TYPE_SOFTWARE;
        }
        return count;
    }

    static int numHardwareEvents(const std::vector<PmuEvent>& evs){
        int count = 0;
        for (const auto& e : evs) {
            count += e.getType() != PmuEvent::Type::PERF_TYPE_SOFTWARE;
        }
        return count;
    }
};

TEST_F(EventGrouperTest, testSoftwareEventGrouping){
    size_t maxGroupSize = 3;
    std::vector<std::vector<PmuEvent>> groups1 = PmuGrouper::group(events, maxGroupSize);

    //Only the final group must have software events
    for (int i = 0; i < groups1.size(); i++){
        for (const PmuEvent& e : groups1[i]){
            if (i == groups1.size() - 1){
                ASSERT_EQ(e.getType(), PmuEvent::Type::PERF_TYPE_SOFTWARE);
            } else {
                ASSERT_NE(e.getType(), PmuEvent::Type::PERF_TYPE_SOFTWARE);
            }
        }
    }

    //Make sure all software events are there
    ASSERT_EQ(groups1.back().size(), numSoftwareEvents(events));
}

TEST_F(EventGrouperTest, testGroupSize){
    int hardwareEvents = numHardwareEvents(events);
    for (size_t groupSize = 1; groupSize <= hardwareEvents; groupSize++){
        std::vector<std::vector<PmuEvent>> group = PmuGrouper::group(events, groupSize);
        //all but the last group (the software group) must abide by the maxGroupSize requirement
        for (int i = 0; i < group.size() - 1; i++){
            ASSERT_LE(group[i].size(), groupSize);
        }
    }
}