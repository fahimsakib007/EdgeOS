# EdgeOS

## A Resource-Aware Operating System for Memory-Constrained Edge Devices

**EdgeOS** is a small experimental **32-bit x86 operating-system kernel** designed to demonstrate how an operating system can manage limited memory more intelligently on resource-constrained edge devices.

The project combines **custom memory management, process management, CPU scheduling, kernel-side system-call dispatching, memory-pressure monitoring, and priority-aware resource reclamation** inside one bootable kernel.

> **Main idea:** when memory becomes critically constrained, EdgeOS should not treat every workload equally. It protects higher-priority workloads and reclaims memory from a lower-priority process.

---

## Key Achievements

| Feature | Measured Result |
|---|---|
| External Fragmentation | **28% → 0%** |
| Memory Pressure | **CRITICAL → NORMAL** |
| Free Memory | **171,984 B → 950,236 B** |
| Camera Process | **Protected** |
| Sensor Process | **Protected** |
| Logger Process | **Reclaimed** |

---

## Core Features

- Bootable 32-bit x86 experimental kernel
- GRUB Multiboot support
- First-Fit memory allocation
- `kmalloc()` and `kfree()`
- Memory block splitting
- Adjacent free-block coalescing
- External fragmentation measurement
- Kernel-side system-call dispatcher
- `SYS_MALLOC` and `SYS_FREE`
- Process Control Blocks (PCB)
- Process priorities and states
- Priority Scheduling
- Round Robin scheduling demonstration
- Memory-pressure monitoring
- NORMAL / WARNING / CRITICAL pressure states
- Priority-aware process reclamation
- QEMU debug output through port `0xE9`
- Reproducible build workflow using GNU Make

---

## Problem Being Addressed

Edge devices such as smart cameras, IoT gateways, monitoring systems, and embedded controllers often operate with limited RAM.

Two important problems can occur:

1. **External memory fragmentation**  
   Repeated allocation and deallocation can divide free memory into separate regions.

2. **Critical memory pressure**  
   Background processes may consume memory that is more important for critical workloads.

EdgeOS demonstrates how an operating system can respond to both problems.

---

## Example Workloads

| Process | Priority | Role |
|---|---:|---|
| Camera | HIGH | Important visual workload |
| Sensor | MEDIUM | Sensor/data collection |
| Logger | LOW | Background logging |

During the resource-pressure demonstration, the **Logger** consumes a large amount of memory. When available memory becomes critically low, EdgeOS reclaims the low-priority Logger while preserving Camera and Sensor.

---

## How EdgeOS Works

```text
               Edge Workloads
          Camera   Sensor   Logger
              \      |      /
               \     |     /
             EdgeOS Kernel
                   |
        +----------+----------+
        |          |          |
        v          v          v
   Memory      Process     Scheduler
   Manager     Manager   Priority / RR
        \          |          /
         \         |         /
          +--------+--------+
                   |
             Resource Manager
                   |
        Memory Pressure Monitor
                   |
       NORMAL / WARNING / CRITICAL
                   |
                   v
        Priority-Aware Reclamation
```

---

## Memory Management

EdgeOS uses a **1 MB experimental memory pool** and a linked-list memory-block structure.

### Allocation

`kmalloc()` uses **First-Fit** allocation.

When a free block is larger than the requested memory, EdgeOS splits the block:

```text
Before:

[                LARGE FREE BLOCK                ]

After:

[ USED REQUEST ][         REMAINING FREE         ]
```

This prevents a small request from wasting an entire free region.

### Deallocation

Memory is released using:

```c
kfree(pointer);
```

When adjacent blocks become free, EdgeOS merges them using coalescing:

```text
[ FREE ][ FREE ]  →  [       LARGER FREE BLOCK       ]
```

---

## Fragmentation Optimization

A controlled test was used to intentionally create external fragmentation.

### Before Optimization

```text
Used memory: 307200 bytes
Free memory: 741340 bytes
Free blocks: 2
Largest free block: 536540 bytes
External fragmentation: 28%
```

