// ========== MODIFIED BY MEMBER 2 ==========
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
    p->sz += n;
    
  } else if(n < 0) {
    // Deallocation: Free physical pages that were actually allocated
    uint64 newsz = addr + n;  // n is negative, so newsz < addr
    uvmdealloc(p->pagetable, addr, newsz);
    p->sz = newsz;
  }
  
  return addr;
}
// ==========================================