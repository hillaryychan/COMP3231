# Tutorial Week 6

## Questions

### Code walk-through

Bring your answers to the code walk-through questions to your tutorial.

### General system calls

#### `kern/arch/mips/`: traps and syscalls

Exceptions are the key to operating systems; they are the mechanism that enables the OS to regain control of execution and therefore do its job. You can think of exceptions as the interface between the processor and the operating system. When the OS boots, it installs an "exception handler" (carefully crafted assembly code) at a specific address in memory. When the processor raises an exception, it invokes this, which sets up a "trap frame" and calls into the operating system. Since "exception" is such an overloaded term in computer science, operating system lingo for an exception is a "trap", when the OS traps execution. Interrupts are exceptions, and more significantly for this assignment, so are system calls. Specifically, syscall.c handles traps that happen to be syscalls. Understanding at least the C code in this directory is key to being a real operating systems junkie, so we highly recommend reading through it carefully. The code is also covered in the system calls lecture.

#### `locore/trap.c`

`mips_trap()` is the key function for returning control to the operating system. This is the C function that gets called by the assembly exception handler. `kill_curthread()` is the function for handling broken user programs; when the processor is in usermode and hits something it can't handle (say, a bad instruction), it raises an exception. There's no way to recover from this, so the OS needs to kill off the process. The bonus part of this assignment will include writing a useful version of this function.

#### `syscall/syscall.c`

`syscall()` is the function that delegates the actual work of a system call off to the kernel function that implements it. Notice that reboot and time are the only cases currently handled.

##### Questions

**1.** What is the numerical value of the exception code for a MIPS system call (Hint: `EX_SYS`)?

See `kern/arch/mips/include/trapframe.h`, this is where various trapframe/exception handling definitions are

``` C
/*
 * MIPS exception codes.
 */
#define EX_IRQ    0    /* Interrupt */
#define EX_MOD    1    /* TLB Modify (write to read-only page) */
#define EX_TLBL   2    /* TLB miss on load */
#define EX_TLBS   3    /* TLB miss on store */
#define EX_ADEL   4    /* Address error on load */
#define EX_ADES   5    /* Address error on store */
#define EX_IBE    6    /* Bus error on instruction fetch */
#define EX_DBE    7    /* Bus error on data load *or* store */
#define EX_SYS    8    /* Syscall */
#define EX_BP     9    /* Breakpoint */
#define EX_RI     10   /* Reserved (illegal) instruction */
#define EX_CPU    11   /* Coprocessor unusable */
#define EX_OVF    12   /* Arithmetic overflow */
```

**2.** How many bytes is an instruction in MIPS? (Answer this by reading `syscall()` carefully, not by looking somewhere else.)

See `kern/arch/mips/syscall/syscall.c`. Also observe the switch statement. This is the code that determine the system call invoked and calls the appropriate routine in the kernel. Also observe the convention or prefixing the in-kernel implementation of a system call with `sys_`, which you should follow as well.

**3.** What is the contents of the `struct trapframe`? Where is the `struct trapframe` that is passed into `syscall()` stored?

See `trapframe.h`. It contains the register contents of the user-level application at the time of the trap, plus a few of the status registers useful to the kernel. The trapframe is stored on this process/thread's stack.

**4.** What would be required to implement a system call that took more than 4 arguments?

There are two options:

1. create a specific convention between the stubs in the user-level C-library that uses some more registers in the register set as input arguments (as we have done for the system call number)
2. use the C-compiler calling convention, which places extra arguments on the user-level stack. The OS would then have to obtain the arguments from the application stack. This has to be done very carefully as there is no guarantee the application has a valid user-level stack pointer when the system call is invoked.

**5.** What is the purpose of `userptr_t`?

