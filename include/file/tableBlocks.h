#include <stdio.h>
#include "../../include/data/data.h"
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
    uint8_t fieldsNumber;
    uint64_t firsTableBlockOffset;
    uint64_t lastTableBLockOffset;
};

struct defineTablesBlock {
    uint32_t countTables;
    struct tableOffsetBlock tableOffsetBlock[MAX_TABLES];
    uint64_t emptySpaceOffset;
};

struct NameTypeBlock *initNameTypeBlock(const char fieldName[MAX_LENGTH_FIELD_NAME], enum DataType dataType);

struct tableOffsetBlock *initTableOffsetBlock(FILE *file, const char name[MAX_LENGTH_TABLE_NAME],
                                              uint8_t fieldsNumber,
                                              const struct NameTypeBlock nameTypeBlock[MAX_FIELDS]);

#define LAB1_BLOCKS_H

#endif //LAB1_BLOCKS_H
