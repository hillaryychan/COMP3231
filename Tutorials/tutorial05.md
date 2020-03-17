# Tutorial Week 5

## Questions

### Memory Hierarchy and Caching

**Q1.** Describe the memory hierarchy. What types of memory appear in it? What are the characteristics of the memory as one moves through the hierarchy? How can do memory hierarchies provide both fast access times and large capacity?

**Q2.** Given that disks can stream data quite fast (1 block in tens of microseconds), why are average access times for a block in milliseconds?

**Q3.** You have a choice of buying a 3 Ghz processor with 512K cache, and a 2 GHz processor (of the same type) with a 3 MB cache for the same price. Assuming memory is the same speed in both machines and is much less than 2GHz (say 400MHz). Which would you purchase and why? Hint: You should consider what applications you expect to run on the machine.

### Files and file systems

**Q4.** Consider a file currently consisting of 100 records of 400 bytes. The filesystem uses fixed blocking, i.e. one 400 byte record is stored per 512 byte block. Assume that the file control block (and the index block, in the case of indexed allocation) is already in memory. Calculate how many disk I/O operations are required for contiguous, linked, and indexed (single-level) allocation strategies, if, for one record, the following conditions hold. In the contiguous-allocation case, assume that there is no room to grow at the beginning, but there is room to grow at the end of the file. Assume that the record information to be added is stored in memory.

  a. The record is added at the beginning.  
  b. The record is added in the middle.  
  c. The record is added at the end.  
  d. The record is removed from the beginning.  
  e. The record is removed from the middle.  
  f. The record is removed from the end.  


**Q5.** In the previous example, only 400 bytes is stored in each 512 byte block. Is this wasted space due to internal or external fragmentation?

**Q6.** Old versions of UNIX allowed you to write to directories. Newer ones do not even allow the superuser to write to them? Why? Note that many unices allow you read directories.

**Q7.** Given a file which varies in size from 4KiB to 4MiB, which of the three allocation schemes (contiguous, linked-list, or i-node based) would be suitable to store such a file? If the file is access randomly, how would that influence the suitability of the three schemes?

**Q8.** Why is there VFS Layer in Unix?

**Q9.** How does choice of block size affect file system performance. You should consider both sequential and random access.

**Q10.** Is the open() system call in UNIX essential? What would be the consequence of not having it?

**Q11.** Some operating system provide a rename system call to give a file a new name. What would be different compared to the approach of simply copying the file to a new name and then deleting the original file?

**Q12.** In both UNIX and Windows, random file access is performed by having a special system call that moves the current position in the file so the subsequent read or write is performed from the new position. What would be the consequence of not having such a call. How could random access be supported by alternative means?

Page last modified: 11:13pm on Sunday, 15th of March, 2020