`userptr_t` is the type of pointers to memory that are supplied by applications (which can't be trusted to be valid). We use this type to avoid accidentally dereferencing one of these unsafe pointers in our kernel.

### Moving data between kernel and userland.

#### `vm/copyinout.c`

This file contains functions for moving data between kernel and user space. Knowing when and how to cross this boundary is critical to properly implementing userlevel programs, so this is a good file to read very carefully. The framework provided is needed to safely implement application buffer handling in the read() and write() system calls. You should also examine the code in `lib/uio.c`.

##### Questions

**6.** What is the difference between `UIO_USERISPACE` and `UIO_USERSPACE`? When should one use `UIO_SYSSPACE` instead?

This flag indicated the destination memory to the uio framework: the kernel is `SYSSPACE`, and the application is `USERSPACE`. If `SYSSPACE` is the destination, various validity checks can be avoided - the kernel is trusted not to corrupt itself. `USERISPACE` is the flag is the data is actual code being loaded to eventually be run by the application - on real hardware, the framework would flush the CPU instruction cache to ensure it is consistent with the content of memory.

``` C
enum uio_seg {
    UIO_USERISPACE,                 /* User process code. */
    UIO_USERSPACE,                  /* User process data. */
    UIO_SYSSPACE,                   /* Kernel. */
};
```

**7.** Why can the `struct uio` that is used to read in an ELF segment be allocated on the stack in `load_segment()` (i.e., where is the destination memory for the read)?

The `uio` is only metadata describing the destination or source of a data transfer. It can be on the stack as it is private to the kernel, the actual destination of load segment is the point in the `struct iovec`

``` C
struct iovec {

#ifdef _KERNEL
    union {
        userptr_t  iov_ubase;   /* user-supplied pointer */
        void      *iov_kbase;   /* kernel-supplied pointer */
    };
#else
    void *iov_base;                 /* user-supplied pointer */
#endif
    size_t iov_len;                 /* Length of data */
};

struct uio {
    struct iovec     *uio_iov;      /* Data blocks */
    unsigned          uio_iovcnt;   /* Number of iovecs */
    off_t             uio_offset;   /* Desired offset into object */
    size_t            uio_resid;    /* Remaining amt of data to xfer */
    enum uio_seg      uio_segflg;   /* What kind of pointer we have */
    enum uio_rw       uio_rw;       /* Whether op is a read or write */
    struct addrspace *uio_space;    /* Address space for user pointer */
};
```

See sample initialisation in `load_segment`

``` C
    iov.iov_ubase = (userptr_t)vaddr;
    iov.iov_len = memsize;           // length of the memory space
    u.uio_iov = &iov;
    u.uio_iovcnt = 1;
    u.uio_resid = filesize;          // amount to read from the file
    u.uio_offset = offset;
    u.uio_segflg = is_executable ? UIO_USERISPACE : UIO_USERSPACE;
    u.uio_rw = UIO_READ;
    u.uio_space = as;
```

**8.** In what file are `copyin()` and `copyout()` defined? `memmove()`? Why can't `copyin()` and `copyout()` be implemented as simply as `memmove()`?

`copyin()` and `copyout()` located in `kern/vm/copyinout.c`.  
`memmove()` can't be used to copy data from/to applications as it has no way to recover if a fault occurs during the copy

### The VFS interface.

#### `kern/include`

The files `vfs.h` and `vnode.h` in this directory contain function declarations and comments that are directly relevant to this assignment.

##### Questions

**9.** How are `vfs_open`, `vfs_close` used? What other `vfs_()` calls are relevant?

`vfs_open()/vfs_close()` implement most of the work needed to implement the system calls `open()/close()`. Read the man page for open and close to understand their semantics, then look at `vfs_open/vfs_close` to see how they relate. The remaining vfs operations required for this assignment are the `VOP_*` functions.

**10.** What are `VOP_READ`, `VOP_WRITE`? How are they used?

They are the implementation of read and write for the filesystem. The main thing to understand is how to use the uio struct to specify the source or destination of the read or write.

``` C
/*
 *    vop_read        - Read data from file to uio, at offset specified
 *                      in the uio, updating uio_resid to reflect the
 *                      amount read, and updating uio_offset to match.
 *                      Not allowed on directories or symlinks.
 *
 *    vop_write       - Write data from uio to file at offset specified
 *                      in the uio, updating uio_resid to reflect the
 *                      amount written, and updating uio_offset to match.
 *                      Not allowed on directories or symlinks.
 */
```

**11.** What does `VOP_ISSEEKABLE` do?

Returns whether the vnode is seekable (via `lseek()`). If not, `lseek` can return an error, if seekable, `lseek` should update your file pointer.

**12.** Where is the `struct thread` defined? What does this structure contain?

In `kern/include/thread.h`

``` C
/* Thread structure. */
struct thread {
    /*
        * These go up front so they're easy to get to even if the
        * debugger is messed up.
        */
    char *t_name;                   /* Name of this thread */
    const char *t_wchan_name;       /* Name of wait channel, if sleeping */
    threadstate_t t_state;          /* State this thread is in */

    /*
        * Thread subsystem internal fields.
        */
    struct thread_machdep t_machdep; /* Any machine-dependent goo */
    struct threadlistnode t_listnode; /* Link for run/sleep/zombie lists */
    void *t_stack;                  /* Kernel-level stack */
    struct switchframe *t_context;  /* Saved register context (on stack) */
    struct cpu *t_cpu;              /* CPU thread runs on */
    struct proc *t_proc;            /* Process thread belongs to */

    /*
        * Interrupt state fields.
        *
        * t_in_interrupt is true if current execution is in an
        * interrupt handler, which means the thread's normal context
        * of execution is stopped somewhere in the middle of doing
        * something else. This makes assorted operations unsafe.
        *
        * See notes in spinlock.c regarding t_curspl and t_iplhigh_count.
        *
        * Exercise for the student: why is this material per-thread
        * rather than per-cpu or global?
        */
    bool t_in_interrupt;            /* Are we in an interrupt? */
    int t_curspl;                   /* Current spl*() state */
    int t_iplhigh_count;            /* # of times IPL has been raised */

    /*
        * Public fields
        */

    /* add more here as needed */
};
```

**13.** Where is the `struct proc` defined? What does this structure contain?

In `kern/include/proc.h`

``` C
/*
 * Process structure.
 */
struct proc {
    char *p_name;                   /* Name of this process */
    struct spinlock p_lock;         /* Lock for this structure */
    struct threadarray p_threads;   /* Threads in this process */

    /* VM */
    struct addrspace *p_addrspace;  /* virtual address space */

    /* VFS */
    struct vnode *p_cwd;            /* current working directory */

    /* add more material here as needed */
};
```

### Loading and running programs.

These questions mostly relate to the advanced assignment, but are worth exploring to understand how a program starts running.

#### `kern/syscall`

This directory contains some syscall implementations, and the files that are responsible for the loading and running of user-level programs. Currently, the only files in the directory are `loadelf.c`, `runprogram.c`, and `time_syscalls.c`, although you may want to add more of your own during this assignment. Understanding these files is the key to getting started with the assignment, especially the advanced part, the implementation of multiprogramming. Note that to answer some of the questions, you will have to look in files outside this directory.

#### `loadelf.c`

This file contains the functions responsible for loading an ELF executable from the filesystem and into virtual memory space. Of course, at this point this virtual memory space does not provide what is normally meant by virtual memory, although there is translation between the addresses that executables "believe" they are using and physical addresses, there is no mechanism for providing more memory than exists physically.

#### `runprogram.c`

This file contains only one function, runprogram(), which is the function that is responsible for running a program from the kernel menu. Once you have designed your file system calls, a program started by runprogram() should have the standard file descriptors (stdout, stderr) available while it's running.

In the advanced assignment, runprogram() is a good base for writing the execv() system call, but only a base. When thinking about your design, you should determine what more is required for execv() that runprogram() does not need to worry about. Once you have designed your process framework, runprogram() should be altered to start processes properly within this framework.

##### Questions

**14.** What are the ELF magic numbers?

They are numbers used as a sanity check to validate an elf file. Numbers are located in `elf.h`:

``` C
/* For e_ident[EI_MAG0..3] */
#define	ELFMAG0         0x7f
#define	ELFMAG1         E'
#define	ELFMAG2         L'
#define	ELFMAG3         F'
```

**15.** In `runprogram()`, why is it important to call `vfs_close` before going to usermode?

After switching to the user application, `runprogram` never returns. Thusm `vfs_close` must be called to avoid a memory leak.

**16.** What function forces the processor to switch into usermode? Is this function machine dependent?

`mips_usermode()`. Yes, it is machine dependent.

``` C
void
enter_new_process(int argc, userptr_t argv, userptr_t env,
                  vaddr_t stack, vaddr_t entry)
{
    struct trapframe tf;

    bzero(&tf, sizeof(tf));

    tf.tf_status = CST_IRQMASK | CST_IEp | CST_KUp;
    tf.tf_epc = entry;
    tf.tf_a0 = argc;
    tf.tf_a1 = (vaddr_t)argv;
    tf.tf_a2 = (vaddr_t)env;
    tf.tf_sp = stack;

    mips_usermode(&tf);
}
```

### fork()

Answer these questions by reading the fork() man page and the sections dealing with fork() in the textbook.

#### Questions

**17.** What is the purpose of the fork() system call?

To duplicate the current process, with the new child process starting at the same program counter (PC) as the parent, just after the `fork` syscall is executed. See `man` page for more information.

**18.** What process state is shared between the parent and child?

Open file table entries (including the file pointer) that were open prior to the `fork` call are shared.

**19.** What process state is copied between the parent and child?

File descriptor tables (i.e. the per-process mapping between file-descriptors and open files) and memory.

### The userland side of system calls.

This section is mostly related to the advanced version, but is still generally insightful to understand how system calls transition into the kernel. This was covered in some detail in the system call lecture in week 4.

#### `userland/lib/crt0`

The userland C startup code. There's only one file in here, mips/crt0.S, which contains the MIPS assembly code that receives control first when a user-level program is started. It calls main(). This is the code that your execv() implementation will be interfacing to, so be sure to check what values it expects to appear in what registers and so forth.

#### `userland/lib/libc`:

There's obviously a lot of code in the OS/161 C library (and a lot more yet in a real system's C library...) We don't expect you to read it all, although it may be instructive in the long run to do so. Job interviewers have an uncanny habit of asking people to implement simple standard C functions on the whiteboard. For present purposes you need only look at the code that implements the user-level side of system calls.

#### `errno.c`

This is where the global variable errno is defined.

#### `syscalls-mips.S`

This file contains the machine-dependent code necessary for implementing the userlevel side of MIPS system calls. syscalls.S is created from this file at compile time and is the actual file assembled to put into the C library. The actual names of the system calls are placed in this file using a script called gensyscalls.sh that reads them from the kernel's header files. This avoids having to make a second list of the system calls. In a real system, typically each system call stub is placed in its own source file, to allow selectively linking them in. OS/161 puts them all together to simplify the makefiles.

##### Questions

**20.** What is the purpose of the SYSCALL macro?

It defines the syscall stubs, see `userland/lib/lib/arch/mips/syscalls-mips.S`

**21.** What is the MIPS instruction that actually triggers a system call? (Answer this by reading the source in this directory, not looking somewhere else.)

Line 83 of `userland/lib/libc/arch/mips/syscalls-mips.S`

``` assembly
__syscall:
   syscall              /* make system call */
   beq a3, $0, 1f       /* if a3 is zero, call succeeded */
   nop                  /* delpay slot */
   sw v0, errno         /* call failed: store errno */
   li v1, -1            /* and force return value to -1 */
   li v0, -1
```

Page last modified: 8:43pm on Sunday, 22nd of March, 2020
