#include <csignal>
#include <asm/unistd.h>
#include <cstring>
#include "Perf.h"
#include "Logger.h"
#include <perfmon/pfmlib_perf_event.h>



/**
 * Warning: We are interfacing with pfmlib, a low level C library, so you may see some traditional C practices instead
 * of modern C++.
 */
namespace Perf {

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

    std::pair<std::unordered_map<int, PmuEvent>, std::vector<int>> perfOpenEvents(const std::vector<std::vector<PmuEvent>>& eventGroups, pid_t pid) {
        std::unordered_map<int, PmuEvent> fds;
        std::vector<int> groupLeaderFds;

        for (auto& eventGroup : eventGroups){
            int groupLeaderFd = -1;
            for (const PmuEvent& event : eventGroup){
                std::optional<perf_event_attr> attr = getPerfEventAttr(event);
                if (!attr){
                    Logger::error("Skipped scheduling of " + event.getName() + ", could not get event encoding.");
                    continue;
                }

                attr->disabled = 0; //TODO? WHy 0 works and not 1
                attr->read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
                int fd = perf_event_open(&attr.value(), pid, -1, groupLeaderFd, 0);
                if (fd < 0){
                    Logger::error("perf_event_open failed for event " + event.getName() + ", " + strerror(errno));
                    continue;
                }

                fds.insert({fd, event});

                if (groupLeaderFd == -1){
                    groupLeaderFds.push_back(fd);
                    groupLeaderFd = fd;
                }
            }
        }

        return std::make_pair(fds, groupLeaderFds);
    }

    void enableEvents(const std::vector<int>& fds) {
        for (int fd : fds){
            ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
        }
    }

    void disableEvents(const std::vector<int>& fds) {
        for (int fd : fds){
            ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
        }
    }
    std::vector<uint64_t> readSamplesForGroup(int groupLeaderFd) {
        std::vector<uint64_t> samples(4, 0);
        //need to know group size here
        //data read as bytes, needs to be turned into a class
        //we will obtain perf ids for everything that we read, need to return that so the caller can associate pmu events
        //with their samples

        uint64_t data[32]{0};

        read(groupLeaderFd, &data, sizeof(data));

        //for some reason only the first two events produce non zero values?

        return samples;
    }

    void closeFds(std::unordered_map<int, PmuEvent> &fds) {
        for (const auto& pair : fds){
            close(pair.first);
        }
    }

}