# Milestones
1. Create `EventNodes`, `FactorNodes` and `FactorGraph`
2. Implement computing the markov blanket for a set of events



## M1: Implement computing the markov blanket for a set of events
algo for finding markov blanket: https://ai.stanford.edu/~koller/Papers/Abbeel+al:JMLR06.pdf
for e1, ..., e_n
    for each e
        for each factor child in e
            for each event child in factor:
                if event not in e1, ..., en:
                    add to MB

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

# Reliable and Efficient Performance Monitoring in Linux Paper
* Scheduling is per core. Operates at the event group granularity
* How the algo works: Given a linked list of groups, the algo will then try to schedule the first group. Then the 
  first and second. Then the first, second, third. When the algorithm fails to schedule the first k groups, it 
  generates the configuration for the previous pass (k-1 elements). The algorithm also stops when k=# of registers.
  * This generates one configuration. Then, the linked list is rotated. The tail comes to the head. And the 
    algorithm is generated again, generating a new configuration.
  * The details of how the algo assigns the first k groups to the counters is also specified. It is a greedy first-match
    strategy, where if an event/group is assigned to a set of counters, it is never moved.

# Probabilistic graphical models book
   1) Factor graph: pages 123, 154, 418
   2) Markov Network 109
   3) Markov blanket 512
   

# Misc Notes
