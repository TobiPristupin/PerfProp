#ifndef BAYESPERF_CPP_PMUEVENTPARSER_H
#define BAYESPERF_CPP_PMUEVENTPARSER_H

// Collection of functions to parse events from the command line

#include <vector>
#include <sstream>
#include <unordered_set>
#include "PmuEvent.h"

/**
 * Parses a list of events from the command line. The input format for the events is the same as `perf stat`.
 * Events must be in a comma separated list with no spaces. Event names can be appended with a : and a modifier, which
 * can be one or more of {u, k, h, H, G}. Refer to perf's documentation for explanations on their meanings.
 *
 * Example input: cycles,instructions:k,cache-misses:ku
 */
std::vector<PmuEvent> parseEvents(const std::string& cmdEventString);


#endif //BAYESPERF_CPP_PMUEVENTPARSER_H
