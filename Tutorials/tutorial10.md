# Tutorial Week 10

## Questions

### Virtual Memory

1. What effect does increasing the page size have?

2. Why is demand paging generally more prevalent than pre-paging?

3. Describe four replacement policies and compare them.

4. What is thrashing? How can it be detected? What can be done to combat it?

### Multi-processors

5. What are the advantages and disadvantages of using a global scheduling queue over per-CPU queues? Under which circumstances would you use the one or the other? What features of a system would influence this decision?

6. When does spinning on a lock (busy waiting, as opposed to blocking on the lock, and being woken up when it's free) make sense in a multiprocessor environment?

7. Why is preemption an issue with spinlocks?

8. How does a read-before-test-and-set lock work and why does it improve scalability?

### Scheduling

9. What do the terms _I/O bound_ and _CPU bound_ mean when used to describe a process (or thread)?

10. What is the difference between cooperative and pre-emptive multitasking?

11. Consider the multilevel feedback queue scheduling algorithm used in traditional Unix systems. It is designed to favour IO bound over CPU bound processes. How is this achieved? How does it make sure that low priority, CPU bound background jobs do not suffer starvation?

12. Note: Unix uses low values to denote high priority and vice versa, 'high' and 'low' in the above text does not refer to the Unix priority value.

13. Why would a hypothetical OS always schedule a thread in the same address space over a thread in a different address space? Is this a good idea?

14. Why would a round robin scheduler NOT use a very short time slice to provide good responsive application behaviour?

### I/O

14. Describe _programmed I/O_ and _interrupt-driven I/O_ in the case of receiving input (e.g. from a serial port). Which technique normally leads to more efficient use of the CPU? Describe a scenario where the alternative technique is more efficient.

15. A device driver routine (e.g. `read_block()` from disk) is invoked by the file system code. The data for the filesystem is requested from the disk, but is not yet available. What do device drivers generally do in this scenario?

16. Describe how I/O buffering can be formulated as a _bounded-buffer producer-consumer problem_.

17. An example operating system runs its interrupt handlers on the kernel stack of the currently running application. What restriction does this place on the interrupt handler code? Why is the restriction required?

Page last modified: 2:40pm on Sunday, 19th of April, 2020
