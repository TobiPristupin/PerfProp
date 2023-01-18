#include <gtest/gtest.h>
#include "../include/PmuEvent.h"
#include "../include/PmuParser.h"



TEST(PmuParserTest, parseEvents){
    std::string eventsCmdInput = "branches,branch-misses:u,instructions:uv,bpf-output,duration_time,L1-dcache-loads";
    std::vector<PmuEvent> parsedEvents = PmuParser::parseEvents(eventsCmdInput);
    std::vector<PmuEvent> expectedEvents = {
            {"branches", PmuEvent::Type::HARDWARE},
            {"branch-misses:u", PmuEvent::Type::HARDWARE},
            {"instructions:uv", PmuEvent::Type::HARDWARE},
            {"bpf-output", PmuEvent::Type::SOFTWARE},
            {"duration_time", PmuEvent::Type::SOFTWARE},
            {"L1-dcache-loads", PmuEvent::Type::HARDWARE},
    };

    ASSERT_EQ(parsedEvents, expectedEvents);
    ASSERT_EQ(parsedEvents[0].getModifiers(), "");
    ASSERT_EQ(parsedEvents[1].getModifiers(), "u");
    ASSERT_EQ(parsedEvents[2].getModifiers(), "uv");
}