### After Coalescing

```text
Used memory: 0 bytes
Free memory: 1048564 bytes
Free blocks: 1
Largest free block: 1048564 bytes
External fragmentation: 0%
```

### Result

```text
External Fragmentation
        28%
         ↓
     Coalescing
         ↓
         0%
```

---

## Kernel-Side System-Call Interface

EdgeOS contains a **kernel-side system-call dispatcher** supporting:

```text
SYS_MALLOC
SYS_FREE
```

### Memory Allocation Path

```text
Application-style Request
          |
      SYS_MALLOC
          |
          v
System Call Dispatcher
          |
     kmalloc(size)
          |
          v
     Memory Manager
          |
          v
   Memory Allocated
```

### Memory Release Path

```text
Application-style Request
          |
       SYS_FREE
          |
          v
System Call Dispatcher
          |
     kfree(pointer)
          |
          v
     Memory Manager
          |
          v
Memory Released + Coalescing
```

> **Technical scope:** the current implementation demonstrates a **kernel-side system-call dispatcher**. It does not implement a full ring-3 user-mode to ring-0 interrupt/trap transition.

---

## Process Management

Each Process Control Block stores information including:

- PID
- Process name
- Priority
- State
- Allocated memory size
- Memory pointer

Supported process states include:

```text
UNUSED
READY
RUNNING
WAITING
TERMINATED
```

Process creation allocates memory from the kernel memory manager, while process termination releases the process memory.

---

## CPU Scheduling

EdgeOS demonstrates two scheduling policies.

### Priority Scheduling

Example priority order:

```text
Camera  = HIGH
Sensor  = MEDIUM
Logger  = LOW
```

Observed scheduling order:

```text
Camera → Sensor → Logger
```

### Round Robin Scheduling

Example:

```text
Round 1:
Logger → Camera → Sensor

Round 2:
Logger → Camera → Sensor
```

> **Technical scope:** scheduling currently demonstrates selection order and simulated time slices. Hardware timer-driven context switching is outside the current version.

---

## Resource-Aware Optimization

The Resource Manager monitors:

- Used memory
- Free memory
- External fragmentation
- Memory-pressure level

Pressure levels:

```text
NORMAL
WARNING
CRITICAL
```

### Before Resource Optimization

```text
Used memory: 876544 bytes
Free memory: 171984 bytes
Memory pressure: CRITICAL
```

EdgeOS detects critical memory pressure and searches for a reclaimable low-priority workload.

```text
Camera → HIGH
Sensor → MEDIUM
Logger → LOW
```

The low-priority **Logger** is selected for reclamation.

### After Resource Optimization

```text
Used memory: 98304 bytes
Free memory: 950236 bytes
Memory pressure: NORMAL
```

Result:

```text
CRITICAL Memory Pressure
          |
          v
Protect Important Workloads
          |
          v
Find LOW-Priority Process
          |
          v
Reclaim Logger
          |
          v
Recover Memory
          |
          v
NORMAL Memory Pressure
```

---

## Final Integrated Demonstration

A successful EdgeOS execution finishes with:

```text
=========================================
       EDGEOS FINAL DEMO COMPLETE
=========================================
[OK] Memory Management
[OK] Fragmentation Optimization
[OK] System Call Dispatcher
[OK] Process Management / PCB
[OK] Priority Scheduling
[OK] Round Robin Scheduling
[OK] Resource-Aware Reclamation
```

---

## Project Structure

```text
EdgeOS/
├── boot/
│   └── boot.asm
│
├── include/
│   ├── debug.h
│   ├── memory.h
│   ├── process.h
│   ├── resource.h
│   ├── scheduler.h
│   └── syscall.h
│
├── kernel/
│   ├── kernel.c
│   ├── debug.c
│   ├── process.c
│   ├── resource.c
│   ├── scheduler.c
│   └── syscall.c
│
├── memory/
│   └── memory.c
│
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg
│
├── Makefile
├── linker.ld
└── README.md
```

