#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[];
void kernelvec();
extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

uint64
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();
  
  p->trapframe->epc = r_sepc();
  
  uint64 scause = r_scause();
  
  if(scause == 8){
    // System call
    if(killed(p))
      kexit(-1);

    p->trapframe->epc += 4;
    intr_on();
    syscall();
    
  } else if((which_dev = devintr()) != 0){
    // Device interrupt
  // Page Fault Handler for Lazy Allocation
  } else if(scause == CAUSE_LOAD_PAGE_FAULT || scause == CAUSE_STORE_PAGE_FAULT) {
    
    uint64 fault_va = r_stval();  // Get address that caused fault
    
    // Validate: Is this address within the process's heap?
    if(fault_va < p->sz) {
      // Valid lazy allocation request
      if(lazy_alloc(p->pagetable, fault_va) == 0) {
        // Success - page allocated, continue execution
        // The faulting instruction will be retried automatically
      } else {
        // Out of memory or mapping failed
        printf("usertrap(): OOM in lazy_alloc for va=%lx pid=%d\n", 
               fault_va, p->pid);
        setkilled(p);
      }
    } else {
      // Invalid address - outside allocated heap
      printf("usertrap(): invalid page fault va=%lx (sz=%lx) pid=%d\n", 
             fault_va, p->sz, p->pid);
      printf("            sepc=%lx scause=%lx\n", r_sepc(), scause);
      setkilled(p);
    }
  // continue
    
  } else {
    printf("usertrap(): unexpected scause 0x%lx pid=%d\n", scause, p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n", r_sepc(), r_stval());
    setkilled(p);
  }

  if(killed(p))
    kexit(-1);

  if(which_dev == 2)
    yield();

  prepare_return();

  uint64 satp = MAKE_SATP(p->pagetable);
  return satp;
}

void
prepare_return(void)
{
  struct proc *p = myproc();

  intr_off();

  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  p->trapframe->kernel_satp = r_satp();
  p->trapframe->kernel_sp = p->kstack + PGSIZE;
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();

  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP;
  x |= SSTATUS_SPIE;
  w_sstatus(x);

  w_sepc(p->trapframe->epc);
}

void 
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();
  
  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  if(which_dev == 2 && myproc() != 0)
    yield();

  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  if(cpuid() == 0){
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  w_stimecmp(r_time() + 1000000);
}

int
devintr()
{
  uint64 scause = r_scause();

  if(scause == 0x8000000000000009L){
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000005L){
    clockintr();
    return 2;
  } else {
    return 0;
  }
}