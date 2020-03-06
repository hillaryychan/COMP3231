# Computer Hardware Review (Memory Hierarchy)

Operating systems exploit the hardware available and provide a set of high-level services that represent or are implemented by hardware. It manages the hardware reliably and efficiently. Understanding operating systems requires a basic understanding of the underlying hardware.

There is a memory hierarchy where going down the hierarchy, there is

* decreasing cost per bit
* increasing capacity
* increasing access time
* (hopefully) decreasing frequency of access to the memory by the processor (based on the principle of locality)

![Memory hierarchy](imgs/7-4_memory-heirarchy.png)

Given two levels of data storage: small and fast vs. large and slow, we can speed access to slower storage by using an intermediate-speed storage as a **cache**

![caching](imgs/7-6_caching.png)

![cpu cache](imgs/7-7_cpu-cache.png)

CPU cache is fast memory placed between the CPU and main memory with 1 to a few cycles access time compared to RAM access time of tens to hundreds of cycles. It holds recently used data or instructions to save memory accesses. The CPU cache matches slow RAM access time to CPU speed if it has a high hit rate. It is hardware maintained and (mostly) transparent to software. The size of the cache ranges from a few kB to tens of MB. There is usually a hierarchy of caches (2-5 levels), on- and off-chip.

The effective access time of memory subsystem depends on the hit rate in the first level.

![effective access time](imgs/7-9_effective-access-time.png)

Example:  
cache memory access time is 1ns  
main memory access time is 10ns  
hit rate is 95%

![effective access time example](imgs/7-10_access-time-example.png)
