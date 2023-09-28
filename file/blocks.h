#include "../data/data.h"
#include "stdbool.h"

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
    u_int64_t startTableOffset;
};

struct defineTablesBlock {
    u_int32_t countTables;
    struct tableOffsetBlock tableOffsetBlock[MAX_TABLES];
    u_int64_t emptySpaceOffset;
};

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

#define LAB1_BLOCKS_H

#endif //LAB1_BLOCKS_H
