# Virtual Memory ctd.

## TLB Recap

TLB may or may not be under the operating system's control

* Hardware-loaded TLB - on a miss, hardware performs a page table lookup and reloads the TLB; e.g. Pentium
* Software-loaded TLB - on a miss, hardware generates a TLB miss exception, and exception handler reloads the TLB; e.g. MIPS

Even if the TLB is filled by software, it is still a hardware-based translator

## Amdahl's Law

**Amdahl's law** states that overall performance improvement is limited by the fraction of time an enhancement can be used.

![law of diminishing returns](../imgs/15-12_amdahls-law.png)

So the goal is to **make common cases FAST**

## MIPS R3000 TLB Handling

TLB refill is handled by software (an exception handler)

TLB refill exceptions are accessing `kuseg` are expected to be frequent so the CPU is optimised for handling `kuseg` TLB refills by having a special exception handler just for TLB refills

![TLB exception](../imgs/15-7_tlb-exception.jpg)

The special exception vector is optimised for TLB refills since it:

* does not need to check the exception type (since it has its own handler)
* does not need to save any registers; it uses a specialised assembly routine that only uses `k0` and `k1`
* does not check if the page table entry exists; assumes a virtual linear array (see extended OS if interested)

With careful data structure choice, the exception handler can be very fast

An example routine:

``` assembly
mfc0  k1, C0_CONTEXT
mfc0  k0, C0_EPC # mfc0 delay slot

lw    k1, 0(k0) # may double fault (k0 = orig EPC)
nop
mtc0  k1, Co_ENTRYLO
nop
tlbwr
jr    k0
rfe
```

MIPS virtual memory related exceptions:

* TLB refill - handled via special exception vector; needs to be very fast
* others - handled by the general exception vector; can be slower as they are mostly caused by an error, or non-resident page
    * TLB Mod- TLB modify exception; attempt to write to a read-only page
    * TLB Load- attempt to load from a page with an invalid translation
    * TLB Store- attempt to store to a page with an invalid translation

We never optimise for errors, and page-loads from disk. They dominate the fault resolution cost (see [Amdahl's law](#Amdahl's-Law))

### c0 Registers

* `c0_EPC` - stores the address of where to restart after the exception
* `c0_status` - stores kernel/user mode bits; interrupt control
* `c0_cause` - stores what caused the exception
* `c0_badvaddr` - stores the address of the fault

If `c0_EPC` and `c0_badvaddr` are the **same** then the page for the next instruction was not loaded  
If `c0_EPC` and `c0_badvaddr` are the **same** then it is likely to be an load/store failure

`c0` Registers are used to read and write individual TLB entries. Each TLB entry contains:

* `EntryHi` to match the page# and ASID
* `EntryLo` which contains the frame# and protection

![TLB and EntryHi, EntryLo](../imgs/15-15_entryhilo-tlb.jpg)

![c0 registers](../imgs/15-16_c0-registers.png)

MIPS' `c0` Registers:

* `N` for not cachable
* `D` for dirty (i.e write protect)
* `G` for gloabl (ignore ASID in lookup)
* `V` for valid bit

There are 64 TLB entries, and they are accessed via software through coprocessor 0 registers (`EntryHi`, `EntryLo`)

The `c0_Index` register is used as an index to TLb entries. Single TLB entries are manipulated/viewed through `EntryHi` and `EntryLo` registers. The index register specifies which TLb entry to change/view

Special TLB management instructions:

* `TLBR` for TLB read - `EntryHi` and `EntryLo` are loaded from the entry pointer by the index register
* `TLBP` for TLB probe - sets `EntryHi` to the entry you wish to match; index register is loaded with the index to the matching entry
* `TLBWR` - write `EntryHi` and `EntryLo` to a pseudo-random location in the TLB
* `TLBWI` - write `EntryHi` and `EntryLo` to the location in the TLB pointed to by the index register

Example of coprocessor 0 registers on a refill exception:

``` tct
c0.EPC           = PC
c0.cause.ExcCode = TLBL; if read fault
c0.cause.ExcCode = TLBS; if write fault
c0.BadVaddr      = faulting address
c0.EntryHi.VPN   = page number of faulting address
c0.status        = kernel mode, interrupts disabled
c0.PC            = 0x8000 000
```

### Outline of TLB Miss Handling

Software does: 

* lookup of page table entry corresponding to faulting address
* If found:
    * load `c0_EntryLo` with translation
    * load TLB with `TLBWR`instruction
    * return from exception
* else, page fault

The TLB entry (i.e. `c0_EntryLo`) can be (theoretically) created on the fly or stored completely in the right format in the page table (the latter being more efficient)

**OS/161 Refill handling**:  
After switching to the kernel stack, it simply calls the common exception handler

* stacks all registes
* can (and does) call `C` code
* unoptimised
* goal is ease of kernel programming, not efficiency

It does not have a page table. It uses the 64 TLB entries and then panics when it runs out. It only supports 256K user-level address space

## Demand Paging/Segmentation

With virtual memory, only parts of the program image need to be resident in memory for execution. We can transfer presently unused page/segments to disk and then reload non-resident pages/segments on **demand**

A **reload** is triggered by a page or segment fault. The faulting process is blocked and another is scheduled. When the page/segment is resident, the faulting process is restarted.  
This may require freeing up memory first, where we replace a current resident page/segment (the replacement "victim"). If the victim is unmodified ("clean") we can simply discard it. This is the reason for maintaining a "dirty" bit in the page table.

Why does demand paging/segmentation work?  
We know a program executes at full speed only when accessing the resident set. TLB misses and page/segment fault introduce delays of several microseconds.  
**Answer**:

* less physical memory is required per process and we can fit more processes in memory. This results in an improved chance of finding a runnable one.
* principle of locality

### Principle of Locality

It was observed that programs tend to reuse data and instructions they have recently used; 90/10 rule - _"A program spends 90% of its time in 10% of its code"_

We can exploit this _locality of refences_

An implication of locality is that we can reasonably predict what instructions and data a program will use in the near future **based on its accesses in the recent past**
