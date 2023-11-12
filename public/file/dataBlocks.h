#include "stdlib.h"
#include "stdint.h"
#include "stdbool.h"

#define BLOCK_DATA_SIZE 8192
#ifndef LAB1_DATABLOCKS_H
#define LAB1_DATABLOCKS_H

struct specialDataSection {
    uint64_t previousBlockOffset;
    uint64_t nextBlockOffset;
};

struct headerSection {
    uint16_t pageNumber;
    uint16_t startEmptySpaceOffset;
    uint16_t endEmptySpaceOffset;
    uint8_t recordsNumber;
};

struct recordId {
    uint16_t offset;
    uint64_t length;
};
#endif //LAB1_DATABLOCKS_H
