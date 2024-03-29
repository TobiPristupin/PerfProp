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

    namespace {
        perf_event_attr getPerfEventAttr(const std::string& name) {
            pfm_perf_encode_arg_t arg;
            memset(&arg, 0, sizeof(arg));
            perf_event_attr attr{0};
            arg.attr = &attr;
            arg.size = sizeof(pfm_perf_encode_arg_t);

            int ret = pfm_get_os_event_encoding(name.c_str(), PFM_PLM3, PFM_OS_PERF_EVENT, &arg);
            if (ret != PFM_SUCCESS) {
                std::string errorMessage;
                if (ret == PFM_ERR_ATTR || ret == PFM_ERR_ATTR_VAL){
                    //The builtin pfm_strerror message for these error codes is not great, so we make our own.
                   errorMessage = "Cannot get encoding for " + name + ", " + "invalid modifier";
                } else {
                    errorMessage = "Cannot get encoding for " + name + ", " + pfm_strerror(ret);
                }

                throw std::runtime_error(errorMessage);
            }

            return attr;
        }

        pfm_pmu_t nextPmu(const pfm_pmu_t& pmu){
            /*
             * pfm_pmu_t is a C enum defined by libpfm. Cpp doesn't allow incrementing an enum, which is what the
             * examples of libpfm do to obtain the next pmu. This is a little casting hack to achieve that.
             *
             * Aside: cpp doesn't allow incrementing enums, because enum values may not be contiguous. In C, enum
             * values are contiguous, which means this code is safe.
             */
            return (pfm_pmu_t)(1 + (int)(pmu));
        }
    }

    PmuEvent::Type getEventType(const std::string &name) {
        perf_event_attr encoding = getPerfEventAttr(name);
        return static_cast<perf_type_id>(encoding.type);
    }

    /*
     * FIXME: There might be more than one pmu. This function returns the first one. This can cause problems because
     * we may return a pmu that has say 6 available counters, but the actual pmu that is used by the OS has less than 6
     * available counters. We should have a way for the user to select which one to use.
     */
    pfm_pmu_info_t getDefaultPmu() {
        pfm_pmu_info_t pmu_info;
        for (pfm_pmu_t pmu = PFM_PMU_NONE; pmu < PFM_PMU_MAX; pmu = nextPmu(pmu)){
            int ret = pfm_get_pmu_info(pmu, &pmu_info);
            if (ret != PFM_SUCCESS){
                continue;
            }

            if (pmu_info.is_present && pmu_info.type != PFM_PMU_TYPE_OS_GENERIC){
                return pmu_info;
            }
        }

        throw std::runtime_error("libpfm could not find default pmu");
    }

    size_t numProgrammableHPCs() {
        static pfm_pmu_info_t defaultPmu{getDefaultPmu()};
        return defaultPmu.num_cntrs;
    }

    perf_event_attr getPerfEventAttr(const PmuEvent& event) {
        std::string eventName = event.getModifiers().empty() ? event.getName() : event.getName() + ":" + event.getModifiers();
        return getPerfEventAttr(eventName);
    }

    std::pair<std::map<int, PmuEvent>, std::vector<int>> perfOpenEvents(const std::vector<std::vector<PmuEvent>>& eventGroups, pid_t pid) {
        std::map<int, PmuEvent> fds;
        std::vector<int> groupLeaderFds;

        for (auto& eventGroup : eventGroups){
            int groupLeaderFd = -1;
            for (const PmuEvent& event : eventGroup){
                perf_event_attr attr{};
                try {
                    attr = getPerfEventAttr(event);
                } catch (const std::runtime_error &e){
                    Logger::error(e.what());
                    Logger::error("Skipped scheduling of " + event.getName() + ", could not get event encoding.");
                    continue;
                }

                attr.disabled = 1;
                attr.enable_on_exec = 1;
                attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
                int fd = perf_event_open(&attr, pid, -1, groupLeaderFd, 0);
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

    Sample readSample(int fd) {
        uint64_t buf[3];
        if (read(fd, &buf, sizeof(buf)) < 0){
            throw std::runtime_error(strerror(errno));
        }

        return {buf[0], Nanosecs(buf[1]), Nanosecs(buf[2])};
    }

    void closeFds(std::map<int, PmuEvent> &fds) {
        for (const auto& pair : fds){
            close(pair.first);
        }
    }
    void resetCounter(int fd) {
        ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    }

}