#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

// ========== MODIFIED BY MEMBER 2 ==========
// Lazy Page Allocation in sbrk()
// Original: growproc() allocated physical memory immediately
// New: Only increment virtual size, defer allocation to page fault
uint64
sys_sbrk(void)
{
  int n;
  uint64 addr;
  struct proc *p = myproc();
  
  // Get number of bytes to increment
  argint(0, &n);
  
  // Save current program break (will be returned)
  addr = p->sz;
  
  if(n > 0) {
    // LAZY ALLOCATION: Only increase virtual address space
    // No physical memory allocated yet!
    // Physical pages will be allocated on-demand via page faults
    p->sz += n;
    
  } else if(n < 0) {
    // Deallocation: Free physical pages that were actually allocated
    // uvmdealloc handles both allocated and lazy pages correctly
    uint64 newsz = addr + n;  // n is negative, so newsz < addr
    uvmdealloc(p->pagetable, addr, newsz);
    p->sz = newsz;
  }
  
  // Return old program break (Unix sbrk semantics)
  return addr;
}
// ==========================================

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}