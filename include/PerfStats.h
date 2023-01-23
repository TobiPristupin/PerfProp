#ifndef PERFPROP_PERFSTATS_H
#define PERFPROP_PERFSTATS_H

#include <cstdint>

//Lone header to avoid a circular dependency between PmuEvent.h and Perf.h

using EventCount = uint64_t;

//We use unsigned int for time because that is what perf_event_open returns
using Nanosecs = std::chrono::duration<uint64_t, std::nano>;
using Millis = std::chrono::duration<uint64_t, std::milli>;

inline Millis nsToMs(const Nanosecs& nanosecs){
    return std::chrono::duration_cast<Millis>(nanosecs);
}

inline Nanosecs msToNs(const Millis& millis){
    return std::chrono::duration_cast<Nanosecs>(millis);
}


#endif
