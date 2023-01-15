#include <memory>
#include <iostream>
#include "include/PmuEventParser.h"
#include "include/Utils.h"

static std::unordered_set<std::string> obtainSoftwareEvents();
static std::string obtainStdoutFromCmd(const char* cmd);

std::vector<PmuEvent> parseEvents(const std::string& cmdEventString) {
    std::unordered_set<std::string> softwareEvents = obtainSoftwareEvents();

    std::vector<PmuEvent> events;
    std::vector<std::string> eventStrings = Utils::splitString(cmdEventString, ',');
    for (const std::string &e : eventStrings){
        std::string eventName;
        PmuEvent::Type type;
        std::string modifiers;

        std::string::size_type colonPos = e.find(':');
        if (colonPos != std::string::npos){
            eventName = e.substr(0, colonPos);
            modifiers = e.substr(colonPos + 1);
        } else {
            eventName = e;
            modifiers = "";
        }

        type = softwareEvents.find(eventName) != softwareEvents.end() ? PmuEvent::SOFTWARE : PmuEvent::HARDWARE;
        events.emplace_back(eventName, type, modifiers);
    }

    return events;
}

/**
 * We obtain software events by calling `perf list sw --no-desc` and parsing the stdout. This is not ideal,
 * since it depends on how perf formats their output. Other alternatives are discussed here:
 * https://stackoverflow.com/questions/63653406/using-the-perf-events-from-perf-list-programatically
 * Another idea is to have the user pass in a special flag denoting that an event is software/hardware. That way
 * we pass on the responsibility to the user. This would cause a deviation from `perf` and would add extra work
 * to the user.
 *
 * If this begins to cause issues, we can investigate a more robust approach.
 *
 * Example abbreviated output of `perf list sw --no-desc`:
 *
 * List of pre-defined events (to be used in -e):
 *
 * alignment-faults                                   [Software event]
 * context-switches OR cs                             [Software event]
 * cpu-clock                                          [Software event]
 * ...
 *
 * duration_time                                      [Tool event]
 * user_time                                          [Tool event]
 * system_time                                        [Tool event]
 */
static std::unordered_set<std::string> obtainSoftwareEvents(){
    std::unordered_set<std::string> softwareEvents;
    std::string perfListEvents = obtainStdoutFromCmd("perf list sw --no-desc");
    std::istringstream iss(perfListEvents);
    for (std::string line; std::getline(iss, line); ){
        if (line.find("[Software event]") != std::string::npos && line.find("[Tool event]") != std::string::npos){
            continue;
        }

        std::vector<std::string> splitLine = Utils::splitString(line, ' ');
        if (splitLine[1] == "OR"){
            softwareEvents.insert(splitLine[0]);
            softwareEvents.insert(splitLine[2]);
        } else {
            softwareEvents.insert(splitLine[0]);
        }
    }

    if (softwareEvents.empty()){
        //There should always be some software events. If none are found, lets fail fast before this propagates
        //further. If this happens, there is probably something wrong with the parsing and this method should be
        //reworked.
        throw std::runtime_error("Error: No software events");
    }

    return softwareEvents;
}

static std::string obtainStdoutFromCmd(const char* cmd){
    //https://stackoverflow.com/a/478960/8402038
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

