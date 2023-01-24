# Summary
PerfProp is a Linux program to improve PMU performance monitoring. It works on top of the Linux perf subsystem. PerfProp measures a set of PMU events for a command, just like the perf stat command. However, PerfProp allows the user to define statistical relationships between events, such as "mean count of event A should be twice the mean count of event B". PerfProp will then use these statistical relationships to provide more accurate measurements than those provided by Perf.   

PerfProp is a project by the [Computer Security & Privacy Laboratory (CSPL)](https://cybersecurity.seas.wustl.edu/) at Washington University in St. Louis.

Project planning and in depth explanations included [here](https://www.dropbox.com/scl/fi/qo7t0u4j3yzneqs01u65u/Bayes-Perf-CSPL.paper?dl=0&rlkey=nq9a95lenmi18jh8jc72dth1s):

# Motivation
PMU events are used by the Linux perf tool to measure various performance metrics. Because there is often a need to measure vastly more events than the physical PMU counters can support, perf will perform "multiplexing" on events, where events are swapped in and out of the counters. Multiplexing causes major losses in accuracy. PerfProp attempts to mitigate these errors in measurements by correcting them through statistical relationships between events. This work was motivated by the [BayesPerf system](https://ssbaner2.cs.illinois.edu/publications/asplos2021/)

# Installation
This program requires `perf` and `lipbfm`. `perf` can be installed using a package manager. To install `libpfm`, follow these steps:
1) Download from here: https://sourceforge.net/p/perfmon2/libpfm4/ci/master/tree/
2) Extract, then run `make` and `make install`. 
3) By default, this will install the library in `/usr/local/lib`, and PerfProp will automatically find it. If another location is used for the installation, change `find_library(PFM_LIB pfm)` in the PerfProp root level`CmakeLists.txt` to search for the library in that location. Read `libpfm`'s README for how to change the installation location.

Once installed, build PerfProp like a standard `CMake` project. In PerfProp's root, create a `build` directory. From that directory, run `cmake ..` and `make`.

# Usage
perfprop {command} {args}

The stat command traces given PMU events on a program.
Usage: perfprop stat --events {events} {program}
Where events is a comma delimited list of events with no spaces
Ex: perfprop stat --events cycles,cache-misses,instructions dd if=/dev/zero of=/dev/null count=1000000

The list command outputs all the available PMU events. Will probably be useful to pipe them into a file for future reference.
Usage: perfprop list

# Coding Style

See [style guidelines](Style.md)

