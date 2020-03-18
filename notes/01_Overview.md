# Overview

## What is an operating system?

The OS can be seen as an **abstract machine**. It extends the basic hardware with added functionality. The OS provides high level abstractions so that is is more programmer friendly and there is a common core for all applications. It hides the details of hardware and makes application code portable

The OS can also be seen as a **resource manager**. It is responsible for allocating resources to users and processes. It must ensure there is

* no starvation
* progress
* allocation according to some desired policy - e.g. first-come, first-served; fair share; weighted fair share; limits (quotas) etc.
* the system is efficiently used

A structural (implementation) view: the OS **_is_** the software _privileged_ mode

![structural implementation view](../imgs/1-11_structural-impl-view.png)

The **OS kernel** (also called **nucleus** and **supervisor**) is a portion of the operating system that is running in _privileged mode_. It usually stays in main memory and contains fundamental functionality; such as whatever is required to implement other services and to provide security. The kernel contains most-frequently used functions.

The OS is privileged. Applications should not be able to interfere or bypass the OS. The OS can enforce the "extended machine" and its resource allocation policies. This prevents applications from interfering with each other.

Delving deeper into the structure of a computer system

![computer system structure](../imgs/1-14_computer-system-interactions.jpg)

It is important to note that:

* the OS can interact with memory, devices, and applications
* the application can only interact with itself and system libraries; the only way it can interact with the OS is via system calls which are performed in system libraries

Some embedded OSs have **no privileged** component. e.g. PalmOS, Mac OS 9, RTEMS. They can implement OS functionality, but cannot enforce it. This means all software runs together, there is no isolation and one fault can potentially bring down the entire system.

**System libraries** are libraries of support functions (procedures, subroutines). Only a subset of library functions are actually system calls. Consider the following functions:

* `strcmp()` and `memcpy()` are **pure library functions**. They manipulate memory within the application, or perform computation
* `open()`, `close()`, `read()`, `write()` are **system calls**. They cross the user-kernel boundary. e.g. to read from a disk device. Their implementation is mainly focused on passing requests to the OS and returning results to the application

System call functions are in the library for convenience. Try `man syscalls` on Linux for a list of system calls.

Fundamentally, OS functions the same way as ordinary computer software. It is machine code that is executed (with the same machine instructions as applications). It has more privileges (i.e. extra instructions and access). The OS relinquishes control of the processor to execute other programs, then it re-establishes control after system calls and interrupts (especially timer interrupts).

## OS Internal Structure

Most operating systems are designed as a **monolithic operating system structure** (also called the "spaghetti nest" approach). Basically everything is tangled up with everything else, however there is some reasonable structure that usually prevails.

Below are the monolithic structures of Linux and Windows

![monolithic os structures](../imgs/1-22_monolithic-os-structures.jpg)
