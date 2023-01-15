#include <csignal>
#include <asm/unistd.h>
#include "PmuArch.h"

namespace PmuArch {

    namespace {
        long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
            return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
        }
    }

    size_t numProgrammableHPCs() {
        //TODO: Fill this in
        return 3;
    }



}