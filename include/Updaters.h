#ifndef PERFPROP_UPDATERS_H
#define PERFPROP_UPDATERS_H

#include <functional>
#include <optional>
#include "PmuEvent.h"
#include "SampleCollector.h"

namespace Updater {

    /*
     * Linear correction is a method of updating statistics. Given two statistically related events e1, e2, we might
     * now that mean e2 = 5 * mean e1. To propagate this statistical dependency to e2, we take in its expected mean
     * (in this case 5 * mean e1), and a correction weight (between (0,1] ). We calculate the difference between the current mean of
     * e2 and its expected mean. Then we linearly scale the mean of e2 to be current mean e2 + diff * correction weight.
     */
    Statistic linearCorrection(Statistic expected, Statistic current, double correctionWeight);
}


#endif
