#include <stdint-gcc.h>
#include "stdlib.h"
#include "stdbool.h"

#ifndef LAB1_DATA_H
#define LAB1_DATA_H


enum DataType {
    INT,
    DOUBLE,
    STRING,
    BOOL
};


struct FieldValue {
    void *data;
    uint64_t dataSize;
};


struct linkNext {
    uint64_t blockOffset;
    uint16_t offsetInBlock;
    uint8_t fieldNumber;
    uint64_t positionInField;
    uint16_t idPosition;
};

struct EntityRecord {
    struct FieldValue *fields;
    struct linkNext *linkNext;
};
#endif //LAB1_DATA_H
