// vm.c
...
int             ismapped(pagetable_t, uint64);
uint64          vmfault(pagetable_t, uint64, int);

// ========== ADDED BY MEMBER 1 ==========
int             lazy_alloc(pagetable_t, uint64);
// ========================================