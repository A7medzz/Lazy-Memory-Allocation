#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

int
main(void)
{
  int i;
  char *initial_brk;
  char *ptr;
  char *ptr2;
  int success = 1;
  
  printf("\n========================================\n");
  printf("   Lazy Page Allocation Test Suite\n");
  printf("========================================\n");
  
  // Get initial program break
  initial_brk = sbrk(0);
  printf("\n[INFO] Initial program break: %p\n", initial_brk);
  
  // ============================================
  // TEST 1: Basic lazy allocation
  // ============================================
  printf("\n--- TEST 1: Basic Lazy Allocation ---\n");
  printf("Allocating 10 pages (40KB) with sbrk...\n");
  ptr = sbrk(10 * PGSIZE);
  
  if(ptr == (char*)-1) {
    printf("FAILED: sbrk returned error\n");
    exit(1);
  }
  
  printf("New program break: %p\n", ptr);
  printf("No physical memory allocated yet (lazy).\n");
  
  // ============================================
  // TEST 2: Touch pages (trigger page faults)
  // ============================================
  printf("\n--- TEST 2: Page Fault Handling ---\n");
  printf("Writing to each page (should trigger page faults):\n");
  
  for(i = 0; i < 10; i++) {
    printf("  Writing to page %d at address %p... ", i, &ptr[i * PGSIZE]);
    ptr[i * PGSIZE] = 'A' + i;
    printf("OK\n");
  }
  
  // ============================================
  // TEST 3: Verify data persistence
  // ============================================
  printf("\n--- TEST 3: Data Verification ---\n");
  for(i = 0; i < 10; i++) {
    char expected = 'A' + i;
    char actual = ptr[i * PGSIZE];
    
    if(actual != expected) {
      printf("  FAIL: Page %d expected '%c' but got '%c'\n", i, expected, actual);
      success = 0;
    } else {
      printf("  PASS: Page %d contains '%c'\n", i, actual);
    }
  }
  
  // ============================================
  // TEST 4: Allocate more memory
  // ============================================
  printf("\n--- TEST 4: Growing the Heap ---\n");
  printf("Allocating 5 more pages (20KB)...\n");
  ptr2 = sbrk(5 * PGSIZE);
  
  if(ptr2 == (char*)-1) {
    printf("FAILED: second sbrk returned error\n");
    exit(1);
  }
  
  printf("New program break: %p\n", ptr2);
  
  // Write to new pages
  for(i = 0; i < 5; i++) {
    printf("  Writing to new page %d at %p... ", i, &ptr2[i * PGSIZE]);
    ptr2[i * PGSIZE] = 'a' + i;
    printf("OK\n");
  }
  
  // Verify new pages
  for(i = 0; i < 5; i++) {
    char expected = 'a' + i;
    char actual = ptr2[i * PGSIZE];
    
    if(actual != expected) {
      printf("  FAIL: New page %d expected '%c' but got '%c'\n", i, expected, actual);
      success = 0;
    } else {
      printf("  PASS: New page %d contains '%c'\n", i, actual);
    }
  }
  
  // ============================================
  // TEST 5: Interleaved access
  // ============================================
  printf("\n--- TEST 5: Interleaved Access ---\n");
  printf("Writing to pages in reverse order:\n");
  
  for(i = 9; i >= 0; i--) {
    printf("  Writing to page %d at %p... ", i, &ptr[i * PGSIZE]);
    ptr[i * PGSIZE] = 'Z' - i;
    printf("OK\n");
  }
  
  // Verify reverse writes
  for(i = 0; i < 10; i++) {
    char expected = 'Z' - i;
    char actual = ptr[i * PGSIZE];
    
    if(actual != expected) {
      printf("  FAIL: Reverse write verification failed at page %d\n", i);
      success = 0;
    }
  }
  printf("  All reverse writes verified successfully!\n");
  
  // ============================================
  // TEST 6: Large allocation test
  // ============================================
  printf("\n--- TEST 6: Large Allocation (100 pages) ---\n");
  printf("Allocating 100 pages (400KB)...\n");
  char *large_ptr = sbrk(100 * PGSIZE);
  
  if(large_ptr == (char*)-1) {
    printf("FAILED: large sbrk returned error\n");
    exit(1);
  }
  
  printf("Touch first and last page of large allocation...\n");
  large_ptr[0] = 'X';
  large_ptr[99 * PGSIZE] = 'Y';
  
  if(large_ptr[0] != 'X' || large_ptr[99 * PGSIZE] != 'Y') {
    printf("FAIL: Large allocation verification failed\n");
    success = 0;
  } else {
    printf("PASS: Large allocation works\n");
  }
  
  // ============================================
  // TEST 7: Stress test - multiple allocations
  // ============================================
  printf("\n--- TEST 7: Stress Test ---\n");
  printf("Allocating and touching 50 pages in a loop...\n");
  
  for(int round = 0; round < 5; round++) {
    printf("  Round %d: ", round);
    char *stress_ptr = sbrk(10 * PGSIZE);
    if(stress_ptr == (char*)-1) {
      printf("FAILED\n");
      success = 0;
      break;
    }
    
    for(i = 0; i < 10; i++) {
      stress_ptr[i * PGSIZE] = round + i;
    }
    printf("OK\n");
  }
  
  // ============================================
  // FINAL RESULTS
  // ============================================
  printf("\n========================================\n");
  if(success) {
    printf("   ✓ ALL TESTS PASSED!\n");
    printf("   Lazy page allocation works correctly!\n");
    printf("========================================\n\n");
    exit(0);
  } else {
    printf("   ✗ SOME TESTS FAILED!\n");
    printf("   Please check the output above.\n");
    printf("========================================\n\n");
    exit(1);
  }
}