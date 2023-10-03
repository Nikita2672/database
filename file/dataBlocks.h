#include "stdlib.h"
#include "stdint.h"

#define BLOCK_DATA_SIZE 8192
#ifndef LAB1_DATABLOCKS_H
#define LAB1_DATABLOCKS_H

struct specialDataSection {
    u_int64_t previousBlockOffset;
    u_int64_t nextBlockOffset;
};

struct headerSection {
    u_int16_t pageNumber;
    u_int16_t startEmptySpaceOffset;
    u_int16_t endEmptySpaceOffset;
    u_int8_t recordsNumber;
};

struct recordId {
    u_int16_t offset;
    u_int16_t length;
};
#endif //LAB1_DATABLOCKS_H
