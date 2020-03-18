# Concurrency and Synchronisation

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

![Where is concurrency](../imgs/3-5_thread-model.png)

There is in-kernel concurrency even for single threaded processes because there are global variables and resources in the OS that can be manipulated by processes through system calls.

![in-kernel concurrency](../imgs/3-6_in-kernel-concurrency.png)

We can control access to shared resources by controlling access to the code that accesses the resources. Uncoordinated entry to the critical region results in a race condition, which in turn leads to incorrect behaviour, deadlock, lost work etc.

**Critical regions** or **critical sections** are regions of code that access a shared resource and their correctness relies on the shared resource not being concurrently modified by another thread/process/entity.

![accessing critical regions](../imgs/3-9_accessing-critical-regions.png)

We've highlighted the critical regions in a basic linked list implementation below.

![example critical region](../imgs/3-12_example-critical-regions.png)

## Critical Regions Solutions

Our conditions required from solutions to the critical region problem:

1. **mutual exclusions** or **lock** - no two processes can simultaneously access the critical region
2. **no assumptions** can be made about speeds or numbers of CPUS
3. **progress** - no process running outside its critical region may block another process
4. **bounded** - no process waits forever to enter its critical region

### Lock ~~Solution~~

A problematic solution would be using a lock variable.  
If `lock == 1`, somebody is in the critical section and we must wait  
If `lock == 0`, nobody is in the critical section and we are free to enter

Looking at the execution sequence on the right-hand side, it is still possible for two threads to enter critical regions as the same time

![execution problem](../imgs/3-16_lock-solution-problem.jpg)

### Mutual Exclusion Solutions by Taking Turns

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

* busy waiting
* process must wait its turn while the other process is doing something else. If there are many processes, all of them must wait their turn. This does not guarantee progress and overall is a **poor solutions** when processes require the critical section for differing durations

### Mutual Exclusion Solutions by Disabling Interrupts

The solution goes like this:  
Before entering a critical region, we disable interrupts.  
After leaving the critical region, we enable interrupts.

This is a nice simple solution that works only on uniprocessor machines.  
It has the following **disadvantages**;

* only available in the kernel; it is not safe to allow applications to disable interrupts
* blocks everyone else, even with no contention; the OS will be unresponsive to interrupts from other devices which is unsafe for extended periods of time
* does not work on a multiprocessor; interrupts are per CPU

### Mutual Exclusion Solutions with Test-and-Set

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

``` assembly
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

* a producer producing data items and storing them in a buffer
* a consumer taking out the items from the buffer and consuming them

![producer consumer](../imgs/3-25_producer-consumer.png)

**The issue**: we must keep an accurate count of items in the buffer

The producer should:

* sleep when the buffer is full
* wakeup when there is an empty space in the buffer; the consumer can call wakeup when it consumes the first entry of the full buffer

The consumer should:

* sleep when the buffer is empty
* wakeup when there are items available; the producer can call wakeup when it adds the first item to the buffer

Our first proposed solution uses a locking primitive based on a test-and-set to protect the current access.

```C
int count = 0;
#define N 4     // bufsize

prod() {                                con(){
    while (TRUE) {                          while (TRUE) {
        item = produce();                       if (count == 0)
        if (count == N)                             sleep();
            sleep();                            acquire_lock();
        acquire_lock();                         remove_item();
        insert_item();                          count--;
        count++;                                release_lock();
        release_lock();                         if (count == N-1)
        if (count == 1)                             wakeup(prod);
            wakeup(con);                    }
    }                                   }
}
```

However this has a problematic execution sequence, where `con` may detect `count` is 0, but is then interrupted. `prod` executes all of it's steps, increasing `count` to 1, and calling `wakeup`. Since `con` is still awake nothing happens. We then resume `con` which goes to sleep. :(

Our problem here is that the test for some condition and actually going to sleep is not atomic. If we try the following:

``` C
// producer             // consumer
if (count == N)         if (count == 0)
    sleep();                sleep();
release_lock();         release_lock();

// The lock is held while asleep, so the count will never change
```

## Semaphores

Dijkstra (1965) introduced two primitives that are more powerful than a simple sleep and wakeup;

* `P()`, `wait` or `down` to test
* `V()`, `signal`, `up` to increment

If a resource is not available the corresponding semaphore blocks any process `wait`ing for the resource. Blocked processes are put into a process queue maintained by the semaphore, avoiding busy waits. When a process releases a resource, it `signal`s this by means of the semaphore. Signalling resumes a blocked process if there is any.  
Wait and signal operations cannot be interrupted.  
Complex co-ordination can be implemented by multiple semaphores.

Semaphores can be used to solve a variety of concurrency problems, however programming with them can be error-prone. E.g. you must signal for every wait if you want mutex. Too many or too few signals, or signals and waits in the wrong order can have catastrophic results.

### Semaphore Implementation

``` C
// A semaphore is defined as a record:
typdef struct {
    int count;          // #times a thread can call wait before it is blocked
    struct process *L;  // a linked list of processes blocked
                        // and waiitng for this semaphore
} semaphore;

