#include "stdio.h"

void errorAllocation(const char *string) {
    fprintf(stderr, "Memory Allocation failed for %s", string);
}