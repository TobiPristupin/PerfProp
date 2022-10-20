# Implementation
* Scheduling: Form the groups based on overlapping statistical dependencies
* Statistical dependencies: extracted from microarchitectural invariants


# Scheduling:
"Given (from a profiling application) an original schedule of configurations 𝐶1 → 𝐶2 → · · · → 𝐶𝑛, where 𝐶𝑖 executes in 
time slice 𝑖, into another schedule of 𝐶′𝑖 s such that transitive statistical relationships hold, such that the validity 
criteria holds on each 𝐶′𝑖 . In the case when it is not possible ensure the validity criteria on every 𝐶′𝑖 , we break the 
chain of repeated events, and start over again from a valid configuration."

Question: How do we obtain this original list of C1, ..., Cn before converting it? 

One option is to instrument the kernel and log this information. However, for now, we can simply generate groupings 
ourselves such that we maximize overlap, and then try to see if they are valid groupings by using the kernel's 
builtin validity checker. 

Problem is now: Given n events, assemble them into k groups such that group G_i has some overlap with group G_i+1, and 
so on. we want to maximize overlap, and hopefully also minimize k. 

Thoughts: The number of events is n~30-50. 

Approach: 
1) Compute factor graph. One node for each event and probability function, and one edge from event to 
function if they are related. 
2) Generate groups. This is the hard part
3) For each group, we can determine if there is a transitive dependency by computing the markov blanket. 

# Reliable and Efficient Performance Monitoring in Linux Paper
* Scheduling is per core. Operates at the event group granularity
* How the algo works: Given a linked list of groups, the algo will then try to schedule the first group. Then the 
  first and second. Then the first, second, third. When the algorithm fails to schedule the first k groups, it 
  generates the configuration for the previous pass (k-1 elements). The algorithm also stops when k=# of registers.
  * This generates one configuration. Then, the linked list is rotated. The tail comes to the head. And the 
    algorithm is generated again, generating a new configuration.
  * The details of how the algo assigns the first k groups to the counters is also specified. It is a greedy first-match
    strategy, where if an event/group is assigned to a set of counters, it is never moved.

# Progress
1) Working on obtaining the original schedule of configurations from perf. Not sure if perf exposes this information 
   to userspace. 
   1) Asked in this blog post for help: https://hadibrais.wordpress.com/author/hadibrais/. Answer was that it's not 
      possible
   2) Look into not using perf to obtain this scheduling? Such as OProfile or perfctr
      1) Oprofile? Couldn't find anything
      2) likwid-perfct forces a configuration, it does not invoke the scheduler.
2) Each scheduler configuration is created on demand. That is, C_2 will be created by perf after C_1 is created, 
   scheduled, and then an interrupt occurs, forcing the scheduler to run again. So we need to run perf for n 
   scheduler time units if we want to obtain C_1, ..., C_n
   1) Can we let perf run, inspect our counters constantly until we see a cycle?
      1) Gets really complicated because what if we miss a scheduling? What if other running programs begin to 
         call perf_event_open with new events, changing things? Also, have not been able to find tools to inspect 
         the current counter config, but it seems possible.
3) After we create our C'_i, how do we schedule them? We could call perf_event_open with each group and call it in 
   ascending order.
4) Read probabilistic graphical models
   1) Factor graph: pages 123, 154, 418
   2) Markov Network 109
   3) Markov blanket 512

algo for finding markov blanket: https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf
for e1, ..., e_n

for each e
    for each factor child in e
        for each event child in factor:
            if event not in e1, ..., en:
                add to MB