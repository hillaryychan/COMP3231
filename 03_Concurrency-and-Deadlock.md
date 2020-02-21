# Concurrency and Deadlock

Consider the following example:

`count` is a global variable that is shared between two threads implementing the functions below. After `increment()` and `decrement()` complete, what is the  value of `count`?

``` C
void increment() {
    int t;
    t =  count;
    t = t + 1;
    count = t;
}


void decrement() {
    int t;
    t =  count;
    t = t - 1;
    count = t;
}
```

We don't know. We can't make assumptions about the execution of threads relative to each other. Both threads have access to `count` and when it is accessed in an uncoordinated manner, `count` will not store its intended value.

This is known as a **race condition**.

## Where does concurrency occur?

Concurrency can occur in a multiple processes with a single thread or in a single process with multiple threads.

![Where is concurrency](imgs/3-5_where-is-concurrency.png)

There is in-kernel concurrency even for single threaded processes because there are global variables and resources in the OS that can be manipulated by processes through system calls.

![in-kernel concurrency](imgs/3-6_in-kernel-concurrency.png)

We can control access to shared resources by controlling access to the code that accesses the resources. Uncoordinated entry to the critical region results in a race condition, which in turn leads to incorrect behaviour, deadlock, lost work etc.

**Critical regions** or **critical sections** are regions of code that access a shared resource and their correctness relies on the shared resource not being concurrently modified by another thread/process/entity.

![accessing critical regions](imgs/3-9_accessing-critical-regions.png)

We've highlighted the critical regions in a basic linked list implementation below.

![example critical region](imgs/3-12_example-critical-regions.png)

## Critical Regions Solutions

Our conditions required from solutions to the critical region problem:

1. **mutual exclusions** - no two processes can simultaneously access the critical region
2. **no assumptions** can be made about speeds or numbers of CPUS
3. **progress** - no process running outside its critical region may block another process
4. **bounded** - no process waits forever to enter its critical region

### Lock ~~Solution~~

A problematic solution would be using a lock variable.  
If `lock == 1`, somebody is in the critical section and we must wait  
If `lock == 0`, nobody is in the critical section and we are free to enter

Looking at the execution sequence on the right-hand side, it is still possible for two threads to enter critical regions as the same time

![execution problem](imgs/3-16_lock-solution-problem.jpg)

### Mutual Exclusion Solutions

#### by Taking Turns

Another proposed solution to the critical region problem is _taking turns_, where access is controlled by a `turn` variable.

``` C
// process 0
while (TRUE) {
    while (turn != 0);
    critical_region();
    turn = 1;
    noncritical_region();
}

// process 1
while (TRUE){
    while (turn != 1) {/*loop*/}
    critical_region();
    turn = 0;
    noncritical_region();
}
```

This works due to _strict alternation_ of each process taking turns.  
It does have the following **disadvantages**:

- busy waiting
- process must wait its turn while the other process is doing something else. If there are many processes, all of them must wait their turn. This does not guarantee progress and overall is a **poor solutions** when processes require the critical section for differing durations

#### by Disabling Interrupts

The solution goes like this:  
Before entering a critical region, we disable interrupts.  
After leaving the critical region, we enable interrupts.

This is a nice simple solution that works only on uniprocessor machines.  
It has the following **disadvantages**;

- only available in the kernel; it is not safe to allow applications to disable interrupts
- blocks everyone else, even with no contention; the OS will be unresponsive to interrupts from other devices which is unsafe for extended periods of time
- does not work on a multiprocessor; interrupts are per CPU

### with Test-and-Set

There is hardware support for mutual exclusion. A **test and set** instruction can be used to implement lock variables correctly (i.e. the lock solution we suggested earlier). It works like this:

``` C
if (lock == 0) {
    set lock to 1
    return the result 0 // we acquire the lock
}

if (lock == 1) {
    return 1            // another thread/process has the lock
}
```

Hardware guarantees that the instruction executes **atomically** (that is as an indivisible unit).

```
enter region:
    TSL REGISTER,LOCK   | copy lock to register and set lock to 1
    CMP REGISTER,#0     | was lock zero?
    JNE enter_region    | if it was non-zero, lock was set, so loop
    RET                 | return to caller; critical region entered

leave_region:
    MOVE LOCK,#0        | store a 0 in lock
    RET                 | return to caller
```

This solution is simple (easy to show it's correct) and available at a user-level to any number of processors and to implement any number of lock variables.  
However because it loops to check whether it can access the critical region, it **busy waits** or **spin locks**. This consumes CPU and starvation is possible when a process leaves its critical region and more than one process is waiting.

A possible solution to the busy-wait problem is by sleeping and waking up the thread/process. The idea is that when the process is waiting for an event, it calls sleep to block instead of busy waiting. When the event happens, the even generator (another process) calls wakeup to unblock the sleeping process/thread. Waking a ready/running process has no effect.

## The Producer-Consumer Problem

In the **producer-consumer problem** or **bounder buffer problem** we have:

- a producer producing data items and storing them in a buffer
- a consumer taking out the items from the buffer and consuming them

![producer consumer](imgs/3-25_producer-consumer.png)

**The issue**: we must keep an accurate count of items in the buffer

The producer should:

- sleep when the buffer is full
- wakeup when there is an empty space in the buffer; the consumer can call wakeup when it consumes the first entry of the full buffer

The consumer should:
 - sleep when the buffer is empty
 - wakeup when there are items available; the producer can call wakeup when it adds the first item to the buffer

