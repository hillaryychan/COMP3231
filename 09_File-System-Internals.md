# File System Internals

UNIX storage stack:

![UNIX storage stack](imgs/9-2_UNIX-storage-stack.jpg)

Some popular file systems:

![popular file systems](imgs/9-12_popular-file-systems.png)

Why are there so many?

* Different physical nature of storage devices
  * Ext3 is optimised for magnetic disks
  * JFFS2 is optimised for flash memory devices
  * ISO9660 is optimised for CD-ROM
* Different storage capacities
  * FAT16 does not support drives >2GB
  * FAT32 becomes inefficient on drives >32GB
  * ZFS, Btrfs is designed to scale to multi-TB disk arrays
* Different CPU and memory requirements
  * FAT16 is not suitable for modern PCs but is a good fit for many embedded devices
* Proprietary standards
  * NTFS may be a nice file system, but its specification is closed

## File Systems for Magnetic Disks
