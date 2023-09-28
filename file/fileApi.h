#ifndef LAB1_FILEAPI_H
#define LAB1_FILEAPI_H

#include <stdlib.h>

void writeEmptyTablesBlock(const char *name);
struct defineTablesBlock* readTablesBlock(const char *name);
u_int32_t readTablesCount(const char *name);
u_int64_t readEmptySpaceOffset(const char *name);
void writeTableCount(const char *name, u_int32_t tablesCount);
#endif //LAB1_FILEAPI_H
