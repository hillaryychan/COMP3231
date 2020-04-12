# Tutorial Week 8

## Questions

### Memory Management

**1.** Describe internal and external fragmentation?

External fragmentation: total memory space exists to satisfy a request, but it not contiguous  
Internal fragmentation: allocated memory may be slightly larger than request memory; this size difference is memory internal to the partition but is not being used

**2.** What are the problems with multiprogrammed systems with fixed-partitioning?

* internal fragmentation
* inability to run processes greater in size than a partition, but smaller than memory

**3.** Assume a system protected with base-limit registers. What are the advantages and problems with such a protected system (compared to either a unprotected system or a paged VM system)?

Disadvantages:

* partitions **must** be contiguous - external fragmentation
* entire process must be in memory
* cannot practically share memory with other processes

Advantages:

* applications are protected from each other
* memory for applications can be allocated dynamically and hardware translates the application (logical) addresses to allocated addresses
* multiple concurrent executions of the same application possible
* compaction is also possible

**4.** A program is to run on a multiprogrammed machine. Describe at which points in time during program development to execution time where addresses within the program can be bound to the actual physical memory it uses for execution? What are the implication of using each of the three binding times?

* **compile/link time binding**  
The executable itself contains the actual physical addresses it will use during execution. It can only run at one location, and only a single copy can run at a time, unless the executable is recompiled/relinked to a new location in physical memory prior to each execution
* **load time binding**  
Addresses within the executable are annotated such that when the program is loaded into physical memory, the loader can bind the addresses to the correct location within physical memory as the program is loaded (copied) into physical memory. This process slows loading (increases start up latency), and increases executable file size (to hold annotations, minor point)
* **run-time binding**  
The compiler/linker generated addresses are bound to a logical address space (an abstract memory layout). The program executes using the logical addresses independent of where it is loaded into physical memory. At run-time, the logical addresses are translated to the appropriate physical addresses by specialised hardware on each memory reference.  
Run-time binding is the **most flexible** (depending of the translation hardware available, e.g. page VM MMU), but it incurs the cost of translation on every memory reference, which can be significant

**5.** Describe four algorithms for allocating regions of contiguous memory, and comment on their properties.

1. First-fit - scan memory from start for first fit.  
Tends to skip over potentially many regions at the start of the list
2. Next-fit - scan memory region list from point of last allocation to find next fit  
Breaks up large block at end of memory with any reduction in searching
3. Best-Fit - pick the closest free region in the entire list  
Tends to leave small unusable regions, and slow to due to requirement to search entire list
4. Worst-Fit -  pick the worst fit in the entire list  
Slower as it searches the entire list, fragmentation is still an issue

**6.** What is compaction? Why would it be used?

Compaction is when you move all allocated regions of memory next to each other (e.g. to the bottom of memory) to free up larger contiguous free regions.

### Virtual Memory

**7.** What is swapping? What benefit might it provide? What is the main limitation of swapping?

Swapping is where a process is brought into main memory in its entirety, runs for a while, and is then put completely back on disk.

Swapping allows the OS to run more programs than what would fit in memory if all processes remained resident in memory

Swapping is slow as it has to copy the entire program's in-memory image out to disk and back

**8.** What is Paging?

Paging is where memory is divided into equal-sized chunks (frames) and the programs address space (virtual address space) is also divided up into matching sized chunks (pages). Memory is transferred from disk into units of pages

**9.** Why do all virtual memory system page sizes have to be a power of 2? Draw a picture.

The lower bits of a virtual address is not translated and passed through the MMU to form a physical address

**10.** What is a TLB? What is its function?

TLB is the translation look-aside buffer. It is an associative **cache** of page table entries to speed up the translation of virtual addresses to physical addresses

**11.** Describe a two-level page table and how it is used to translate a virtual address into a physical address.

**12.** Given a two-level page table (in physical memory), what is the average number of physical memory accesses per virtual memory access in the case where the TLB has a 100% miss ratio, and the case of a 95% hit ratio

100% miss ratio: `3 accesses (2 for two-level page table, 1 for physical address access)`
95% hit ratio:   `0.95*1 + (1-0.95)*3 = 1.1 accesses`

**13.** What are the two broad categories of events causing page faults? What other event might cause page faults?

**Illegal memory references** and **access to non-resident pages**

Page faults may be used to set reference and dirty bits on architectures that do not support them on hardware

**14.** Translate the following virtual addresses to Physical Addresses using the TLB. The system is a R3000. Indicate if the page is mapped, and if so if its read-only or read/write.

The EntryHi register currently contains `0x00000200`.

The virtual addresses are `0x00028123`, `0x0008a7eb`, `0x0005cfff`, `0x0001c642`, `0x0005b888`, `0x00034000`.

``` txt
         TLB
EntryHi     EntryLo
0x00028200  0x0063f400
0x00034200  0x001fc600
0x0005b200  0x002af200
0x0008a100  0x00145600
0x0005c100  0x006a8700
0x0001c200  0x00a97600
```

``` txt
|               Results                             |
|---------------------------------------------------|
| VAddr      | PhysAddr   | Access                  |
| 0x00028123 |            | Invalid                 |
| 0x0008a7eb |            | Invalid (ASID mismatch) |
| 0x0005cfff | 0x006a8fff | Global bit, R/W         |
| 0x0001c642 | 0x00a97642 | R/W                     |
| 0x0005b888 | 0x002af888 | Read-only               |
| 0x00034000 | 0x001fc000 | R/W                     |
```

**15.** Describe an inverted page table and how it is used to translate a virtual address into a physical address.

**16.** Describe a hashed page table and how it is used to translate a virtual address into a physical address.

**17.** Of the three page table types covered in lectures, which ones are most appropriate for large virtual address spaces that are sparsely populated (e.g. many single pages scattered through memory)?

The two-level page table suffers from internal fragmentation of page table nodes themselves.  
The IPT and HPT are best as they are searched via a hash and are not based on the structure of the virtual address space.

**18.** What is temporal and spatial locality?

Temporal locality - recently accesed items are likely to be accessed in the near future  
Spatial locality - items whose addresses are near each other tend to be referenced close together in time

Page last modified: 8:04pm on Sunday, 5th of April, 2020

