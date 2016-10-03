# dust
I want to extend the basic functionality of consistent hashing to enable distributed, fault tolerant, load-balanced, high performance cluster software.

Should be fully distributed (no central/master node).
Adding/removing servers should have low impact in performance.
Server faults should have low impact in performance.
Fauly servers' load should be evenly distributed among remaining servers.
Should be able to detect the access patterns to the domain objects and perform optimizations like prefeching, caching, or even change the distribution model used.
