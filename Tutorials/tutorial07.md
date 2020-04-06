# Tutorial Week 7

## Questions

### Files and file systems

**Q1.** Why does Linux pre-allocate up to 8 blocks on a write to a file.

Pre-allocating provides better **locality** when many writes to independent files are interleaves.  The 8 blocks cannot be taken by any other file.

**Q2.** Linux uses a buffer cache to improve performance. What is the drawback of such a cache? In what scenario is it problematic? What alternative would be more appropriate where a buffer cache is inappropriate?

Buffering writes in the buffer cache provides the opportunity for data to be lost in the system prior to the cache being flushed.

Removable storage devices become problematic if user's don't 'unmount' them before removing them.

Robustness can be improved by using a **write-through** cache, which writes straight to the disk, at the expense of poor write performance.

**Q3.** What is the structure of the contents of a directory? Does it contain attributes such as creation times of files? If not, where might this information be stored?

Directories do not contain attributes, they only have a name-to-inode mapping; the attributes of a file are stored in the inode itself, which is somewhere else on the disk

**Q4.** The Unix inode structure contains a reference count. What is the reference count for? Why can't we just remove the inode without checking the reference count when a file is deleted?

Inodes contain a reference count due to hard links. The reference count is equal to the number of directory entries that reference the inode. For hard-linked files, multiple directory entries reference a single inode. The inode must not be removed until no directory entries are left (i.e. reference count 0) to ensure that the file system remains **consistent**.

**Q5.** Inode-based filesystems typically divide a file system partition into block groups. Each block group consists of a number of contiguous physical disk blocks. Inodes for a given block group are stored in the same physical location as the block groups. What are the advantages of this scheme? Are they any disadvantages?

Each group contains a **redundant** superblock. This makes the file system more **robust** to disk block failures.

Block groups keep the inodes physically closer to the file they refer to than they would be (on average) on a system without block groups. Since accessing and updating files also involves accessing or updating it inode, having the inode and file's block close together **reduces disk seek time**, and thus improves performance. The OS must take care that all blocks remain within the block group of their inode

**Q6.** Assume an inode with 10 direct blocks, as well as single, double and triple indirect block pointers. Taking into account creation and accounting of the indirect blocks themselves, what is the largest possible number of block reads and writes in order to:

  a. Read 1 byte  
  4 reads: read a single indirect, read double direct, read triple direct, read data block  
  b. Write 1 byte  
  4 writes: create a single indirect, create a double indirect, create a tripe indirect, write data block  
  3 reads, 2 writes: read a single indirect, read a double indirect, read a tripe indirect, write triple indirect, write data block  
  other combinations possible

Assume the inode is cached in memory.

**Q7.** Assume you have an inode-based filesystem. The filesystem has 512 byte blocks. Each inode has 10 direct, 1 single indirect, 1 double indirect, and 1 triple indirect block pointer. Block pointers are 4 bytes each. Assume the inode and any block free list is always in memory. Blocks are not cached.

  a. What is the maximum file size that can be stored before  
    1. the single indirect pointer is needed?  
    `10 * 512B ≈ 5K`  
    2. the double indirect pointer is needed?  
    `512B*(10 + 512B/4B) ≈ 69KB`  
    3. the triple indirect pointer is needed?  
    `512B*(10 + (512B/4B) + (512B/4B)^2) ≈ 8261KB`  
  b. What is the maximum file size supported?  
    `512B*(10 + (512B/4B) + (512B/4B)^2 + (512B/4B)^3) ≈ 1056837K`  
  c. What is the number of disk block reads required to read 1 byte from a file  
    1.  in the best case?  
    1  
    2. in the worst case?  
    4  
  d. What is the number of disk block reads and writes required to write 1 byte to a file  
    1. in the best case?  
    1 write  
    2. in the worst case?  
    4 reads, 1 write

**Q8.** A typical UNIX inode stores both the file's size and the number of blocks currently used to store the file. Why store both? Should not blocks = size / block size?

file size is just the highest offset of the file, while blocks is the physical locations of where files are written.
Blocks used to store the file are only indirectly related to the file size

The blocks used to store a file includes and indirects blocks used by the file system to keep track of the file data blocks themselves
File systems only store blocks tht actually contain file data. Sparsely populated files can have large regions that are unused within a file

**Q9.** How can deleting a file leave a inode-based file system (like ext2fs in Linux) inconsistent in the presence of a power failure.

Deleting a file consists of three separate modifications

* mark disk blocks as free
* remove the directory entry
* mark the inode as free

If the system only completes a subset of the operations (due to power failures or the like), the file system is no longer consistent.

**Q10.** How does adding journaling to a file system avoid corruption in the presence of unexpected power failures.

Adding a journal addresses the issue by grouping file system updates into transactions that should either completely fail or succeed. These transactions are logged prior to manipulating the file system. In the presence of failure, the transaction can be completed by replaing the updates remaining in the log.

Page last modified: 2:25pm on Sunday, 29th of March, 2020
