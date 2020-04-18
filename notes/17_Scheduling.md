# Scheduling

On a multi-programmed system, we may have more than one _ready_ process  
On a batch system, we may have my jobs waiting to be run  
On a multi-user system, we may have many users concurrently using the system

The **scheduler** decides who to run next. The process of choosing is called **scehduling**.

Scheduling is not important in certain scenarios:

* if you have no choice - early systems usually use batching, where the scheduling algorithm is simple; run next on tape, or next on punch tape
* only one thing to run - simple PCs only ran a word processor etc., simple embedded systems (e.g. TV remote control, washing machine, etc.)

It is important in most realistic scenarios:

* multi-tasking/multi-user system -  e.g. an email daemon takes 2 seconds to process an email and the user clicks a button on the application
    * scenario 1: run daemon, then application -appears sluggish to user
    * scenario 2: run application, then daemon - application appears really responsive, small email delay is unnoticed

Scheduling decisions can have a dramatic effect on perceived performance of the system. It can also affect the correctness of a system with deadlines.

## Application Behaviour

Applications tend to have bursts of CPU usage alternating with periods of I/O wait. A process can be a 

![Application Behaviour](../imgs/17-6_app-behaviour.png)

* CPU-bound process - spends most of its time computing; time to completion is largely determined by received CPU time
* I/O-bound process - spends most of its time waiting for I/O to complete (small bursts of CPU to process I/O and request next I/O); time to completion is largely determined by I/O request time

We need a mix of CPU-bound and I/O-bound processes to keep both CPU and I/O systems busy. Processes can go from CPU-bound to I/O-bound (or vice-versa) in different phases of execution.

Choosing to run an I/O-bound process delays a CPU-bound process by very little.  
Choosing to run an CPU-bound process prior to a I/O-bound process delays the next I/O request significantly. There is no overlap of I/O waiting with computation resulting in the device (disk) being not as busy as possible.

Generally, **_favour I/O-bound processes over CPU-bound processes_**

## When is Scheduling Performed?

* a new process - run the parent or the child?
* a process exits - who runs next?
* a process waits for I/O - who runs next?
* a process blocks on a lock - who runs next? the lock holder?
* an I/O interrupt occurs - who do we resume, the interrupted process or the process that was waiting?
* on a timer interrupt?

Generally, a scheduling decision is required when a process (or thread) can no longer continue, or when an activity results in more than one ready process.

Recall: preemptive vs. non-preemptive scheduling

**Non-preemptive:** once a thread is in the _running_ state, it continues until it completes, blocks on I/O, or voluntarily yields the CPU. A single process can monopolise the entire system.  
**Preemptive:** the current thread can be interrupted by the OS and moved to _ready_ state. Usually after a timer interrupt and process has exceeded its maximum run time. It can also be a result of higher priority process that has become ready (after I/O interrupt). This ensures fairer service as a single thread can't monopolise the system

## Scheduling Algorithms

The choice of scheduling algorithm depends on the goal of the application (or the operating system). No one algorithm suits all environments.

We can roughly categorise scheduling algorithms as follows:

* **batch systems** - no users directly waiting, can optimise overall machine performance
* **interactive systems** - users directly waiting for their results, can optimise for users perceived performance
* **realtime systems** -  jobs have deadlines, must schedule such that all jobs (predictably) meet their deadlines

All algorithms have the following goals:

* **fairness** - give each process a _fair_ share of the CPU
* **policy enforcement** - what ever policy is chosen, the scheduler should ensure it is carried out
* **balance/efficiency** - try to keep all parts of the system busy

Interactive algorithms have additional goals:

* **minimise _response_ time** - response time is the time difference between issuing a command and getting the result; e.g. selecting a menu, and getting the result of that selection; response time is important to the user's perception of the performance of the system
* **provide _proportionality_** - proportionality is the user expectation that **_short jobs will have a short response time, and long jobs can have a long response time_**; generally, favour short jobs

Realtime algorithms has additional goals:

* **must meet _deadlines_** - each job/task has a deadline; a missed deadline can result in data loss or catastrophic failure (e.g. an aircraft control system missed deadline to apply breaks)
* **provide _predictablity_** - for some apps, an occasional missed deadline is okay (e.g. DVD decoder - predictable behaviour allows smooth DVD decoding with only rare skips)

### Interactive Scheduling

#### Round Robin Scheduling

Each process is given a **timeslice** to run in. When the time slice expires, the next process pre-empts the current process, and runs for its time slice, and so on. The pre-empted process is placed at the end of the queue. 

It can be implemented with a ready queue and a regular timer interrupt

![round robin](../imgs/17-19_round-robin.jpg)
