# Tutorial Week 7

## Questions

### Files and file systems

**Q1.** Why does Linux pre-allocate up to 8 blocks on a write to a file.

**Q2.** Linux uses a buffer cache to improve performance. What is the drawback of such a cache? In what scenario is it problematic? What alternative would be more appropriate where a buffer cache is inappropriate?

**Q3.** What is the structure of the contents of a directory? Does it contain attributes such as creation times of files? If not, where might this information be stored?

**Q4.** The Unix inode structure contains a reference count. What is the reference count for? Why can't we just remove the inode without checking the reference count when a file is deleted?

**Q5.** Inode-based filesystems typically divide a file system partition into block groups. Each block group consists of a number of contiguous physical disk blocks. Inodes for a given block group are stored in the same physical location as the block groups. What are the advantages of this scheme? Are they any disadvantages?

**Q6.** Assume an inode with 10 direct blocks, as well as single, double and triple indirect block pointers. Taking into account creation and accounting of the indirect blocks themselves, what is the largest possible number of block reads and writes in order to:

  a. Read 1 byte  
  b. Write 1 byte  

Assume the inode is cached in memory.

**Q7.** Assume you have an inode-based filesystem. The filesystem has 512 byte blocks. Each inode has 10 direct, 1 single indirect, 1 double indirect, and 1 triple indirect block pointer. Block pointers are 4 bytes each. Assume the inode and any block free list is always in memory. Blocks are not cached.

  a. What is the maximum file size that can be stored before  
    1. the single indirect pointer is needed?
    2. the double indirect pointer is needed?
    3. the triple indirect pointer is needed?
  b. What is the maximum file size supported?
  c. What is the number of disk block reads required to read 1 byte from a file
    1.  in the best case?
    2. in the worst case?
  d. What is the number of disk block reads and writes required to write 1 byte to a file
    1. in the best case?
    2. in the worst case?

**Q8.** A typical UNIX inode stores both the file's size and the number of blocks currently used to store the file. Why store both? Should not blocks = size / block size?

**Q9.** How can deleting a file leave a inode-based file system (like ext2fs in Linux) inconsistent in the presence of a power failure.

**Q10.** How does adding journalling to a file system avoid corruption in the presence of unexpected power failures.

Page last modified: 2:25pm on Sunday, 29th of March, 2020
