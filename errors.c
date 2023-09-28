#include "stdio.h"

void errorAllocation(const char *string) {
    fprintf(stderr, "Memory Allocation failed for %s", string);
}

void errorOpenFile(const char *string) {
    fprintf(stderr, "Error opening file %s", string);
}