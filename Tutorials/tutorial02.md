# Tutorial Week 2

Tutor: Kevin Luong  
Email: kevin.luong@student.unsw.edu.au

## Questions

### Operating Systems Intro

**Q1.** What are some of the differences between a processor running in privileged mode (also called kernel mode) and user mode? Why are the two modes needed?

kernel:

* access to more instructions,
* not restricted to anything,
* can't syscall

user:

* can syscall
* CPU control registers inaccessible
* CPU management instructions are inaccessible
* Part's of address space (containing kernel code and data) are inaccessible
* Some device memeory and registers (or ports) are inaccessible

Modes are required to endure that applications (running in user mode) cannot bypass, circumvent or take control of the operating system.

**Q2.** What are the two main roles of an Operating System?

1) Manage resources distribution amongst competing programs or users according to some systedm policy
2) Provide abstraction to hardware; hide details of hardware so that applications have a common interface with it; don't need to worry about i/o or anything a syscall can do

**Q3.** Given a high-level understanding of file systems, explain how a file system fulfills the two roles of an operating system?

At the level of the hardware, storage involves low-level controller hardware and storage devices that store blocks of data at many locations in the store. The OS filesystem abstracts above all these details and provides and interface to store, name and organise arbitrary unstructured data.

The file system also arbitrates between competing processors by managing allocated and free space on the storage device, in addition to enforcing limits on storage consumption (e.g. quotas)

**Q4.** Which of the following instructions (or instruction sequences) should only be allowed in kernel mode?

o Disable all interrupts.
x Read the time of day clock.
o Set the time of day clock.
o Change the memory map.
o Write to the hard disk controller register.
x Trigger the write of all buffered blocks associated with a file back to disk (fsync).

### OS system call interface

**Q5.** The following code contains the use of typical UNIX process management system calls: fork(), execl(), exit() and getpid(). If you are unfamiliar with their function, browse the man pages on a UNIX/Linux machine get an overview, e.g: man fork
fork - clones process
execl - replace current process with a new one
exit - exit with specified code

getpid - get process id of current process

Answer the following questions about the code below.

  a. What is the value of i in the parent and child after fork.  
  i = 1 since the child is a copy of the parent process

  b. What is the value of my_pid in a parent after a child updates it?  
  The parent process id does not change. The parent and child andparent are independent after forking

  c. What is the process id of /bin/echo?  
  execl replaces the content of a running process with a specifiedexecutable. The process id does not change.

  d. Why is the code after execl not expected to be reached in the normal case? 
  execl replaces the process image with anew one and you never run anymore lines of code.
  A successful execl results in the code being replaced. If itsucceeds, execl does not return as there is no previous code toreturn to.

  e. How many times is Hello World printed when FORK_DEPTH is 3?  
  4 times

  f. How many processes are created when running the code (including the first process)?  
  8 processes;

``` txt
    |
i=1 |               |
i=2 |       |       |       |
i=3 |   |   |   |   |   |   |   |
```

``` C
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define FORK_DEPTH 3

main()
{
  int i, r;
  pid_t my_pid;

  my_pid = getpid();

  for (i = 1; i <= FORK_DEPTH; i++) {

    r = fork();

    if (r > 0) {
      /* we're in the parent process after
         successfully forking a child */

      printf("Parent process %d forked child process %d\n",my_pid, r);

    } else if (r == 0) {

      /* We're in the child process, so update my_pid */
      my_pid = getpid();

      /* run /bin/echo if we are at maximum depth, otherwise continue loop */
      if (i == FORK_DEPTH) {
        r = execl("/bin/echo","/bin/echo","Hello World",NULL);

        /* we never expect to get here, just bail out */
        exit(1);
      }
    } else { /* r < 0 */
      /* Eek, not expecting to fail, just bail ungracefully */
      exit(1);
    }
  }
}
```

**Q6.**
  a. What does the following code do?  
  Create file named testfile  
  If it fails, exit  
  Counts the length of a test string  
  Write test string "The quick brown fox jumps over the lazy dog.\n" to testfile  
  If fail to write exit  
  Close file

  b. In addition to O_WRONLY, what are the other 2 ways one can open a file?  
  `O_RDONLY` - read-only  
  `O_RDWR` - read-write  

  c. What open return in fd, what is it used for? Consider success and failure in your answer.  
  The fd is used in other file related systems to identify the file to operate on.  
  Returns and integer greater than zero for a successfully opened file  
  If failed returns -1

``` C
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char teststr[] = "The quick brown fox jumps over the lazy dog.\n";

main()
{
  int fd;
  int len;
  ssize_t r;


  fd = open("testfile", O_WRONLY | O_CREAT, 0600); // filename, open mode, permissions to open with
  if (fd < 0) {
    /* just ungracefully bail out */
    perror("File open failed");
    exit(1);
  }

  len = strlen(teststr);
  printf("Attempting to write %d bytes\n",len);

  r = write(fd, teststr, len);

  if (r < 0) {
    perror("File write failed");
    exit(1);
  }
  printf("Wrote %d bytes\n", (int) r);

  close(fd);
}
```

**Q7.** The following code is a variation of the previous code that writes twice.

  a. How big is the file (in bytes) after the two writes?  
  50 bytes. For each open file, the os keeps track of the current offset within the file. The current offset is where the next read or write will start from. The current offset is usually the location of offset of the end of the previous read or write. You would expect the file size to be 90 bytes after two 45 byte writes, if it werent for lseek's interference
    The qThe quick brown fox jumps over the lazy dog.

  b. What is `lseek()` doing that is affecting the final file size?  
  lseek moves position in file to specified loc, moves 5 char from beginning of file

  c. What over options are there in addition to SEEK_SET?  
  `SEEK_CUR`, `SEEK_END`

