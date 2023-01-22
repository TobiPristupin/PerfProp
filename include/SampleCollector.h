#ifndef BAYESPERF_CPP_SAMPLECOLLECTOR_H
#define BAYESPERF_CPP_SAMPLECOLLECTOR_H

#include <map>
#include <set>
#include <utility>
#include <functional>
#include "PmuEvent.h"
#include "Utils.h"
#include "Perf.h"


//comments for this class are outdated TODO: comments

using StatUpdaterFunc = std::function<void(
        const PmuEvent::Stats& relatedEventStats,
        PmuEvent::Stats &eventToUpdate)>;

/*
 * Graph where events are nodes, and directed edges represent statistical relationships between two events. If there
 * is a (directed) edge from e1 to e2, that means that the measured values of e1 should affect the values of e2. Thus,
 * if we obtain a sample for e1, we can use that sample to update the statistics of e2.
 *
 * Every edge in the graph contains a StatUpdaterFunc, a function that takes in the two events that touch that edge
 * as parameters, and updates the values of the destination event based on the value of the origin event.
 */
class SampleCollector {
public:

    explicit SampleCollector(const std::vector<PmuEvent>& events);
    SampleCollector() = default;

    void registerEvent(const PmuEvent &event);

    /*
     * Adds an edge with `func` from `eventFrom` to `eventTo`. If the events have not been added to the graph yet,
     * this method will also add them.
     */
    void addRelationship(const PmuEvent &from, const PmuEvent &eventTo, const StatUpdaterFunc& func);

    void pushSample(const PmuEvent& event, Perf::Sample sample);

    std::optional<PmuEvent::Stats> getEventStatistics(const PmuEvent& event) const;

private:

    class Edge {
    public:
        PmuEvent eventFrom, eventTo;
        StatUpdaterFunc updaterFunc;
        Edge(PmuEvent eventFrom, PmuEvent eventTo, StatUpdaterFunc updaterFunc)
        : eventFrom(std::move(eventFrom)), eventTo{std::move(eventTo)}, updaterFunc{std::move(updaterFunc)} {};

        bool operator==(const Edge &e) const {
            return this->eventTo == e.eventTo && this->eventFrom == e.eventFrom;
        }

        bool operator<(const Edge &e) const {
            if (this->eventFrom == e.eventFrom){
                return this->eventTo < e.eventTo;
            }
            return this->eventFrom < e.eventFrom;
        }
    };

    /*
     * Modifies the stats struct by updating its mean with the new sample.
     *
     * WARNING: Do not update stats.sampleCount before calling this method. Do it after, as this method
     * expected sampleCount to not include the new sample in the count.
     */
    static void updateMean(PmuEvent::Stats &stats, EventCount newCount, Nanosecs timeDiff);

    /*
     * Modifies the stats struct by updating its variance with the new sample.
     *
     * WARNING: Do not update stats.sampleCount before calling this method. Do it after, as this method
     * expected sampleCount to not include the new sample in the count.
     */
    static void updateVariance(PmuEvent::Stats &stats, EventCount newCount, Nanosecs timeDiff);


    /*
     * Propagate update to neighbors of event. eventUpdatedStats are the already updated stats for event, which will
     * then be used to propagate changes to neighbors.
     */
    void propagateUpdateToNeighbors(const PmuEvent& event, const PmuEvent::Stats& eventUpdatedStats);


    std::map<PmuEvent, std::set<Edge>> graph;
    std::map<PmuEvent, PmuEvent::Stats> collectorStats;

};





#endif
