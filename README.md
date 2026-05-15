# xv6 Lazy-Memory-Allocation

## Overview
This project implements lazy page allocation (demand paging) in xv6-riscv.

## Team Members

| Name | ID | Task | Files |
|------|----|------|-------|
| Mazen Ahmed | 231001075 | VM Core Functions, Integration | `riscv.h`, `defs.h`, `vm.c → uvmunmap()` |
| Rodina Ahmed | 231002758 | System Call Expert | `sysproc.c` |
| Ayten Hassan | 231001283 | Trap Handler Expert | `trap.c` |
| Ahmed Hany | 231001623 | VM Layer Specialist | `vm.c` |
| Karim Ahmed | 231001537 | Testing | `lazytest.c`, `Makefile` |

## Quick Start
```bash
git clone https://github.com/your-org/xv6-lazy-alloc
cd xv6-lazy-alloc
make qemu
lazytest