``` C
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char teststr[] = "The quick brown fox jumps over the lazy dog.\n";

main()
{
  int fd;
  int len;
  ssize_t r;
  off_t off;


  fd = open("testfile2", O_WRONLY | O_CREAT, 0600);
  if (fd < 0) {
    /* just ungracefully bail out */
    perror("File open failed");
    exit(1);
  }

  len = strlen(teststr);
  printf("Attempting to write %d bytes\n",len);

  r = write(fd, teststr, len);

  if (r < 0) {
    perror("File write failed");
    exit(1);
  }
  printf("Wrote %d bytes\n", (int) r);

  off = lseek(fd, 5, SEEK_SET);
  if (off < 0) {
    perror("File lseek failed");
    exit(1);
  }

  r = write(fd, teststr, len);

  if (r < 0) {
    perror("File write failed");
    exit(1);
  }
  printf("Wrote %d bytes\n", (int) r);

  close(fd);
}
```

**Q8.** Compile either of the previous two code fragments on a UNIX/Linux machine and run strace ./a.out and observe the output.

  a. What is strace doing?  
  strace prints a trace of all system calls invoked by a process along with its arguments.

  b. Without modifying the above code to print fd, what is the value of the file descriptor used to write to the open file?  
  3  
  usually 0,1,2 are stdin, stdout, stderr

  c. printf does not appear in the system call trace. What is appearing in it's place? What's happening here?  
  printf is a wrapper to write. It creates a buffer based on the string specification that is passed. The buffer is then written to (the console) fd 1 using write()

**Q9.** Compile and run the following code.

  a. What do the following code do?  
  Change working directory to parent directory then run ls.

  b. After the program runs, the current working directory of the shell is the same. Why?  
  When you run a program in the shell it forks the program then exececutes.  
  The shell forks a child process that runs the code. Each process has its own current working directory, so the code abive changes the current working directory of the child process, the current working directory of the parent remains the same

  c. In what directory does /bin/ls run in? Why?  
  exec replaces the contents of the child process with ls, not the environment the child process runs in. The current working directory is part of the environment that the OS manages on behalf of every process, so ls run in the current directory of the child process, which is '..'

``` C
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

main()
{
  int r;
  r = chdir("..");
  if (r < 0) {
    perror("Eek!");
    exit(1);
  }

  r = execl("/bin/ls","/bin/ls",NULL);  / arg to run, args, null terminate
  perror("Double eek!");
}
```

**Q10.** On UNIX, which of the following are considered system calls? Why?

  a. read()       syscall  
  b. printf()     not syscall  
  c. memcpy()     not syscall  
  d. open()       syscall  
  e. strncpy()    not syscall

### Processes and Threads

**Q11.** In the three-state process model, what do each of the three states signify? What transitions are possible between each of the states, and what causes a process (or thread) to undertake such a transition?

States:

* running - currently executed on CPU
* ready   - ready to execute but has not been selected for execution by dispatcher
* blocked - process is not runnable as it is waiting for an event before it can continue

running -> ready   : time slice expired, yield or higher priority process becomes ready
ready-> running    : dispatcher chose the next thread to run
running -> blocked : a requested resource is unavailable, so it waits for it to become available
blocked -> ready   : a resource has become available so it is ready to continue execution


**Q12.** Given N threads in a uniprocessor system. How many threads can be running at the same point in time? How many threads can be ready at the same time? How many threads can be blocked at the same time?

running: 1 since it is a uniprocessor
blocked: N - running - ready; max of N threads can be blocked
ready  : N - blocked - running; max of N-1 thread waiting

**Q13.** Compare reading a file using a single-threaded file server and a multithreaded file server. Within the file server, it takes 15 msec to get a request for work and do all the necessary processing, assuming the required block is in the main memory disk block cache. A disk operation is required for one third of the requests, which takes an additional 75 msec during which the thread sleeps. How many requests/sec can a server handled if it is single threaded? If it is multithreaded?

singlethread: 25 requests per second
cache hits take 15msec and cache misses take 90msec with 1/3
15 * 2/3 + 90 * 1/3 = 40msec average for a task to complete
1000/40 = 25 requests per second

multithread: 66 requests per second
all processes waiting for the disk is overlapped, so every request takes 15 msec
1000/15 = 66 and 2/3 requests per second

### Critical sections

**Q14.** The following fragment of code is a single line of code. How might a race condition occur if it is executed concurrently by multiple threads? Can you give an example of how an incorrect result can be computed for x.

x = x + 1;

When all threads try to access the variable; read and write to it
The memory location corresponding to x is loaded into a register, incremented and then stored back into memory. During the interval between the load and store in the first thread, another thread may perform a load, increment and store and when control passes back to the first thread, the results of the scond are overwritten.

**Q15.** The following function is called by multiple threads (potentially concurrently) in a multi-threaded program. Identify the critical section(s) that require(s) mutual exclusion. Describe the race condition or why no race condition exists.

``` C
int i;

void foo()
{
    int j;

    /* random stuff*/

    i = i + 1;      // <- critical section
    j = j + 1;

    /* more random stuff */
}
```

There is no race condition on j since it is a local variable per thread.
i is a variable shared between threads so it would form a critical section

**Q16.** The following function is called by threads in a multi-thread program. Under what conditions would it form a critical section.

``` C
void inc_mem(int *iptr)
{
    *iptr = *iptr + 1;
}
```

Form a critical section when the pointer is a global pointer. If it is a local pointer it is not.

Page last modified: 4:37pm on Thursday, 20th of February, 2020
