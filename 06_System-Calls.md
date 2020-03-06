# System Calls

![Structure of a computer system](imgs/6-4_computer-system-structure.png)

**System calls** can be viewed as special function calls that provide a controlled entry into the kernel. While in the kernel, they perform a privileged operation and return to the original caller with the result. They restrict possible entry points to secure locations to prevent entering after any security checks.  
The system call interface represents the abstract machine provided by the operating system.

A brief overview of the system call interface from the user's perspective:

* process management
* file I/O
* directories management
* some other selected calls
* many more; see `man syscalls` for a list

Some system calls for process management

| Call                                      | Description                                       |
| ---                                       | ---                                               |
| `pid = fork()`                            | create a child process identical to the parent    |
| `pid = waitpid(pid, &statloc, options)`   | wait for a child process to terminate             |
| `s = execve(name, argv, environp)`        | replace a process' core image                     |
| `exit(status)`                            | terminate process execution and return status     |

Some system calls for file management

| Call                                      | Description                              |
| ---                                       | ---                                      |
| `fd = open(file, how, ...)`               | open a file for reading, writing or both |
| `s = close(fd)`                           | close an open file                       |
| `n = read(fd, buffer, nbytes)`            | read data from a file into a buffer      |
| `n = write(fd, buffer, nbytes)`           | write data from a buffer into a file     |
| `position = lseek(fd, offset, whence)`    | move the file pointer                    |
| `s = stat(name, &buf)`                    | get a file's status information          |

A stripped down shell is essentially:

``` C
while (TRUE) {                              // repeat forever
    type_prompt();                          // display prompt
    read_command(command, parameters)       // input from terminal

    if (fork() != 0) {                      // fork off child process
        /* Parent code */
        wait(-1, &status, 0);               // wait for child to exit
    } else {
        /* Child code */
        execve(command, parameters, 0);     // execute command
    }
}
```

Some Win32 API calls and their equivalent UNIX syscalls:

![Win32 API calls](imgs/6-10_win32-api-calls.png)

## System Call Implementation

A simple model of CPU computation uses the **fetch-execute cycle**.

1. load memory contents from address in program counter (PC)
2. execute instruction
3. increment PC
4. repeat

![fetch-execute cycle](imgs/6-12_fetch-execute.png)

There is/are also

* stack pointer (SP)
* status register for condition codes
  * positive result
  * zero result
  * negative result
* general purpose registers, which hold operands of most instructions and enables programmers (compilers) to minimise memory references

In privileged-mode operation, to protect the operating system execution, two or more CPU modes of operation exist;  
**Privileged mode** (or **system/kernel-mode**), where all instructions and registers are available  
**User-mode**, which uses and can only access 'safe' subset of the instruction set. It only affects the state of the application itself and cannot be used to uncontrollable interfere with the OS

![cpu registers](imgs/6-14_cpu-registers.jpg)

An example of an _unsafe_ instruction would be the `cli` instruction on the x86 architecture, which disables interrupts. An example exploit:

``` assembly
cli /* disable interupts */
while (true)
    /* loop forever */
```

The accessibility of addresses within an address space changes depending on the operating mode (to protect kernel code and data). Note that the exact memory ranges are usually configurable and vary between CPU architectures and/or operating systems 

![accessible memory space](imgs/6-16_accessible-memory-space.png)

### System Call Mechanism Overview

When a system call is made the syscall mechanism securely transfers from user execution to kernel execution and back.

![syscall diagram](imgs/6-17_syscall-diagram.jpg)

System call transitions are triggered by special processor instructions.  
From user to kernel via a system call instruction  
from kernel to user via a return from a privileged mode instruction

During a system call:

* the processor mode
  * switched from user-mode to kernel-mode
  * switched back when returning to user-mode
* the stack pointer (SP)
  * user-level SP is saved and kernel SP is initialised
  * user-level SP is restored when returning to user-mode
* the program counter (PC)
  * user-level PC is saved and PC is set to kernel entry point
  * user-level PC is restored when returning to user-level
  * kernel entry via designated entry point must be strictly enforced
* registers
  * set at user-level to indicate the system call type and its arguments; this is a convention between applications and the kernel
  * some registers are preserved at user-level or kernel level in order to restart user-level execution; this depends on the language calling convention etc.
  * result of the system call is placed in registers when returning to user-level; another convention

Steps in making a system call:

![Steps in making a syscall](imgs/6-23_steps-in-making-a-syscall.png)

### MIPS R2000/R3000

Before we look at system call mechanisms in detail, we'll need a basic understanding of MIPS R3000

The processor control registers are located in CP0. It contains exception/interrupt management registers, translation management registers etc. CP0 is manipulated using `mtc0` (move to and `mfc0` (move from) instructions; these instructions are only accessible in kernel mode

CP0 Registers:

* Exception Management
  * `c0_cause` **stores the cause of the recent exception**
  * `c0_status` **stores the current status of the CPU**
  * `c0_epc` **stores the address of the instruction that caused the exception**
  * `c0_baddvaddr` stores the address accessed that caused the exception
* Miscellaneous
  * `c0_prid` stores the process identifier
* Memory Management
  * `c0_index`
  * `c0_random`
  * `c0_entryhi`
  * `c0_entrylo`
  * `c0_context`

For now we will only focus on the registers in **bold**. We will look at the other ones later

![CP0 registers](imgs/6-26_cp0-registers.jpg)

### Hardware Exception Handling

![hardware exception handling](imgs/6-35_hardware-exception-handling.jpg)
