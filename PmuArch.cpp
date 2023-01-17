#include <csignal>
#include <asm/unistd.h>
#include <cstring>
#include "PmuArch.h"
#include "Logger.h"
#include <perfmon/pfmlib_perf_event.h>


/**
 * Warning: We are interfacing with pfmlib, a low level C library, so you may see some traditional C practices instead
 * of modern C++.
 */
namespace PmuArch {

    size_t numProgrammableHPCs() {
        //TODO: Fill this in by using pfm_get_pmu_info and num_cntrs field, see man pages for details
        return 3;
    }

    std::optional<perf_event_attr> getPerfEventAttr(const PmuEvent& event) {
        pfm_perf_encode_arg_t arg;
        memset(&arg, 0, sizeof(arg));
        perf_event_attr attr{0};
        arg.attr = &attr;
        arg.size = sizeof(pfm_perf_encode_arg_t);

        std::string eventName = event.getModifiers().empty() ? event.getName() : event.getName() + ":" + event.getModifiers();
        int ret = pfm_get_os_event_encoding(eventName.c_str(), PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
        if (ret != PFM_SUCCESS) {
            if (ret == PFM_ERR_ATTR || ret == PFM_ERR_ATTR_VAL){
                //The builtin pfm_strerror message for these error codes is not great, so we make our own.
                Logger::error("Cannot get encoding for " + eventName + ", " + "invalid modifier");
            } else {
                Logger::error("Cannot get encoding for " + eventName + ", " + pfm_strerror(ret));
                return std::nullopt;
            }
        }

        return attr;
    }
    std::unordered_map<int, PmuEvent> perfOpenEvents(const std::vector<std::vector<PmuEvent>>& eventGroups, pid_t pid) {
        std::unordered_map<int, PmuEvent> fds;

        for (auto& eventGroup : eventGroups){
            int groupLeaderFd = -1;
            for (const PmuEvent& event : eventGroup){
                std::optional<perf_event_attr> attr = getPerfEventAttr(event);
                if (!attr){
                    Logger::error("Skipped scheduling of " + event.getName() + ", could not get event encoding.");
                    continue;
                }

                attr->disabled = 1;
                attr->read_format = PERF_FORMAT_GROUP;
                int fd = perf_event_open(&attr.value(), pid, -1, groupLeaderFd, 0);
                fds.insert({fd, event});

                if (groupLeaderFd == -1){
                    groupLeaderFd = fd;
                }
            }
        }

        return fds;
    }


}