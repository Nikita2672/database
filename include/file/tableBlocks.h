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

typedef struct {
    char fieldName[MAX_LENGTH_FIELD_NAME];
    enum DataType dataType;
} NameTypeBlock;

typedef struct {
    bool isActive;
    char tableName[MAX_LENGTH_TABLE_NAME];
    NameTypeBlock nameTypeBlock[MAX_FIELDS];
    uint8_t fieldsNumber;
    uint64_t firsTableBlockOffset;
    uint64_t lastTableBLockOffset;
} TableOffsetBlock;

typedef struct {
    uint32_t countTables;
    TableOffsetBlock tableOffsetBlock[MAX_TABLES];
    uint64_t emptySpaceOffset;
} DefineTablesBlock;

NameTypeBlock *initNameTypeBlock(const char *fieldName, enum DataType dataType);

TableOffsetBlock *initTableOffsetBlock(FILE *file, const char *name, uint8_t fieldsNumber, NameTypeBlock *nameTypeBlock);

#define LAB1_BLOCKS_H

#endif //LAB1_BLOCKS_H
