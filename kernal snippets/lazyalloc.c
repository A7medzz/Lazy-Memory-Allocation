// ========== ADDED BY MEMBER 4 ==========
// Core lazy allocation function
// Allocates physical page on page fault
int
lazy_alloc(pagetable_t pagetable, uint64 va)
{
  char *mem;
  
  // Step 1: Align to page boundary
  va = PGROUNDDOWN(va);
  
  // Step 2: Safety check - already mapped?
  if(walkaddr(pagetable, va) != 0) {
    return 0;  // Already mapped
  }
  
  // Step 3: Allocate physical page frame
  mem = kalloc();
  if(mem == 0) {
    return -1;  // Out of memory
  }
  
  // Step 4: Zero the page (security)
  memset(mem, 0, PGSIZE);
  
  // Step 5: Map virtual to physical with user permissions
  if(mappages(pagetable, va, PGSIZE, (uint64)mem, PTE_R | PTE_W | PTE_U) != 0) {
    kfree(mem);
    return -1;
  }
  
  return 0;
}
// ==========================================