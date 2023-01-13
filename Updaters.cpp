#include "Updaters.h"

StatUpdaterFunc linearCorrectionUpdater(double expectedMean, double expectedVar, double correctionWeight){
    return [=] (const PmuEvent &relatedEvent, PmuEvent &eventToUpdate) {
        linearCorrectionMeanUpdater(expectedMean, correctionWeight)(relatedEvent, eventToUpdate);
        linearCorrectionVarUpdater(expectedMean, correctionWeight)(relatedEvent, eventToUpdate);
    };
}


StatUpdaterFunc linearCorrectionMeanUpdater(double expectedMean, double correctionWeight){
    return [=] (const PmuEvent &relatedEvent, PmuEvent &eventToUpdate) {
        double currMean = eventToUpdate.getMean().value_or(0);
        double diff = currMean - expectedMean;
        eventToUpdate.setMean(currMean + diff * correctionWeight);
    };
}

StatUpdaterFunc linearCorrectionVarUpdater(double expectedVar, double correctionWeight){
    return [=] (const PmuEvent &relatedEvent, PmuEvent &eventToUpdate) {
        double currVar = eventToUpdate.getVariance().value_or(0);
        double diff = currVar - expectedVar;
        eventToUpdate.setMean(currVar + diff * correctionWeight);
    };
}