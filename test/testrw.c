#include "../file/fileApi.h"
#include "stdio.h"
#include "../file/blocks.h"


#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"

int main() {
    writeEmptyTablesBlock(FILE_NAME);
    printf("written emptySpaceOffset: %lu\n", readEmptySpaceOffset(FILE_NAME));
    printf("sizeof defineTablesBlock: %lu\n", sizeof (struct defineTablesBlock));
    printf("written table count: %u\n", readTablesCount(FILE_NAME));
    writeTableCount(FILE_NAME, 3);
    printf("written emptySpaceOffset: %lu\n", readEmptySpaceOffset(FILE_NAME));
    printf("written table count: %u\n", readTablesCount(FILE_NAME));
    return 0;
}

