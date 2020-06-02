# COMP3231 - Operating Systems

1. [Overview](notes/01_Overview.md)
2. [Processes and Threads](notes/02_Processes-and-Threads.md)
    1. [Processes and Threads Implementation](notes/05_Processes-and-Threads-Impl.md) (MIPS recap, user and kernel level threads, context switching)
    2. [System Calls](notes/06_System-Calls.md) (syscall implementation, exception handling in MIPS and OS/161)
3. Concurrency and Deadlock
    1. [Concurrency and Synchronisation](notes/03_Concurrency-and-Synchronisation.md) (mutual exclusion, semaphores, monitors, OS/161 synchronisation primitives)
    2. [Deadlock](notes/04_Deadlock.md) (deadlock modelling and recovery, starvation)
4. File Systems
    1. [Computer Hardware Review](notes/07_Memory-Hierarchy.md) (memory hierarchy, caching)
    2. [File Management I](notes/08_File-Management.md) (history, file system abstraction)
    3. [File System Internals](notes/09_File-System-Internals.md) (file system structure, allocation strategies)
    4. [File Management II](notes/10_File-Management-ctd.md) (virtual file system, file system consistency)
    5. Case Study
        1. [ext2 File System](notes/11_Case-Study:ext2.md)
        2. [ext3 File System](notes/12_Case-Study:ext3.md)
5. Memory and Virtual Memory Management
    1. [Memory Management](notes/13_Memory-Management.md) (memory allocation, swapping)
    2. [Virtual Memory I](notes/14_Virtual-Memory.md) (sharing memory, page table structures, implementation issues, translation look-aside buffers)
    3. [Virtual Memory II](notes/15_Virtual-Memory-ctd.md) (MIPS TLB handling, demand paging/segmentation, locality, working sets, thrashing, virtual memory policies)
6. [Multiprocessors](notes/16_Multiprocessors.md) (bus based UMA,OS for multiprocessors, multiprocessor synchronisation)
7. [Scheduling](notes/17_Scheduling.md) (scheduling algorithms, multiprocessor scheduling)
8. [I/O](notes/18_IO.md) (device drivers, I/O interaction, interrupt handling, buffering)

## Tutorials

| Tutorial                                 | Topics |
| ---                                      | ---    |
| [Tutorial 2](Tutorials/tutorial02.md)    | Intro, Syscall Interface, Processes and Threads, Critical Sections|
| [Tutorial 3](Tutorials/tutorial03.md)    | Synchronisation, Concurrency, Deadlock                            |
| [Tutorial 4](Tutorials/tutorial04.md)    | MIPS, Threads, Kernel Entry and Exit                              |
| [Tutorial 5](Tutorials/tutorial05.md)    | Memory Hierarchy and Caching, Files and File System               |
| [Tutorial 6](Tutorials/tutorial06.md)    | Assignment 2                                                      |
| [Tutorial 7](Tutorials/tutorial07.md)    | Files and File System                                             |
| [Tutorial 8](Tutorials/tutorial08.md)    | Memory Management, Virtual Memory                                 |
| [Tutorial 9](Tutorials/tutorial09.md)    | Assignment 3                                                      |
| [Tutorial 10](Tutorials/tutorial10.md)   | Virtual Memory, Multi-processors, Scheduling, I/O                 |
