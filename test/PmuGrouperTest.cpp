#include <gtest/gtest.h>
#include "PmuGrouper.h"

class PmuGrouperTest : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}

    std::vector<PmuEvent> events = {
            {"branches", PmuEvent::Type::HARDWARE},
            {"branch-misses:u", PmuEvent::Type::HARDWARE},
            {"instructions:uv", PmuEvent::Type::HARDWARE},
            {"bpf-output", PmuEvent::Type::SOFTWARE},
            {"duration_time", PmuEvent::Type::SOFTWARE},
            {"L1-dcache-loads", PmuEvent::Type::HARDWARE},
            {"conntext-switches", PmuEvent::Type::SOFTWARE},
            {"cpu-clock", PmuEvent::Type::SOFTWARE},
    };

    static int numSoftwareEvents(const std::vector<PmuEvent>& evs){
        int count = 0;
        for (const auto& e : evs) {
            count += e.getType() == PmuEvent::SOFTWARE;
        }
        return count;
    }

    static int numHardwareEvents(const std::vector<PmuEvent>& evs){
        int count = 0;
        for (const auto& e : evs) {
            count += e.getType() == PmuEvent::HARDWARE;
        }
        return count;
    }
};

TEST_F(PmuGrouperTest, testSoftwareEventGrouping){
    size_t maxGroupSize = 3;
    std::vector<std::vector<PmuEvent>> groups1 = PmuGrouper::group(events, maxGroupSize);

    //Only the final group must have software events
    for (int i = 0; i < groups1.size(); i++){
        for (const PmuEvent& e : groups1[i]){
            if (i == groups1.size() - 1){
                ASSERT_EQ(e.getType(), PmuEvent::SOFTWARE);
            } else {
                ASSERT_EQ(e.getType(), PmuEvent::HARDWARE);
            }
        }
    }

    //Make sure all software events are there
    ASSERT_EQ(groups1.back().size(), numSoftwareEvents(events));
}

TEST_F(PmuGrouperTest, testGroupSize){
    int hardwareEvents = numHardwareEvents(events);
    for (size_t groupSize = 1; groupSize <= hardwareEvents; groupSize++){
        std::vector<std::vector<PmuEvent>> group = PmuGrouper::group(events, groupSize);
        //all but the last group (the software group) must abide by the maxGroupSize requirement
        for (int i = 0; i < group.size() - 1; i++){
            ASSERT_LE(group[i].size(), groupSize);
        }
    }
}