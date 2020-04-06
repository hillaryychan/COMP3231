# Tutorial Week 8

## Questions

### Memory Management

**1.** Describe internal and external fragmentation.?

**2.** What are the problems with multiprogrammed systems with fixed-partitioning?

**3.** Assume a system protected with base-limit registers. What are the advantages and problems with such a protected system (compared to either a unprotected system or a paged VM system)?

**4.** A program is to run on a multiprogrammed machine. Describe at which points in time during program development to execution time where addresses within the program can be bound to the actual physical memory it uses for execution? What are the implication of using each of the three binding times?

**5.** Describe four algorithms for allocating regions of contiguous memory, and comment on their properties.

**6.** What is compaction? Why would it be used?

### Virtual Memory

**7.** What is swapping? What benefit might it provide? What is the main limitation of swapping?

**8.** What is Paging?

**9.** Why do all virtual memory system page sizes have to be a power of 2? Draw a picture.

**10.** What is a TLB? What is its function?

**11.** Describe a two-level page table and how it is used to translate a virtual address into a physical address.

**12.** Given a two-level page table (in physical memory), what is the average number of physical memory accesses per virtual memory access in the case where the TLB has a 100% miss ratio, and the case of a 95% hit ratio

**13.** What are the two broad categories of events causing page faults? What other event might cause page faults?

**14.** Translate the following virtual addresses to Physical Addresses using the TLB. The system is a R3000. Indicate if the page is mapped, and if so if its read-only or read/write.

The EntryHi register currently contains `0x00000200`.

The virtual addresses are `0x00028123`, `0x0008a7eb`, `0x0005cfff`,`0x0001c642`, `0x0005b888`, `0x00034000`.

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

**15.** Describe an inverted page table and how it is used to translate a virtual address into a physical address.

**16.** Describe a hashed page table and how it is used to translate a virtual address into a physical address.

**17.** Of the three page table types covered in lectures, which ones are most appropriate for large virtual address spaces that are sparsely populated (e.g. many single pages scattered through memory)?

**18.** What is temporal and spatial locality?

Page last modified: 8:04pm on Sunday, 5th of April, 2020
