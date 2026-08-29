# EdgeOS
## A Resource-Aware Operating System for Memory-Constrained Edge Devices

EdgeOS is a small experimental 32-bit x86 operating-system kernel developed to demonstrate resource-aware memory management for constrained edge devices.

The project combines:

- First-Fit memory allocation
- `kmalloc()` and `kfree()`
- Block splitting
- Free-block coalescing
- External fragmentation measurement
- Kernel-side system-call dispatcher
- Process Control Blocks
- Process priorities and states
- Priority Scheduling
- Round Robin Scheduling
- Memory-pressure monitoring
- Priority-aware process reclamation

## Key Results

### Fragmentation Optimization

```text
28% → 0%
