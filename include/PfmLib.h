#ifndef PERFPROP_PFMLIB_H
#define PERFPROP_PFMLIB_H

#include "perfmon/pfmlib.h"

namespace PfmLib {
    /*
     * Simple RAII style class to manage initialization and termination of the library. It is enough to instantiate
     * pfm only once by initializing one PfmLib object in main. Further initializations of this class will have no effect.
     */
    class Instance {
    public:
        void initialize(){
            pfm_initialize();
        }

        ~Instance() {
            pfm_terminate();
        }
    };

    extern "C" void printAllEventsInfo();
}




#endif
