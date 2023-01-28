# PerfProp CSPL

## Overview 

### High Level 
The program should be able to take in a list of PMU events and a program to run. It should also support a method to input mathematical/statistical relationships between the mean/variances of these events (currently we need to hardcode these in code, see [Reading Statistical Dependencies](#reading-statistical-dependencies)). It should then run the program in a child process, and use `perf_event_open` to register all the events that we want to measure. Then, it should periodically read samples from each event and calculate mean/variance for that event. These samples should trigger updates in the mean/variances of related events. In other words, sample data is propagated across a network of related events (that is where the "Prop" in "PerfProp" comes from).

## Replicating perfs syntax

Support commands of the form: `perf stat -e/events event1:modifiers,… {program}`

- Use a cpp command line parameter library?
    - No, `getopt` is enough for what we need and we don’t add a dependency

**Problem:** How do we determine if a passed in event is a hardware or software event? It is important to make this distinction because software events are measured in software, and thus they don't actually use a physical PMU counter. Unlimited software events can be scheduled, but not hardware events. Note that "hardware events" for this case refers to every event that is not a software event (like Kernel PMU Events, etc.). 

**Possible Solutions:**

- `perf list sw` outputs a list of software events from the given architecture. We can run this command and parse the output. This is not ideal because it depends on how `perf` outputs their data.
- Obtain events from the same source as `perf list sw`?
    - After some investigation [here](https://stackoverflow.com/questions/63653406/using-the-perf-events-from-perf-list-programatically) and looking at the sources for `perf`, it is not really feasible to access the same data sources because they depend on kernel data and complicated readings from the system. 
- Use a library like `jevents`?
    - Appears to only work with Intel based CPUs
- Use `libpfm`

**Chosen Solution:**  Initially, parsing perf output was the solution chosen. However, later on we discovered `libpfm`, which directly supports this feature and is the obvious choice. The current codebase now uses `libpfm`.


## Unit Tests
What testing framework to use?
- Google test seems to be the most well known and simple to set up

## Grouping Events
Perf allows a set of events to be grouped together, which means that those events are all scheduled/unscheduled together as one unit (i.e. there is no multiplexing between events in the same group). By default, free events are treated as a group of size 1.

The old BayesPerf implementation had a custom grouping algorithm that would group events based on their statistical dependencies. PerfProp doesn't really have the need to group events at runtime. The code to support grouping is still there, see `EventGrouper`. Group size for events should always be at most the number of PMU programmable counters available, which is given by `Perf::numProgrammableHpcs()`. 

**TODO**: 
- We should allow the user to group events on their own, just like how perf does it with the "{}" syntax. See `perf stat -h` for more details
- Do we need a runtime grouping algorithm at all? Or can we get rid of `EventGrouper` and simply let the user group events on their own if they want to? 

### Validity of Groups
If we want to group events, we must be able to detect if a grouping is valid. An invalid grouping will never be scheduled. An invalid grouping can be generated if we have more events than programmable PMU counters. We can account for that using `Perf::numProgrammableHpcs()`. However, individual events may have specific constraints as to what PMU counter they can be measured on, and conversely a PMU counter may be restricted in the set of events that it can measure. To determine if a group is valid, it is best to leave it to the builtin perf validity checker (this is also the approach that BayesPerf uses). When calling `perf_event_open` to add an event to a group, the function will return `EINVAL` if the group is invalid. The `EINVAL` error code is overloaded to encompass various error situations, which is why this isn't very well documented in `man perf_event_open`.

## Encoding Events

The user will pass in via the command line the names of the events to be measured. We must then call `perf_event_open` with these events. `perf_event_open` does not accept the names of events, but instead uses a more involved way of representing an event, using the `type`, `config`, and `config2` fields of `perf_event_attr`. Furthermore, all architecture specific events are encoded with a unique hex code that must be passed in to `perf_event_open` (these are events of type `PERF_TYPE_RAW`). Architecture specific events are defined in [Linux's source](https://github.com/torvalds/linux/tree/master/tools/perf/pmu-events/arch).

**Approaches to converting an event's name to its encoding:**
1. Parse the `json` files in the Linux source tree that map events to their raw codes. This would be only for architecture specific events. For standard perf events, we would need to implement the logic described in `man perf_event_open`.
2. Implement the logic that perf uses internally by copying its source code. This logic is described [here](https://github.com/torvalds/linux/blob/master/tools/perf/pmu-events/README). It is very complex and depends on a Python script that dynamically generates a C program that is then compiled and linked.
3. Use `libpfm`, which provides a function `pfm_get_os_event_encoding()`, that performs the exact conversion that we need. Like always, there is one caveat: The naming scheme for architecture specific events seems to be slightly different from the one used by `perf`. For example, On AMD Zen 2, perf will recognize the event `ex_ret_instr` but `libpfm` will fail to recognize. Instead, the library writes it as `RETIRED_INSTRUCTIONS`. Perf uses the names taken from the Linux source tree and uploaded by CPU vendors, so those are clearly the official names. [I have asked the mailing list of libpfm for clarification](https://sourceforge.net/p/perfmon2/mailman/perfmon2-devel/) and am waiting for moderator approval. In the meantime, If we want to use `libpfm`, we have to name events by their alternative `libpfm` names, instead of using the official Linux source names.

**Decision taken:** `libpfm` is by far the easiest approach and works great after some testing. The discordance with the naming scheme can be fixed by providing a `perfprop list` command, that outputs the `libpfm` names for every event (just like `perf list` does). 

**Installing libpfm4:**

`https://sourceforge.net/p/perfmon2/libpfm4ci/master/tree/`

Extract, then run `make` and `make install` in source folder. By default, this will install the library in `usr/local/lib`, and the Makefile for the program will find the library. If another location is used for the install, you may have to modify the root level makefile by changing the `find_library(PFM_LIB pfm)` command to search for the library somewhere else. More compilation option for pfmlib (as well as how to install in another location) can be found in their README.



## Reading Samples

Perf supports a *counting* read mode where one can periodically call `read` on a file descriptor, and a *sampling* method where an event periodically writes to a buffer and can trigger an epoll notification. The sampling method triggers writes to the buffer whenever the event's count overflows a certain threshold. Although being able to be notified of writes by perf is great, it is very hard to programatically set the overflow threshold for an event that is passed in by the user, since every event has wildly different frequencies. 

The approach that was taken was to use the standard counting mode, and read from all events in a round robin fashion. Currently, we read all events and process their samples, then wait 200ms and repeat:

``` C
while (!tracedProcess->hasTerminated()){
        readAndProcessSamplesOneRound(fdsToEvent, sampleCollector.get());
        int millis = 200;
        usleep(millis * 1000);
    }
```

The 200ms was simply added to slow down the read frequency for debugging.

**TODO**: Allow the user to pass in a read frequency as a command line parameter?

## Measuring Mean and Variance

Perf allows us to measure the count of an event (the amount of times it was triggered). It also gives us a measure of the time that an event was actively being measured, and the total time that it was enabled (i.e. total runtime of the program). The difference between these two is the time the event was multiplexed. From this data, we want to measure a meaningful statistic that lends itself well to mean/variance calculations.

Two approaches:
1. Read event samples in a round robin fashion. To calculate the mean, simply average out values across reads, and make sure to reset counts after every read. The problem with this approach is that we can’t ensure that reads to the same event happen at the same time intervals. For example, the first read might yield a count of 100, and the second read a count of 200 because perfprop was unscheduled by the OS and thus took a long time to perform the second read. It is not reasonable then to say that the mean is 150. And even if perfprop can guarantee that it can read from an event exactly every X ms, we can’t ensure that the event itself wasn’t multiplexed, leading to different counts across reads. 
2. Measure event counts / millisecond (or any other time unit). Perf allows us to obtain the time that an event was running (i.e. being actively measured), and the total time it was enabled (i.e. running time + time where event wasn’t measured due to multiplexing). Measuring per millisecond allows us to control for the time an event was being multiplexed out. Ex:
    - First read: count of 100, running time of 2 sec, total time 2 sec
    - Second read:  count of 80, running time of 1 sec, total time 2 sec
    - Average: (100/2 + 80/1) / 2 = 65 counts per sec.

Conclusion: Approach 2 better. Note that perf doesn’t reset time counts across reads, so the program will have to manually compute time differences across reads by subtracting the current time from the time of last read.

## Statistical Relationships

Statistical relationships must be encoded in a way such that they can be used at runtime. Currently, we use `StatUpdaterFunc`, which is documented in more detail in `SampleCollector.h`. With this approach, we must hardcode statistical dependencies in code, as seen here

```c++
collector->addRelationship(pmuEvents.at(0), pmuEvents.at(1), [&] (const PmuEvent::Stats& relatedEventStats,
                                                              PmuEvent::Stats &eventToUpdate) {
        /*
         * Here we have to modify eventToUpdate. We can do something simple like
         * ->   eventToUpdate.meanCountsPerMillis = relatedEventStats.meanCountPerMillis;
         * which would set the mean of the event to update to be the same as the event that received the new sample.
         *
         * Or, we can employ more advanced techniques such as using a linear corrector, as seen below
         *
         * In this function, only update mean and/or variance for eventToUpdate.
         */
        eventToUpdate.meanCountsPerMillis = Updater::linearCorrection(
                2*relatedEventStats.meanCountsPerMillis, eventToUpdate.meanCountsPerMillis, 0.2);
    });
```

### Statistical Dependencies at Runtime (TODO)
Allow the user to add statistical dependencies, instead of requiring them to be hardcoded. To do so, we first need to input the dependencies somehow. I believe the easiest way would be to define a `json` format that can describe mathematical dependencies between two events, something like this

```json
[{
    "eventFrom": "L1_MISSES",
    "eventTo": "L2_MISSES",
    "expectedMean": 2 * {mean}, 
    "expectedVariance": {variance} + 50,
}]
```

This would represent that we expect the mean of `L2_MISSES` to be twice the mean of `L1_MISSES`, and similarly the variance of `L2_MISSES` should be the variance of `L1_MISSES` + 50.

PerfProp would then take in a `json` file with all the dependencies, parse it, and generate the according `StatUpdaterFunc` for each dependency. Mathematical expressions can be quickly parsed using a recursive descent parser, which can generate an Abstract Syntax Tree (AST) of the mathematical expression. ASTs of mathematical expressions can be easily evaluated. We would use the AST to fill in the `StatUpdaterFunc`.

Using `json` files would also allow perfprop to integrate some of BayesPerf's ideas. See [next section](#bayesperfs-approach--and-weakness-)

#### BayesPerf's Approach (and Weakness?)

BayesPerf claims that there are existing `json` files in the Linux source tree that encode statistical relationships between events. These relationships are based on micro-architectural invariantsm and are provided directly by CPU vendors. They are stored [here](https://github.com/torvalds/linux/tree/master/tools/perf/pmu-events/arch). For every architecture there is a folder which contains a list of JSON files that defines the events for that CPU, as well as metrics, which are simply events that are composed of the values of other events. For Intel CPUs, there is always a `{arch}-metrics.json` file which contains metrics and their corresponding algebraic expression. For example, this is a metric for the Sandy Bridge Intel architecture:

```json
{
    "BriefDescription": "This metric represents fraction of slots the CPU was stalled due to Frontend latency issues",
    "MetricExpr": "4 * min(CPU_CLK_UNHALTED.THREAD, IDQ_UOPS_NOT_DELIVERED.CYCLES_0_UOPS_DELIV.CORE) / SLOTS",
    "MetricGroup": "Frontend;TopdownL2;tma_L2_group;tma_frontend_bound_group",
    "MetricName": "tma_fetch_latency",
    "PublicDescription": "This metric represents fraction of slots the CPU was stalled due to Frontend latency issues.  For example; instruction-cache misses; iTLB misses or fetch stalls after a branch misprediction are categorized under Frontend Latency. In such cases; the Frontend eventually delivers no uops for some period. Sample with: RS_EVENTS.EMPTY_END",
    "ScaleUnit": "100%"
}
```

The `"``MetricExpr``"` field holds the algebraic expression. For AMD, metrics appear to be stored in `recommended.json` and `pipeline.json`. However, only metrics contain these mathematical relationships. Metrics are a small subset of all the possible measurable events. This leads me to believe that BayesPerf's approach only works for metrics, not for all events. 

### Linear Correction Updater

Linear correction is a method of updating statistics between related events that deserves some explanation. Given two statistically related events e1, e2, we might know that `mean e2 = 5 * mean e1`. Whenever we receive a sample for `e1`, we want to use that sample to update the mean of `e2`. However, we don't want to simply set the mean of `e2` to be exactly `5 * mean e1`, since `e2` has its own samples and its own mean. Linear correction achieves an update by slightly correcting the mean of `e2` towards its expected value. The strength of that correction can be tuned. The algorithm is as follows:

```C
function update_mean(curr_mean_1, curr_mean_2):
      expected_mean_2 = curr_mean_1 * 5 //We expected that mean_2 ~ 10*mean_1
      diff = expected_mean_2 - curr_mean_2 //Diff between current and expected
      correction_weight = 0.25 //A higher weight gives more weight to corrections
      corrected_mean_2 = curr_mean_2 + correction_weight * diff
      return corrected_mean_2
```

## Other

**Useful links**
- https://hadibrais.wordpress.com/2019/09/06/the-linux-perf-event-scheduling-algorithm/
- https://man7.org/linux/man-pages/man2/perf_event_open.2.html


    