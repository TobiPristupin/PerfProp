#ifndef BAYESPERF_CPP_UPDATERS_H
#define BAYESPERF_CPP_UPDATERS_H

#include <functional>
#include "PmuEvent.h"

/*
 * A StatUpdaterFunc is a function that updates a statistic (mean, variance, etc.) of eventToUpdate given
 * a statistically related event.
 */
using StatUpdaterFunc = std::function<void(const PmuEvent &relatedEvent, PmuEvent &eventToUpdate)>;

/*
 * Linear correction is a method of updating statistics. Given two statistically related events e1, e2, we might
 * now that mean e2 = 5 * mean e1. To propagate this statistical dependency to e2, we take in its expected mean
 * (in this case 5 * mean e1), and a correction weight (between (0,1] ). We calculate the difference between the current mean of
 * e2 and its expected mean. Then we linearly scale the mean of e2 to be current mean e2 + diff * correction weight.
 */

// Correct mean and variance
StatUpdaterFunc linearCorrectionUpdater(double expectedMean, double expectedVar, double correctionWeight);

// Correct only mean
StatUpdaterFunc linearCorrectionMeanUpdater(double expectedMean, double correctionWeight);

// Correct only variance
StatUpdaterFunc linearCorrectionVarUpdater(double expectedVar, double correctionWeight);

/*
 * NOTE: As of now, a PMU event only keeps track of two statistics, mean and variance. Using this structure,
 * we could possibly extend to a design where an event keeps track of a runtime defined set of statistics. There are
 * two possible ways to do this:
 *
 * (1) An edge in the EventGraph holds a list of updater functions, each function updates one statistic (like now)
 * (2) An edge in the Event graph holds only one updater function (like now), and we provide a way of composing
 * multiple updater functions into one.
 */

#endif
