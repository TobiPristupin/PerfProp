# Old Design

Initially, this project was intended to be an implementation of the [BayesPerf](https://ssbaner2.cs.illinois.edu/publications/asplos2021/) paper by Banerjee Et al. However, the paper is vague about various implementation details, which made it hard to implement. Eventually it was decided to drop the strict requirement of implementing BayesPerf, and instead implement our own spinoff called PerfProp. For reference, here is the design doc that was written for the (incomplete) BayesPerf implementation before transitioning to the PerfProp implementation.

# BayesPerf CSPL

# Milestones
1. Create `EventNodes`, `FactorNodes`, and `FactorGraph`
2. Implement computing Markov Blanket for a set of events
3. Replicate perf’s cmd syntax
    1. When we receive a list of events, detect if certain events are software events. If they are not, then always schedule them and don’t run the grouping algo.
4. Add unit tests
5. Work on grouping algorithm
6. Parse and populate factor graph based on $\mu$-arch listing in Linux Source Tree

# M2: Computing Markov Blanket

Markov blanket meaning: gives you the set of variables that provide complete information on the given set of variables.

algo for finding markov blanket on factor graph: https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf

    for e1, ..., e_n
        for each e
            for each factor child in e
                for each event child in factor:
                    if event not in e1, ..., en:
                        add to MB


# M3: Replicating perfs syntax

Support commands of the form: `perf stat -e/events event1:modifiers,… {program}`

- Use a cpp command line parameter library?
    - No, `getopt` is enough for what we need and we don’t add a dependency

**Problem:** How do we determine if a passed in event is a hardware or software event? We need to make the distinction because unlimited software events can be scheduled and don’t interfere with scheduling on PMU counters, so we can safely ignore them when grouping events.
**Possible Solutions:**

    - `perf list sw` outputs a list of software events from the given architecture. We can run this command and parse the output. This is not ideal because it depends on how `perf` outputs their data.
    - Obtain events from the same source as `perf list sw`?
        - After some investigation [here](https://stackoverflow.com/questions/63653406/using-the-perf-events-from-perf-list-programatically) and looking at the sources for `perf`, it is not really feasible to access the same data sources because they depend on kernel data and complicated readings from the system. 
    - Use a library like `jevents`?
        - Appears to only work with Intel based CPUs

**Chosen Solution:**  Parse `perf` output. This will be the easiest approach. Determining software events is a minor problem and we shouldn’t spend major time on it, so parsing `perf` output works for now. We can revisit this later if we need more robustness.
NOTE: to obtain hardware software events… map perf list to full name for config field, and then inspect structs to see which struct it is on? see pmu project


# M4: Unit Tests
- What testing framework to use?
    - Google test seems to be the most well known and simple to set up
# M5: Grouping/scheduling algorithm
- Grouping algorithm:
    - **Input:** A set of PMU events to be measures
    - **Output:** Given a set of events, we want to group them into groups such that they share some statistical dependencies between groups. This will allow us to perform inference on the samples of the events to accurately predict the values of events that were multiplexed out.
- **Assumptions**:
    - We have a set size per group. Since most architectures have ~3-4 programmable PMU counters, we can have our program detect how many available counters there are, and make all groups exactly that size. There is no point in making them bigger (they won’t be able to be scheduled), and there is no point in making them smaller (the more events we can schedule the better).
        - This ignores the fact that some events only work on specific PMU counters, so if you have 3 PMU counters and 3 events, it might be the case that it still can’t be scheduled. For simplicity I am ignoring this, and will later tweak the algorithm to adjust for this.
- According to the paper, if we have two groups $C_1,C_2$, we can determine if there is a statistical relationship between the groups by computing the markov blankets of both groups. If the intersection of both blankets is non-empty, then we have some statistical dependence.
- **Question:** Our algorithm has two opposing forces: We want to maximize how “statistically dependent” consecutive groups are, and we want to minimize how many groups we create (while still including every element in at least one group).
    - We can measure the total “statistical dependence” by computing the markov blankets of adjacent groups, taking the intersection, and taking the size of that intersection. A larger intersection means there is more statistical dependence.
    - A possible approach would be to fix the number of groups, and then attempt to maximize the total dependence for that fixed number of groups.
        - We could attempt to find the minimal number of groups that cover every element with some statistical dependency among consecutive groups. That is our fixed number of groups. Then we can attempt to improve total statistical dependence for that number of groups.
    - **Question for @Ao**:  I’m unsure about the algorithm that I’m proposing above. Fixing the number of groups to the minimum amount may be very restrictive. I think we need a greater understanding of how inference will be performed on the output of this algorithm, so we can understand how important it is to prioritize less groups vs more statistical dependence.


## Generating Groups:

Say we have a list of events $E$, a group of events $C=\{e_1, e_2, e_3\}$ that generates markov blanket $B=\{e_4, e_7, e_5, e_9\}$. How can we compute the next group?

Note that the markov blanket of an event $e$ is the set of all event node neighbors of $e$ in the factor graph. The factor graph is a bipartite graph that connects a set of events to a set of “factor” nodes that encode relationships between event nodes. For an event $e$, any event $e'$ that is reachable from $e$ by travelling to a factor node and back to an event node is considered a neighbor.

If group $C_i$ contains events $\{e_1,e_2, e_3\}$, then we can construct the next group by repeating one of the elements of $C_i$ and filling the other spots with other non-grouped events. Therefore, $C_{i+1}$ would look like $\{e_1, e_4, e_5\}$ where $e_1$ is the duplicated element and $e_4,e_5$ are elements that have not been grouped yet. Because both groups contain $e_i$, it is almost guaranteed that the markov blankets of $C_i$ and $C_{i+1}$ will have some overlap (caused by the elements that $e_1$ contributes to the markov blanket of each).


## Proposed Algorithm

**Input:**

- A set $E$ of PMU events to be grouped
- `groupSize` integer parameter
-  `overlapSize` integer parameter

**Output:** A list of $(C_1, \ldots, C_n)$ groups where each $C_i$ is a set of $k$ events, and consecutive groups (almost always) have statistical dependencies between them. That is, $blanket(C_i)\cap blanket(C_{i+1}) \neq \emptyset$.

Note: For now, we are not checking to see if a possible grouping is valid. Certain groupings might not be able to be scheduled all at once due to architectural constraints.

Generate our first group $C_1$ by repeating the following procedure until the group is full:

    1. Compute the blanket of $C_1$ (initially empty).
    2. Add an element from $E$ not in $C_1$ or the blanket of $C_1$

> This approach will maximize the size of the markov blanket. This is because if an element $e$ is in $C_1$, then by definition $e$ cannot be in the blanket of $C_1$. So, it is ideal to choose elements **not** in the current markov blanket, because if we choose an element $e$ that is already in the blanket, then we lose $e$ from the blanket.

While not all events in $E$ are in at least one group:

    Let $C_{i-1}$ be the latest generated group. Compute $C_i$ as follows: randomly choose `overlapSize` elements from $C_{i-1}$. Then, choose elements from $E$ that are not grouped until $C_i$ has size `groupSize`. How do we choose elements from $E$? Again choose elements by selecting ungrouped elements not in the current markov blanket. 

This algorithm will eventually group every event at least once and terminate.  This is because at every iteration of the while loop we add at least one non-grouped element. In the worst case our algorithm will require $\Theta(|E|)$ iterations.

In terms of implementation details, we need a hash set that can determine in $\Theta(1)$ if an event has been already grouped or not. We repeat the while loop until this set has size $|E|$.

As discussed above, there are two dimensions to a grouping algorithm: how many groups it generates in order to group all events, and how strong the statistical dependencies are between consecutive groups. The proposed algorithm allows the user to tune this balance via the `overlapSize` parameter.

This is a greedy algorithm that will not necessarily produce the optimal grouping. The greediness comes from greedily choosing ungrouped elements that are not in the current markov blanket.


# M6: Populate Factor Graph

When BayesPerf is called with a set of events, we need to create the factor graph with those events, and encode the statistical relationships between them. These statistical relationships are algebraic relationships between events that encode micro-architectural invariants.

According to the slides, the factor graph can be created by parsing the JSON micro-architecture listings in the Linux source tree, which are provided by CPU vendors. From my inspection of the Linux source tree, I believe these are stored [here](https://github.com/torvalds/linux/tree/master/tools/perf/pmu-events/arch). For every architecture there is a folder which contains a list of JSON files that defines the events for that CPU.

For Intel CPUs, there is always a `{arch}-metrics.json` file which contains metrics and their corresponding algebraic expression. For example, this is a metric for the Sandy Bridge Intel architecture:


    "BriefDescription": "This metric represents fraction of slots the CPU was stalled due to Frontend latency issues",
            "MetricExpr": "4 * min(CPU_CLK_UNHALTED.THREAD, IDQ_UOPS_NOT_DELIVERED.CYCLES_0_UOPS_DELIV.CORE) / SLOTS",
            "MetricGroup": "Frontend;TopdownL2;tma_L2_group;tma_frontend_bound_group",
            "MetricName": "tma_fetch_latency",
            "PublicDescription": "This metric represents fraction of slots the CPU was stalled due to Frontend latency issues.  For example; instruction-cache misses; iTLB misses or fetch stalls after a branch misprediction are categorized under Frontend Latency. In such cases; the Frontend eventually delivers no uops for some period. Sample with: RS_EVENTS.EMPTY_END",
            "ScaleUnit": "100%"

The `"``MetricExpr``"` field holds the algebraic expression.

For AMD, metrics appear to be stored in `recommended.json` and `pipeline.json`.

**Question:** There seems to be a difference between “Events” and “Metrics” in the JSON files. Events do not have algebraic expressions corresponding to them. Metrics do. Metrics are stored in a special `metrics.json` file, whereas events are stored in JSON files corresponding to various categories, such as `cache`, `branch`, `core`, etc. My uncertainty arises from the following: From the BayesPerf paper, I imagine that all (or almost all) PMU events that the user can request to be measured have an algebraic relation. This is because inference is performed based on these relations, so we need the relations to perform inference. Does that mean that the PMU events that the user can request are all “Metrics” and are all in the `metrics.json` file? Or can the user request a PMU event that is an “Event” and has no algebraic relation? I may also be misunderstanding the role of the algebraic relations in performing inference. Maybe we don’t need a relation for *every* PMU event we want to measure, because we can perform good enough inference with only some algebraic relations.

???
metrics appear to be derived from events
metrics work in perf with -M
paper says that “Remember from above that the statistical dependencies between the events are specified as joint probability functions”. How do we go from an algebraic relation to a joint probability function?


**Useful links**

https://hadibrais.wordpress.com/2019/09/06/the-linux-perf-event-scheduling-algorithm/


    