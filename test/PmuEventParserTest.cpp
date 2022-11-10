#include <gtest/gtest.h>
#include "../include/PmuEvent.h"
#include "../include/PmuEventParser.h"



TEST(PmuEventParserTest, parseSoftwareEvents){
    std::string eventsCmdInput = "branches,branch-misses:u,instructions:uv,bpf-output,duration_time,L1-dcache-loads";
    std::vector<PmuEvent> parsedEvents = parseEvents(eventsCmdInput);
    std::vector<PmuEvent> expectedEvents = {
            {"branches", PmuEvent::Type::HARDWARE, ""},
            {"branch-misses", PmuEvent::Type::HARDWARE, "u"},
            {"instructions", PmuEvent::Type::HARDWARE, "uv"},
            {"bpf-output", PmuEvent::Type::SOFTWARE, ""},
            {"duration_time", PmuEvent::Type::SOFTWARE, ""},
            {"L1-dcache-loads", PmuEvent::Type::HARDWARE, ""},
    };

    ASSERT_EQ(parsedEvents, expectedEvents);
}