// user/minitest.c
#include "kernel/types.h"
#include "user/user.h"

int main() {
    printf("Testing lazy allocation...\n");
    char *p = sbrk(10 * 4096);  // Allocate 10 pages
    printf("Allocated 10 pages, touching...\n");
    p[0] = 'A';                  // Should page fault
    p[4096] = 'B';               // Should page fault
    printf("Values: %c, %c\n", p[0], p[4096]);
    printf("Test passed!\n");
    exit(0);
}