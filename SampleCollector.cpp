#include "SampleCollector.h"
#include "Logger.h"

SampleCollector::SampleCollector(const std::vector<PmuEvent> &events) {
    for (const PmuEvent& e : events){
        registerEvent(e);
    }
}

void SampleCollector::registerEvent(const PmuEvent &event) {
    if (graph.find(event) == graph.end()){
        graph.insert({event, std::set<Edge>()});
        collectorStats.insert({event, PmuEvent::Stats()});
    } else {
        Logger::debug("Warning: Attempting to add an already existing event. This has no effect\n");
    }
}

void SampleCollector::addRelationship(const PmuEvent &from, const PmuEvent &eventTo, const StatUpdaterFunc &func) {
    if (graph.find(from) == graph.end()){
        registerEvent(from);
    }

    if (graph.find(eventTo) == graph.end()){
        registerEvent(eventTo);
    }

    for (const auto& edge : graph.at(from)){
        if (edge.eventTo == eventTo){
            throw std::runtime_error("A relationship already exists for these two events");
        }
    }

    graph.at(from).insert(Edge(from, eventTo, func));
}

void SampleCollector::pushSample(const PmuEvent &event, Perf::Sample sample) {
    PmuEvent::Stats &eventStats = collectorStats.at(event);

    Nanosecs timeDiff = sample.timeEnabled - eventStats.timeEnabled;
    EventCount newCount = sample.value - eventStats.count;

    updateMean(eventStats, newCount, timeDiff);
    updateVariance(eventStats, newCount, timeDiff);
    eventStats.count = sample.value;
    eventStats.timeEnabled = sample.timeEnabled;
    eventStats.samples++;
    //Note: Update samples after mean and var, because mean and var updates expect an "old" count of samples

    propagateUpdateToNeighbors(event, eventStats);
}

void SampleCollector::updateMean(PmuEvent::Stats &stats, EventCount newCount, Nanosecs timeDiff) {
    if (timeDiff.count() <= 0){
        throw std::runtime_error("Time enabled must monotonically increase");
    }

    EventCount oldSampleCount = stats.samples;
    Statistic newCountPerMillis = newCountPerMillis = (Statistic) newCount / nsToMs(timeDiff).count();



    Statistic currSum = stats.meanCountsPerMillis * (oldSampleCount);
    Statistic newMean = (currSum + newCountPerMillis) / (oldSampleCount + 1);
    stats.meanCountsPerMillis = newMean;
}

void SampleCollector::updateVariance(PmuEvent::Stats &stats, EventCount newCount, Nanosecs timeDiff) {
    stats.varianceCountPerMillis = 0;
    //TODO: update running variance
}

std::optional<PmuEvent::Stats> SampleCollector::getEventStatistics(const PmuEvent &event) const {
    if (collectorStats.find(event) == collectorStats.end()){
        throw std::runtime_error("Attempting to obtain stats for unregistered event");
    }

    PmuEvent::Stats stats = collectorStats.at(event);
    if (stats.samples == 0 && stats.propagations == 0){
        return std::nullopt;
    }
    return stats;
}

void SampleCollector::propagateUpdateToNeighbors(const PmuEvent &event, const PmuEvent::Stats &eventUpdatedStats) {
    for (const Edge& edge : graph.at(event)){
        std::cout << "[Stat Propagation] from=" << event.getName() << " to=" << edge.eventTo.getName() << "\n";
        PmuEvent::Stats &statsToUpdate = collectorStats.at(edge.eventTo);
        std::cout << "[Stat Propagation] Before=" << statsToUpdate << "\n";
        edge.updaterFunc(eventUpdatedStats, statsToUpdate);
        statsToUpdate.propagations++;
        std::cout << "[Stat Propagation] After=" << statsToUpdate << "\n";
    }
}





