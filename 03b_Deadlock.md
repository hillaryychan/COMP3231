# Deadlock

Examples of computer resources include printers, tape drivers and tables in a database. Processes need access to resources in a reasonable order.  
Preemptable resources can be taken away from a process with no ill effects.  
Non-preemptable resources will cause the process to fail if taken away.

Suppose a process holds resource A and requests resource B. At the same time another process holds B and requests A. Both processes are blocked and remain so. This is known as **deadlock**.

**Deadlock** occurs when processes are granted exclusive access to devices, locks, tables etc. We refer to these entities as **resources**.

The sequence of events required to use a resource:

1. request the resource
2. use the resource
3. release the resource

If the request is denied, the process must wait. As a result the requesting process may be blocked or may fail with an error code.

We have two segments of code:

```
// segment 1
semaphore res_1, res_2;
void proc_A() {                     void prov_B() {
    down(&res_1);                       down(&res_1);
    down(&res_2);                       down(&res_2);
    use_both_res();                     use_both_res();
    up(&res_2);                         up(&res_2);
    up(&res_1);                         up(&res_1);
}                                   }

// segment 2
semaphore res_1, res_2;
void proc_A() {                     void prov_B() {
    down(&res_1);                       down(&res_2);
    down(&res_2);                       down(&res_1);
    use_both_res();                     use_both_res();
    up(&res_2);                         up(&res_1);
    up(&res_1);                         up(&res_2);
}                                   }
```

Segment 2 is deadlock prone because the resources are accessed in different orders;  
Say `proc_A` gains access to `res_1`, then `proc_B` runs and gains access to `res_2`. When `proc_A` tries to access `res_2` it is blocked. The same applies to `proc_B` for `res_1`. Now we are in deadlock.