---

## Technologies Used

| Area | Technology |
|---|---|
| Kernel | C |
| Boot Code | x86 Assembly |
| Assembler | NASM |
| Compiler | GCC |
| Linker | GNU `ld` |
| Bootloader | GRUB |
| Emulator | QEMU |
| Build Automation | GNU Make |
| Development Environment | Ubuntu / WSL |
| Target Architecture | 32-bit x86 |

---

## Build and Run

### Install Required Packages

```bash
sudo apt update
sudo apt install build-essential gcc-multilib nasm make grub-pc-bin grub-common xorriso qemu-system-x86
```

### Build EdgeOS

```bash
cd ~/EdgeOS_SUBMISSION
make clean
make
```

### Run EdgeOS

```bash
make run
```

`make run` launches the generated `EdgeOS.iso` inside QEMU and displays kernel debug output directly in the terminal.

To exit QEMU:

```text
Ctrl + A
X
```

---

## Skills Demonstrated

This project demonstrates practical experience with:

- Operating-system architecture
- Kernel programming in C
- x86 Assembly
- Dynamic memory management
- First-Fit allocation
- Memory fragmentation analysis
- Linked data structures
- Process Control Blocks
- Process lifecycle management
- Priority Scheduling
- Round Robin scheduling
- Kernel-side system-call architecture
- Resource-pressure monitoring
- Priority-aware reclamation
- QEMU kernel debugging
- GRUB Multiboot
- WSL / Linux development
- GNU Make build automation
- Modular software design
- Experimental testing and result analysis

---

## Demo Video

### EdgeOS - 2–3 Minute Faculty Demonstration

[![Watch the EdgeOS Demo](https://img.youtube.com/vi/LxQLjPN0wLU/maxresdefault.jpg)](https://www.youtube.com/watch?v=LxQLjPN0wLU)

▶️ **[Watch the full EdgeOS demonstration on YouTube](https://www.youtube.com/watch?v=LxQLjPN0wLU)**

The video demonstrates:

- Custom memory allocation and coalescing
- External fragmentation reduction from **28% to 0%**
- `SYS_MALLOC` and `SYS_FREE`
- PCB-based process management
- Priority and Round Robin scheduling
- Resource-aware reclamation
- Memory-pressure recovery from **CRITICAL to NORMAL**
- Free-memory recovery from **171,984 B to 950,236 B**

## Technical Report

The technical report documents:

- System architecture
- Implementation details
- Experimental results
- STAR-format technical challenges
- Project limitations
- Skills demonstrated

**Technical Report:** `Add report link here after uploading the PDF to the repository`

<!--
Example after adding the report to docs/:
[Download the EdgeOS Technical Report](docs/EdgeOS_Technical_Report_FINAL.pdf)
-->

---

## Scope and Limitations

EdgeOS is an **educational experimental kernel**, not a production operating system.

The current version does not implement:

- Ring-3 user applications
- Interrupt/trap-based real user-to-kernel system calls
- Hardware timer-driven context switching
- Virtual memory and paging
- Multicore scheduling
- Persistent filesystem services
- Production device drivers
- Full networking stack

These components are possible future extensions.

---

## Conclusion

EdgeOS demonstrates that memory management can be integrated with process importance and system resource conditions rather than operating as an isolated allocator.

The project connects:

```text
Memory Management
        +
Process Management
        +
CPU Scheduling
        +
System Calls
        +
Resource Monitoring
        |
        v
Resource-Aware EdgeOS
```

The strongest demonstrated results are:

```text
External Fragmentation:
28% → 0%

Memory Pressure:
CRITICAL → NORMAL

Free Memory:
171,984 B → 950,236 B
```

The central idea of EdgeOS is:

> **When memory becomes critically constrained, protect the important workload first.**

---

## Repository

**GitHub:** https://github.com/fahimsakib007/EdgeOS
