#ifndef BAYESPERF_CPP_PFMLIB_H
#define BAYESPERF_CPP_PFMLIB_H

#include "perfmon/pfmlib.h"

/*
 * Simple RAII style class to manage initialization and termination of the library. This class needs to be
 * instantiated only once.
 */
class PfmLib {
public:
    PfmLib(){
        pfm_initialize();
    }

    ~PfmLib() {
        pfm_terminate();
    }
};


#endif
