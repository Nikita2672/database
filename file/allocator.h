#include <stdint-gcc.h>
#include "stdio.h"

#ifndef LAB1_ALLOCATOR_H
#define LAB1_ALLOCATOR_H

uint64_t allocateBlock(FILE *file, uint64_t previousOffset, uint64_t pageNumber);

#endif //LAB1_ALLOCATOR_H
