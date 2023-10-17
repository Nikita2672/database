#ifndef LAB1_FILEAPI_H
#define LAB1_FILEAPI_H

#include <stdlib.h>
#include <bits/types/FILE.h>
#include "../data/data.h"
#include "dataBlocks.h"

void writeEmptyTablesBlock(const char *name);
struct defineTablesBlock* readTablesBlock(const char *name);
uint32_t readTablesCount(const char *name);
uint64_t readEmptySpaceOffset(const char *name);
void writeTableCount(const char *name, uint32_t tablesCount);
void writeEmptySpaceOffset(const char *name, uint64_t offset);
struct tableOffsetBlock* readTableOffsetBlock(const char *name, uint16_t tablePosition);
uint64_t findOffsetForTableOffsetBlock(FILE *file);
void writeTableOffsetBlock(const char *name, struct tableOffsetBlock * tableOffsetBlock);
void insertRecord(const char* fileName, struct EntityRecord* entityRecord, struct tableOffsetBlock* tableOffsetBlock);
struct EntityRecord *readRecord(FILE * file, uint16_t idPosition, uint64_t offset, uint16_t fieldsNumber);
#endif //LAB1_FILEAPI_H
