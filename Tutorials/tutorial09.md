# Tutorial Week 9

## Questions

### Assignment 3 Exercises

By now you should be familiar enough with navigating the kernel source that you can find the answers to the questions below by yourself (Hint: use a source code browser or the grep utility). You may also find the [MIPS r3000 reference](http://www.cse.unsw.edu.au/~cs3231/doc/mips.php) useful.

**1.** What is the difference between the different MIPS address space segments? What is the use of each segment?

* `kseg2` - unused
* `kseg1` - not needed for ass3
* `kseg0` - the segment where you store the page table
* `kuseg`

**2.** What functions exist to help you manage the TLB? Describe their use. (Hint: look in `kern/arch/mips/include/tlb.h`)

``` C
/*
 * MIPS-specific TLB access functions.
 *
 *   tlb_random: write the TLB entry specified by ENTRYHI and ENTRYLO
 *        into a "random" TLB slot chosen by the processor.
 *
 *        IMPORTANT NOTE: never write more than one TLB entry with the
 *        same virtual page field.
 *
 *   tlb_write: same as tlb_random, but you choose the slot.
 *
 *   tlb_read: read a TLB entry out of the TLB into ENTRYHI and ENTRYLO.
 *        INDEX specifies which one to get.
 *
 *   tlb_probe: look for an entry matching the virtual page in ENTRYHI.
 *        Returns the index, or a negative number if no matching entry
 *        was found. ENTRYLO is not actually used, but must be set; 0
 *        should be passed.
 *
 *        IMPORTANT NOTE: An entry may be matching even if the valid bit
 *        is not set. To completely invalidate the TLB, load it with
 *        translations for addresses in one of the unmapped address
 *        ranges - these will never be matched.
 */

void tlb_random(uint32_t entryhi, uint32_t entrylo);
void tlb_write(uint32_t entryhi, uint32_t entrylo, uint32_t index);
void tlb_read(uint32_t *entryhi, uint32_t *entrylo, uint32_t index);
int tlb_probe(uint32_t entryhi, uint32_t entrylo);
```

**3.** What macros are used to convert from a physical address to a kernel virtual address?

``` C
/*
 * The first 512 megs of physical space can be addressed in both kseg0 and
 * kseg1. We use kseg0 for the kernel. This macro returns the kernel virtual
 * address of a given physical address within that range. (We assume we're
 * not using systems with more physical space than that anyway.)
 *
 * N.B. If you, say, call a function that returns a paddr or 0 on error,
 * check the paddr for being 0 *before* you use this macro. While paddr 0
 * is not legal for memory allocation or memory management (it holds
 * exception handler code) when converted to a vaddr it's *not* NULL, *is*
 * a valid address, and will make a *huge* mess if you scribble on it.
 */
#define PADDR_TO_KVADDR(paddr) paddr_to_kvaddr(paddr)
static inline vaddr_t
paddr_to_kvaddr(paddr_t paddr){
    return ((paddr) + MIPS_KSEG0);
}

#define KVADDR_TO_PADDR(vaddr) kvaddr_to_paddr(vaddr)
static inline paddr_t
kvaddr_to_paddr(vaddr_t vaddr){
    return ((vaddr) - MIPS_KSEG0);
}
```

**4.** What address should the initial user stack pointer be?

``` C
/*
 * The top of user space. (Actually, the address immediately above the
 * last valid user address.)
 */
#define USERSPACETOP  MIPS_KSEG0

/*
 * The starting value for the stack pointer at user level.  Because
 * the stack is subtract-then-store, this can start as the next
 * address after the stack area.
 *
 * We put the stack at the very top of user virtual memory because it
 * grows downwards.
 */
#define USERSTACK     USERSPACETOP
```

**5.** What are the `entryhi` and `entrylo` co-processor registers? Describe their contents.

The following macros are defined in OS/161 for setting and masking values written to or read from the `Entry*` registers

``` C
/*
 * TLB entry fields.
 *
 * Note that the MIPS has support for a 6-bit address space ID. In the
 * interests of simplicity, we don't use it. The fields related to it
 * (TLBLO_GLOBAL and TLBHI_PID) can be left always zero, as can the
 * bits that aren't assigned a meaning.
 *
 * The TLBLO_DIRTY bit is actually a write privilege bit - it is not
 * ever set by the processor. If you set it, writes are permitted. If
 * you don't set it, you'll get a "TLB Modify" exception when a write
 * is attempted.
 *
 * There is probably no reason in the course of CS161 to use TLBLO_NOCACHE.
 */

/* Fields in the high-order word */
#define TLBHI_VPAGE   0xfffff000
/*      TLBHI_PID     0x00000fc0 */

/* Fields in the low-order word */
#define TLBLO_PPAGE   0xfffff000
#define TLBLO_NOCACHE 0x00000800
#define TLBLO_DIRTY   0x00000400
#define TLBLO_VALID   0x00000200
/*      TLBLO_GLOBAL  0x00000100 */
```

**6.** What do the `as_*` functions do? Why do we need `as_prepare_load()` and `as_complete_load()`?

These functions are used by OS/161 internally to manage the address spaces of processes. These functions are dependent on the data structures (e.g. region list and page tables) sed to book-keep virtual memory, and thus you have to implement them. A summary of them is available in the header file.

`as_prepare_load()` is required as normally code segments are mapped read-only. `as_prepare_load()` enable writing to the code segment while the OS loads the code associated with the process. `as_complete_load()` then removes write permissions to the code segment to revert it back to read-only.

``` C
/*
 * Functions in addrspace.c:
 *
 *    as_create - create a new empty address space. You need to make
 *                sure this gets called in all the right places. You
 *                may find you want to change the argument list. May
 *                return NULL on out-of-memory error.
 *
 *    as_copy   - create a new address space that is an exact copy of
 *                an old one. Probably calls as_create to get a new
 *                empty address space and fill it in, but that's up to
 *                you.
 *
 *    as_activate - make curproc's address space the one currently
 *                "seen" by the processor.
 *
 *    as_deactivate - unload curproc's address space so it isn't
 *                currently "seen" by the processor. This is used to
 *                avoid potentially "seeing" it while it's being
 *                destroyed.
 *
 *    as_destroy - dispose of an address space. You may need to change
 *                the way this works if implementing user-level threads.
 *
 *    as_define_region - set up a region of memory within the address
 *                space.
 *
 *    as_prepare_load - this is called before actually loading from an
 *                executable into the address space.
 *
 *    as_complete_load - this is called when loading from an executable
 *                is complete.
 *
 *    as_define_stack - set up the stack region in the address space.
 *                (Normally called *after* as_complete_load().) Hands
 *                back the initial stack pointer for the new process.
 *
 * Note that when using dumbvm, addrspace.c is not used and these
 * functions are found in dumbvm.c.
 */
```

**7.** What does `vm_fault()` do? When is it called?

`vm_fault()` is called for any virtual memory related faults (i.e. TLB misses, writes to read-only pages, and accesses to invalid pages). It is responsible for resolving the fault by either returning an error, or loading an approrpiate TLB entry for the application to continue.

**8.** Assuming a 2-level hierarchical page table (4k pages), show for the following virtual addresses:

    1. The page number and offset;
    2. the translated address (after any page allocation); and
    3. the contents of the page table after the TLB miss.

The page table is initially empty, with no L2 pages. You may assume that the allocator returns frames in order, so that the first frame allocated is frame 1, then frames 2, 3, 4, etc.

* `00x100008`
* `00x101008`
* `00x1000f0`
* `00x41000`
* `00x41b00`
* `00x410000`

Start out by determining which bits of the virtual address are used for each component of the virtual address. The level 1 index is the 10 most significant bits (i.e. `vaddr >> 22`), the level 2 index is the next 10 bits from the top (i.e. `(vaddr >> 12) & 0x3ff`), the offset bits are the last 12 bits of the address (i.e. `vaddr & 0xfff`)

The components of all the virtual addresses are as follows:

| Virtual Address | Level 1 Index | Level 2 Index | Offset  |
| ---             | ---           | ---           | ---     |
| `00x00100008`   | `0x0`         | `0x100`       | `0x008` |
| `00x00101008`   | `0x0`         | `0x101`       | `0x008` |
| `00x001000f0`   | `0x0`         | `0x100`       | `0x0f0` |
| `00x00041000`   | `0x0`         | `0x41`        | `0x000` |
| `00x00041b00`   | `0x0`         | `0x41`        | `0xb00` |
| `00x00410000`   | `0x1`         | `0x10`        | `0x000` |

If we examine the level 1 indexes, we see that there are only two indexes used; i.e. there are only two pointers to level-2 page tables. Let's assume we allocate them in advance.

``` C
level1[0x0] = PADDR_TO_KVADDR(1 << 12)
level1[0x1] = PADDR_TO_KVADDR(2 << 12)
```

Note the frame numbers can be converted into addresses of the start of the frame by shifting. The level-2 page tables are walked by the OS (i.e. lie in `kseg0`), and so need to be converted into addresses in that range. If you get the types correct, you can then index off the pointers in the level-1 page tables.

``` C
(level1[0x0])[0x100] = (3 << 12) | TLB_VALID | TLB_DIRTY
(level1[0x0])[0x101] = (4 << 12) | TLB_VALID | TLB_DIRTY
(level1[0x0])[0x41] = (5 << 12) | TLB_VALID | TLB_DIRTY
(level1[0x1])[0x10] = (6 << 12) | TLB_VALID | TLB_DIRTY
```

Thus the translations for the above page table are:

| Virtual Address | Physical Address |
| ---             | ---              |
| `00x00100008`   | `0X3008`         |
| `00x00101008`   | `0X4008`         |
| `00x001000f0`   | `0X30f0`         |
| `00x00041000`   | `0x5000`         |
| `00x00041b00`   | `0x5b00`         |
| `00x00410000`   | `0x6000`         |

### C bitwise operations refresher

Given the following constants from the source code:

``` C
#define TLBHI_VPAGE   0xfffff000
#define TLBLO_PPAGE   0xfffff000
#define TLBLO_NOCACHE 0x00000800
#define TLBLO_DIRTY   0x00000400
#define TLBLO_VALID   0x00000200
```

And a 32-bit page table entry and 32-bit virtual address is defined as

``` C
uint32_t pte;
vaddr_t vaddr;
```

**9.** What C expressions would you use to set or reset the valid bit in a page table entry?

``` C
pte = pte | TLBLO_VALID /* set */
pte = pte & ~TLBLO_VALID /* reset */
```

`kprintf()` uses a lock to serialise access to the console. If the lock blocks, it context switches, whcih will call `as_activate` and flush the TLB, ejecting any newly inserted entry. Hence, the system ends up in an infinite loop replacing the TLB entry, and then ejecting it via `kprintf()`

**10.** What C expression would you use to test if the valid bit is set?

``` C
if (pte & TLBLO_VALID)
    / * do stuff */
```

**11.** How would you extract the 12-bit offset (bits 0 - 11) from the virtual address?

``` C
offset = vaddr & ~TLBHI_VPAGE
```

**12.** How would you convert the 10 most significant bits (22-31) of a virtual address into an index?

``` C
index = vaddr >> 22
```

**13.** How would you convert the next 10 most significant bits (12-21) into an index?

``` C
index = (vaddr << 10) >> 22
```

**14.** How would you round down a virtual address to the base of the page?

``` C
vaddr = vaddr & TLBHI_VPAGE
```

Page last modified: 12:19pm on Sunday, 12th of April, 2020