// Semaphore operations:
wait(S) {
    S.count--;
    if (S.count < 0) {
        add process to S.L;
        sleep();
    }
}

signal(S) {
    S.count++;
    if (S.count <= 0) {
        remove process P from S.L;
        wakeup(P);
    }
}
```

Each primitive is atomic because interrupts are disabled for each.

### Semaphore Uses

#### Synchronisation Tool

Semaphores can be used a synchronisation tools.  
Say we have two threads with functions A and B, where we always want be to execute after A. Using a semaphore count intialised to 0, all we need is this:

``` C
A() {
    ...
    signal(flag);
}

B() {
    ...
    wait(flag);
}
```

When `A` runs first, `signal` is called but does nothing, then `B` runs.  
When `B` runs first, `wait` is called and `B` becomes blocked. Then `A` runs, `signal`s `B` to wake up and run.

#### Implementation of a Mutex

``` C
semaphore mutex;
mutex.count = 1     // init mutex

wait(mutex);        // enter critical region

doStuff();

signal(mutex);      // exit critical region
```

Notice how the initial count determines how many waits can progress before blocking and requiring a signal.

#### Solving the Producer-Consumer Problem

```C
#define N 4     // bufsize

semaphore mutex = 1;    // lock for critical regions
semaphore empty = N;    // #empty slots
semaphore full = 0;     // #full slots

