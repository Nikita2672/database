#include "../data/data.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"

#ifndef LAB1_BLOCKS_H
#define MAX_LENGTH_TABLE_NAME 32
#define MAX_LENGTH_FIELD_NAME 32
#define MAX_FIELDS 256
#define MAX_TABLES 1000

struct NameTypeBlock {
    char fieldName[MAX_LENGTH_FIELD_NAME];
    enum DataType dataType;
};

struct tableOffsetBlock {
    bool isActive;
    char tableName[MAX_LENGTH_TABLE_NAME];
    struct NameTypeBlock nameTypeBlock[MAX_FIELDS];
    u_int8_t fieldsNumber;
    u_int64_t firsTableBlockOffset;
    u_int64_t lastTableBLockOffset;
};

struct defineTablesBlock {
    u_int32_t countTables;
    struct tableOffsetBlock tableOffsetBlock[MAX_TABLES];
    u_int64_t emptySpaceOffset;
};

struct NameTypeBlock* initNameTypeBlock(const char fieldName[MAX_LENGTH_FIELD_NAME], enum DataType dataType);
struct tableOffsetBlock* initTableOffsetBlock(const char name[MAX_LENGTH_TABLE_NAME],
        uint64_t firstTableOffset, uint64_t lastTableOffset, uint8_t fieldsNumber,
        const struct NameTypeBlock nameTypeBlock[MAX_FIELDS]);

#define LAB1_BLOCKS_H

#endif //LAB1_BLOCKS_H
