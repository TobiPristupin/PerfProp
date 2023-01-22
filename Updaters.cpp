#include "Updaters.h"

namespace Updater {

    Statistic linearCorrection(Statistic expected, Statistic current, double correctionWeight) {
        Statistic diff = expected - current;
        current += diff * correctionWeight;
        return current;
    }
}

