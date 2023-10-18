#ifndef LAB1_FILEAPI_H
#define LAB1_FILEAPI_H

#include <stdlib.h>
#include <bits/types/FILE.h>
#include "../data/data.h"
#include "dataBlocks.h"

void writeEmptyTablesBlock(FILE *file);

struct defineTablesBlock *readTablesBlock(FILE *file);

uint32_t readTablesCount(FILE *file);

uint64_t readEmptySpaceOffset(FILE *file);

void writeTableCount(FILE *file, uint32_t tablesCount);

void writeEmptySpaceOffset(FILE *file, uint64_t offset);

struct tableOffsetBlock *readTableOffsetBlock(FILE *file, uint16_t tablePosition);

uint64_t findOffsetForTableOffsetBlock(FILE *file);

void writeTableOffsetBlock(FILE *file, struct tableOffsetBlock *tableOffsetBlock);

void insertRecord(FILE *file, struct EntityRecord *entityRecord, struct tableOffsetBlock *tableOffsetBlock);

struct EntityRecord *readRecord(FILE *file, uint16_t idPosition, uint64_t offset, uint16_t fieldsNumber);

void insertRecordIntoTable(FILE* file, struct EntityRecord *entityRecord, const char *tableName);

#endif //LAB1_FILEAPI_H
