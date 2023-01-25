#ifndef PERFPROP_SAMPLECOLLECTOR_H
#define PERFPROP_SAMPLECOLLECTOR_H

#include <map>
#include <set>
#include <utility>
#include <functional>
#include "PmuEvent.h"
#include "Utils.h"
#include "Perf.h"
#include "PerfStats.h"


/*
 * Function that updates the stats of an event. It takes in the stats from a related event, and the stats to update, and
 * should directly modify eventToUpdate. The direct modification is not the greatest idea, but it simplifies the
 * implementation of custom updaters as seen in Updaters.h
 */
using StatUpdaterFunc = std::function<void(
        const PmuEvent::Stats& relatedEventStats,
        PmuEvent::Stats &eventToUpdate)>;

/*
 * Collects samples for events, calculates their mean and variance, and propagates updates across statistically related
 * events.
 */
class SampleCollector {
public:

    explicit SampleCollector(const std::vector<PmuEvent>& events);
    SampleCollector() = default;

    /*
     * All events need to be registered before we can process samples for them.
     */
    void registerEvent(const PmuEvent &event);

    /*
     * Adds a statistical relationship between eventFrom -> eventTo, given by func. This is a directed one-way
     * relationship, which means that whenever a new sample is pushed to `eventFrom`, the stats for event `eventTo`
     * will be updated by running func. However, nothing will occur if a sample is pushed to `eventTo`.
     */
    void addRelationship(const PmuEvent &eventFrom, const PmuEvent &eventTo, const StatUpdaterFunc& func);

    void pushSample(const PmuEvent& event, Perf::Sample sample);

    /*
     * Returns optional because an event may not have received any meaningful statistics. If an event hasn't had any
     * samples or statistical propagations, then this function will return nullopt.
     */
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
