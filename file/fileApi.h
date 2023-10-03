#ifndef LAB1_FILEAPI_H
#define LAB1_FILEAPI_H

#include <stdlib.h>
#include <bits/types/FILE.h>

void writeEmptyTablesBlock(const char *name);
struct defineTablesBlock* readTablesBlock(const char *name);
u_int32_t readTablesCount(const char *name);
u_int64_t readEmptySpaceOffset(const char *name);
void writeTableCount(const char *name, u_int32_t tablesCount);
void writeEmptySpaceOffset(const char *name, u_int64_t offset);
struct tableOffsetBlock* readTableOffsetBlock(const char *name, u_int16_t tablePosition);
u_int64_t findOffsetForTableOffsetBlock(FILE *file);
void writeTableOffsetBlock(const char *name, struct tableOffsetBlock * tableOffsetBlock);
#endif //LAB1_FILEAPI_H
