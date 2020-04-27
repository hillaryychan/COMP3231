# Tutorial Week 10

## Questions

### Virtual Memory

**1.** What effect does increasing the page size have?

* It increases internal fragmentations, and the unit of allocation is now greater, hence greater potential for un-used memory when allocation is rounded up. Practically speaking, this is not usually an issue, and round up an application to the next page boundary usually results in a relatively small amount wasted compared to the application size itself
* Decreases number of pages. For a given amount of memory, larger pages results in fewer page table entries, thus smaller (and potentially faster to lookup) page table
* Increases TLB coverage as the TLB has a fixed number of entries, thus larger entries cover more memory, reducing miss rate
* Increases page fault latency as a page fault must load the whole page into memory before allowing the process to continue. Large pages have to wait for a longer loading time per fault
* Increases swapping I/O throughput. Given a certain amount of memory, larger pages have higher I/O
* Increasing the working set size. Work set is defined as the size of memory associated with all pages accessed within a window of time. The larger the pages, the more potential for pages to include significant amounts of unused data, thus working set size generally increases with page size

**2.** Why is demand paging generally more prevalent than pre-paging?

Pre-paging requires predicting the future (which is hard) and the penalty for making a mistake may be expensive (may page out a needed page for an unneeded page.

**3.** Describe four replacement policies and compare them.

1. Optimal
    * toss the page that won't be used for the longest time
    * impossible to implement
    * only good as a theoretical reference point: the closer a practical algorithm gets to optimal, the better
2. FIFO
    * first-in, first-out: toss, the oldest page
    * easy to implement
    * age of a page isn't necessarily related to usage
3. LRU
    * toss the least recently used page
    * assumes that page that has not been referenced for a long time is unlikely to be referenced in the near future
    * will work if locality holds
    * implementation requires a time stamp to be kept for each page, updated on every reference
    * impossible to implement efficiently
4. clock
    * employs a usage or reference bit in the frame table
    * set to one when page is used
    * while scanning for a victim, reset all the reference bits
    * toss the first page with a zero reference bit

**4.** What is thrashing? How can it be detected? What can be done to combat it?

Thrashing is where the sum of the working set sizes of all processes exceeds available physical memory and the computer spends more and more time waiting for pages to transfer in and out.  
It can be detected by monitoring page fault frequency  
Some processes can be suspended and swapped out to relieve pressure on memory

### Multi-processors

**5.** What are the advantages and disadvantages of using a global scheduling queue over per-CPU queues? Under which circumstances would you use the one or the other? What features of a system would influence this decision?

A global queue is simple and provides automatic load balancing, but it can suffer from contention on the global scheduling queue in the presence of many scheduling events or many CPUs or both. Another disadvantage is that all CPUs are treated equally, so it does not take advantage of hot caches present on the CPU the process was last schedules on.

Per-CPU queues provide CPU affinity, avoid contention on the scheduling queue, however they require more complex schemes to implement load balancing

**6.** When does spinning on a lock (busy waiting, as opposed to blocking on the lock, and being woken up when it's free) make sense in a multiprocessor environment?

Spinning makes sense when the average spin time spent spinning is less than the time spent blocking the lock requesting, switching to another process, switching back, and unblocking the lock requester

**7.** Why is pre-emption an issue with spinlock?

Spinning wastes CPU time and indirectly consumes bus bandwidth. When acquiring a lock, an overall system design should minimise the time spent spinning, which implies minimising the time a lock holder holds the lock. Pre-emption of the lock holder extends lock holding time across potentially many time slices, increasing the spin time of lock acquirers.

**8.** How does a read-before-test-and-set lock work and why does it improve scalability?

It reads the value of the test-and-set and only performs the test-and-set when it becomes free.  
This improves scalability as the spinning is on a read instruction (not a test-and-set instruction), which allows the spinning to occur on a local-to-the-CPU cached copy of the lock's memory location. Only when the lock changes state is cache coherency traffic required across the bus as a result of invalidating the local copy og the lock's memory location, and the test-and-set instruction which requires exclusive access to the memory across all CPUs.

### Scheduling

**9.** What do the terms _I/O bound_ and _CPU bound_ mean when used to describe a process (or thread)?

The time to completion of a **CPU-bound** process is largely determined by the amount of CPU time it receives

The time to completion of an **I/O-bound** process is largely determined by the time take to service its I/O requests. CPU time plays little part in the completion time of I/O-bound processes

**10.** What is the difference between cooperative and pre-emptive multitasking?

Cooperative threads specifically release access to the CPU, while pre-emptive threads have no choice

**11.** Consider the multilevel feedback queue scheduling algorithm used in traditional Unix systems. It is designed to favour IO bound over CPU bound processes. How is this achieved? How does it make sure that low priority, CPU bound background jobs do not suffer starvation?

Note: Unix uses low values to denote high priority and vice versa, 'high' and 'low' in the above text does not refer to the Unix priority value.

I/O bound processes typically have more short CPU bursts. If such process is scheduled, it will typically not use up it's time slice. Priorities are recomputed taking into account the consumed CPU, and hence an I/O-bound process will end up having higher priority than a process that started at the same priority level and which used more CPU cycles.

Even a process with low priority will be scheduled eventually, since the priority of processes that are continually scheduled eventually receive a low or lower priority.

Also note that the recorded amount of CPU consumed (that is used to calculate priority) is aged (reduced) over time, and hence CPU-bound processes also increase in priority as they are not scheduled

**12.** Why would a hypothetical OS always schedule a thread in the same address space over a thread in a different address space? Is this a good idea?

The context switch is faster. Better locality, If done too often it starves other tasks

**13.** Why would a round robin scheduler NOT use a very short time slice to provide good responsive application behaviour?

CPU is consumed when switching from one task to another. This switching does not contribute to application progress. If done very frequently (a very short time slice), a significant portion of available CPU will be wasted on scheduler overhead

### I/O

**14.** Describe _programmed I/O_ and _interrupt-driven I/O_ in the case of receiving input (e.g. from a serial port). Which technique normally leads to more efficient use of the CPU? Describe a scenario where the alternative technique is more efficient.

**15.** A device driver routine (e.g. `read_block()` from disk) is invoked by the file system code. The data for the filesystem is requested from the disk, but is not yet available. What do device drivers generally do in this scenario?

**16.** Describe how I/O buffering can be formulated as a _bounded-buffer producer-consumer problem_.

**17.** An example operating system runs its interrupt handlers on the kernel stack of the currently running application. What restriction does this place on the interrupt handler code? Why is the restriction required?

Page last modified: 2:40pm on Sunday, 19th of April, 2020
