#include <gtest/gtest.h>
#include <perfmon/pfmlib.h>
#include "../include/PmuEvent.h"
#include "../include/EventParser.h"

class EventParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        pfm_initialize();
    }

    void TearDown() override {
        pfm_terminate();
    }
};

TEST_F(EventParserTest, parseEvents){
    std::string eventsCmdInput = "branches,branch-misses:u,instructions,L2_PREFETCH_HIT_L2";
    std::vector<PmuEvent> parsedEvents = PmuParser::parseEvents(eventsCmdInput);
    std::vector<PmuEvent> expectedEvents = {
            {"branches", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"branch-misses:u", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"instructions", PmuEvent::Type::PERF_TYPE_HARDWARE},
            {"L2_PREFETCH_HIT_L2", PmuEvent::Type::PERF_TYPE_RAW},
    };

    ASSERT_EQ(parsedEvents, expectedEvents);
    ASSERT_EQ(parsedEvents[0].getModifiers(), "");
    ASSERT_EQ(parsedEvents[1].getModifiers(), "u");
    ASSERT_EQ(parsedEvents[2].getModifiers(), "");
    ASSERT_EQ(parsedEvents[3].getModifiers(), "");
}