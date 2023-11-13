#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

#define BLOCK_DATA_SIZE 8192
#ifndef LAB1_DATABLOCKS_H
#define LAB1_DATABLOCKS_H

typedef struct {
    uint64_t previousBlockOffset;
    uint64_t nextBlockOffset;
} SpecialDataSection;

typedef struct {
    uint16_t pageNumber;
    uint16_t startEmptySpaceOffset;
    uint16_t endEmptySpaceOffset;
    uint8_t recordsNumber;
} HeaderSection;

typedef struct {
    uint16_t offset;
    uint64_t length;
} RecordId;
#endif //LAB1_DATABLOCKS_H
