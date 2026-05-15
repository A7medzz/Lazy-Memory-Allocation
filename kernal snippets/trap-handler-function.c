// Page Fault Handler for Lazy Allocation
} else if(scause == CAUSE_LOAD_PAGE_FAULT || scause == CAUSE_STORE_PAGE_FAULT) {
    
    uint64 fault_va = r_stval();  // Get address that caused fault
    
    // Validate: Is this address within the process's heap?
    if(fault_va < p->sz) {
      // Valid lazy allocation request
      if(lazy_alloc(p->pagetable, fault_va) == 0) {
        // Success - page allocated, continue execution
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
      setkilled(p);
    }