prod() {                                con(){
    while (TRUE) {                          while (TRUE) {
        item = produce();                       wait(full);
        wait(empty);                            wait(mutex;
        wait(mutex);                            remove_item();
        insert_item();                          signal(mutex);
        signal(mutex);                          signal(empty);
        signal(full);                       }
    }                                   }
}
```

## Monitors

To ease concurrent programming, Hoare (1974) proposed **monitors**. Monitors are a higher level synchronisation primitive and a programming language construct.

The idea is that a set of procedures, variables and data types are group in a special kind of module called a **monitor**. These variables and data types are only accessed from within the monitor. Only one process/thread can be in the monitor at any one time.  
Mutual exclusion is implemented by the compiler, which should be less error prone.

When a thread calls a monitor procedure, which already has a thread inside, it queues and it sleeps until the current thread exits the monitor.

![Monitor](../imgs/3-44_monitors.png)

Examples of monitors:

``` C
monitor example {
    int count;;
    procedure inc() {
        count++;
    }
    procedure dec() {
        count--;
    }
}
```

### Condition Variables

**Condition variables** are a mechanism to block when waiting for an event in additon to ensuring mutual exclusion. e.g. for the producer-consumer problem when the buffer is empty or full.

To allow a process to wait within the monitor, a **condition variable** must be declared as: `condition x,y;`.  
Condition variables can only be used with the operations `wait` and `signal`.  
`x.wait()` means that the process invoking this operation is suspended until another process invokes. Another thread can enter the monitory while the original is suspended.  
`x.signal()` resumes exactly **one** suspended process. If no process is suspended, then the `signal` operation has no effect.

![Conditon variables in monitors](../imgs/3-49_condition-var.png)

Solution to producer-consumer problem using monitors and condition variables:

``` C
monitor ProducerConsumer
    condition full, empty;
    integer count;

    procedure insert(item: integer);
    begin
        if count = N then wait(full);
        inser_item(item)
        count := count + 1;
        if count = 1 then signal(empty);
    end

    procedure remove: integer;
    begin
        if count = 0 then wait(empty);
        inser_item(item)
        count := count - 1;
        if count = N-1 then signal(full);
    end;
end monitor;

procedure producer;
begin
    while true do
    begin
        item = produce_item;
        ProducerConsumer.insert(item);
    end;
end;

procedure consumer;
begin
    while true do
    begin
        item = ProducerConsumer.remove();
        consume_item(item);
    end;
end;
```

## OS/161 Provided Synchronisation Primitives

### Lock Functions

``` C
// Functions to create and destroy locks:
// create a lock by giving it a name (for debugging purposes)
struct lock *lock_create(const char *name);
void lock_destroy(struct lock*);

// Functions to acquire and release locks:
void lock_acquire(struct lock *);
void lock_release(struct lock *);
```

Example use of locks:

``` C
int count;
struct lock *count_lock;

int main() {
    count = 0;
    count_lock = lock_create("countlock");
    if (count_lock == NULL)
        panic("I'm dead");
    stuff();
}

procedure inc() {
    lock_acquire(count_lock);
    count = count + 1;
    lock_release(count_lock);
}

procedure dec() {
    lock_acquire(count_lock);
    count = count - 1;
    lock_release(count_lock);
}
```

If OS/161 cannot enter the critical section, it will put the thread to sleep until lock release is called, where it will allow one of the sleeping threads to enter the critical section

### Semaphore Functions

``` C
// create a semaphore by giving it a name and an initial value
struct semaphore *sem_create(const char *name, int initial_count);
void sem_destroy(struct semaphore(struct semaphore *);

void P(struct semaphore *); // wait
void V(struct semaphore *); // signal
```

Example use of semaphores:

``` C
int count;
struct semaphore *count_mutex;

int main() {
    count = 0;
    count_mutex = sem_create("count", 1);
    if (count_mutex == NULL)
        painc("I'm dead");
    stuff();
}

procedure inc() {
    P(count_mutex);
    count = count + 1;
    V(count_mutex);
}

procedure dec() {
    P(count_mutex);
    count = count - 1;
    V(count_mutex);
}
```

### Condition Variable Functions

You can build a monitor-like design construct by using locks and condition variables

``` C
struct cv *cv_create(const char*name);
void cv_destroy(struct cv *);

void cv_wait(struct cv *cv, struct lock *lock);
// Release the lock and blocks (i.e. puts current thread to sleep)
// Upon resumption, it re-acquires the lock.
// Note: we must recheck the condition we slept on

void cv_signal(struct cv *cv, struct lock *lock);
// Wakes one thread, but does not release the lock
void cv_broadcast(struct cv *cv, struct lock *lock);
// Wakes up all threads, but does not release the lock
// The first "waiter" scheduled after signaller releases the lock will acquire the lock

// Notes: all three variants must hold the lock passed in
```

Example of condition variables with bounded buffers:

``` C
// non solution
lock_acquire(c_lock);
if (count == 0)
    sleep();
remove_item();
count--;
lock_release(c_lock);

// solution
lock_acquire(c_lock);
while (count == 0)          // use while in case other threads have woken up
    cv_wait(c_cv, c_lock);  // before you and made the count equal to 0
remove_item();
count--;
lock_release(c_lock);
```

Solving the producer consumer problem using condition variables and locks:

``` C
int count = 0;
#define N 4     // bufsize

prod() {                            con() {
    while (TRUE) {                      while (TRUE) {
        item = produce();                   lock_acquire(1);
        lock_acquire(1);                    while (count == 0)
        while (count == N)                      cv_wait(full, 1);
            cv_wait(full, 1);               item = remove_item();
        insert_item(item);                  count--;
        count++;                            cv_signal(full,1);
        cv_signal(empty,1);                 lock_release(1);
        lock_release(1);                    consume(item);
    }                                   }
}                                   }
```

## More Concurrency and Synchronisation Problems

### Dining Philosophers

Scenario: philosophers eat and think. When they eat they need 2 forks, but they can only pick up one fork at a time. If all philosophers pick up the fork on their right-hand side, they will be caught in a deadlock and starve to death.

How do we prevent the deadlock?

One solution could be to make it so that only one philosopher on the table can be eating at any time, but this is inefficient if there are lots of philosophers.

Another solution is using semaphores:

``` C
#define N           5           // # of philosophers
#define LEFT        (i+N-1)%N   // # of i's left neighbour
#define RIGHT       (i+1)%N     // # of i's right neighbour
#define THINKING    0           // philosopher is thinking
#define HUNGRY      1           // philosopher is hungry
#define EATING      2           // philosopher is eating

typdef int semaphore;           // semaphores are a special kind of int
int state[N];                   // array to keep track of everyone's state
semaphore mutex = 1;            // mutual exclusion for critical regions
semaphore s[N]                  // one semaphore per philosopher


void philosopher(int i) {       // i is the philosopher number from 0 to N-1
    while (TRUE) {              // repeat forever
        think();                // philospher is thinking
        take_forks(i);          // acquire two forks and block
        eat();                  // yum-yum, spaghetti
        put_forks(i);           // put both forks back on the table
    }
}

void take_forks(int i) {        // i is the philosopher number from 0 to N-1
    down(&mutex);               // enter critical region
    state[i] = HUNGRY;          // record fact that philosopher is hungry
    test(i);                    // try to acquire two forks
    up(&mutex);                 // exit critical regioin
    down(&s[i]);                // block if forks not acquired
}

void put_forks(int i) {         // i is the philosopher number from 0 to N-1
    down(&mutex);               // enter critical region
    state[i] = THINKING;        // philosopher has finished eating
    test(LEFT);                 // see if left neighbour can eat now
    test(RIGHT);                // see if right neighbour can eat now
    up(&mutex);                 // exit critical region
}

void test(int i) {              // i is the philosopher number from 0 to N-1
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        up(&s[i]);
    }
}
```

### The Readers and Writers Problem

This models access to a database such as a airline reservation system. It can have more than one concurrent reader, but writers must have exclusive access.

Solution:

``` C
typedef int semaphore;
semaphore mutex = 1;                // controls access to 'rc'
semaphore db = 1;                   // controls access to the database
int rc = 0;                         // # of processes reading or waiting to

void reader(void){
    while (TRUE) {                  // repeat forever
        down(&mutex);               // get exclusive access to rc
        rc++;                       // one reader more now
        if (rc == 1) down(&db);     // if this is the first reader...
        up(&mutex);                 // release exclusive access to rc
        rc--;                       // one reader fewer now
        if (rc == 0) up(&db);       // if was the last reader
        up(&mutex);                 // release exclusions access to rc
        use_data_read();            // non-critical region
    }
}

void writer(void){
    while (TRUE) {                  // repeat forever
        think_up_data();            // non-critical region
        down(&db);                  // get exclusive access
        write_data_base();          // update the data
        up(&db);                    // release exclusive acces
    }
}
```
