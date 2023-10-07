#include "../file/fileApi.h"
#include "stdio.h"
#include "../file/tableBlocks.h"
#include "tests.h"

#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"

int main() {
    test1();
    test2();
    test3();
    test4();
//    struct tableOffsetBlock* tableOffsetBlock = readTableOffsetBlock(FILE_NAME, 0);
//    if(true == tableOffsetBlock->isActive) printf("%s", "hello");
//    if(false == tableOffsetBlock->isActive) printf("%s", "hello1");
    return 0;
}

