#include <gtest/gtest.h>
#include "../include/PmuEvent.h"
#include "../include/PmuEventParser.h"



TEST(PmuEventParserTest, parseSoftwareEvents){
    std::string eventsCmdInput = "branches,branch-misses:u,instructions:uv,bpf-output,duration_time,L1-dcache-loads";
    std::vector<PmuEvent> events = parseEvents(eventsCmdInput);
    for (const PmuEvent &e : events){
        std::cout << e.name << "\n";
    }
}