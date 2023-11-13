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


typedef struct {
    void *data;
    uint64_t dataSize;
} FieldValue;


typedef struct {
    uint64_t blockOffset;
    uint16_t offsetInBlock;
    uint8_t fieldNumber;
    uint64_t positionInField;
    uint16_t idPosition;
} LinkNext;

typedef struct {
    FieldValue *fields;
    LinkNext *linkNext;
} EntityRecord;
#endif //LAB1_DATA_H
