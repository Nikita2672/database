#include "../file/fileApi.h"
#include "stdio.h"
#include "../file/blocks.h"


#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"

int main() {
//    writeEmptyTablesBlock(FILE_NAME);
    struct defineTablesBlock* defineTablesBlock = readTablesBlock(FILE_NAME);
    printf("%lu\n", defineTablesBlock->emptySpaceOffset);
    printf("%lu\n", sizeof (struct defineTablesBlock));
//    u_int32_t tablesCount = readTablesCount(FILE_NAME);
//    printf("%u\n", tablesCount);
//    u_int64_t emptySpaceOffset = readEmptySpaceOffset(FILE_NAME);
//    printf("%lu\n", emptySpaceOffset);
//    writeTableCount(FILE_NAME, 3);
//    writeTableCount(FILE_NAME, 3);
//    printf("%u\n", readTablesCount(FILE_NAME));
//    printf("%lu\n", readEmptySpaceOffset(FILE_NAME));
    return 0;
}

