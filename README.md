# xv6 Lazy-Memory-Allocation

## Overview
This project implements lazy page allocation (demand paging) in xv6-riscv.

## Team Members
- **Member 1**: Memory Management Lead - `riscv.h`, `defs.h`
- **Member 2**: System Call Expert - `sysproc.c`  
- **Member 3**: Trap Handler Expert - `trap.c`
- **Member 4**: VM Layer Specialist - `vm.c`
- **Member 5**: Testing & Integration - `lazytest.c`, `Makefile`

## Quick Start
```bash
git clone https://github.com/your-org/xv6-lazy-alloc
cd xv6-lazy-alloc
make qemu
lazytest