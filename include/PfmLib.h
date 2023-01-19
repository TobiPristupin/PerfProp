#ifndef BAYESPERF_CPP_PFMLIB_H
#define BAYESPERF_CPP_PFMLIB_H

#include "perfmon/pfmlib.h"

/*
 * Simple RAII style class to manage initialization and termination of the library. It is enough to instantiate
 * pfm only once by initializing one PfmLib object in main. Further initializations of this class will have no effect.
 */
class PfmLib {
public:
    void initialize(){
        pfm_initialize();
    }

    ~PfmLib() {
        pfm_terminate();
    }
};


#endif
