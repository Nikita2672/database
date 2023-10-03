#include "../file/fileApi.h"
#include "stdio.h"
#include "../file/tableBlocks.h"
#include <string.h>

#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"

void assertEquals(const uint64_t found, const uint64_t expected, char *fieldName, uint8_t testNumber, uint8_t position) {
    if (found != expected) printf("%u.%u: %s don't equals, expected %lu, but found %lu\n",
                                  testNumber, position, fieldName, expected, found);
}

void assertEqualsS(const char *found, const char *expected, char *fieldName, uint8_t testNumber, uint8_t position) {
    if (strcmp(found, expected) != 0) printf("%u.%u: %s don't equals, expected %s, but found %s\n",
                                  testNumber, position, fieldName, expected, found);
}

// Тест проверяет запись в файл отступа и количества таблиц
void test1() {
    writeEmptyTablesBlock(FILE_NAME);
    assertEquals(readEmptySpaceOffset(FILE_NAME), sizeof (struct defineTablesBlock), "EmptySpaceOffset", 1, 1);
    assertEquals(readTablesCount(FILE_NAME), 0, "table count", 1, 2);
    writeTableCount(FILE_NAME, 3);
    assertEquals(readEmptySpaceOffset(FILE_NAME), sizeof (struct defineTablesBlock), "EmptySpaceOffset", 1, 3);
    assertEquals(readTablesCount(FILE_NAME), 3, "table count", 1, 4);
    u_int64_t offset = readEmptySpaceOffset(FILE_NAME);
    writeEmptySpaceOffset(FILE_NAME, offset + 34);
    assertEquals(readEmptySpaceOffset(FILE_NAME), sizeof (struct defineTablesBlock) + 34, "EmptySpaceOffset", 1, 5);
    assertEquals(readTablesCount(FILE_NAME), 3, "table count", 1, 6);
}


void test2() {
    writeEmptyTablesBlock(FILE_NAME);
    struct NameTypeBlock* nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock* nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    struct NameTypeBlock* nameTypeBlock3 = initNameTypeBlock("Age", INT);
    struct NameTypeBlock* nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    struct NameTypeBlock* nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);
    // 1 table
    struct NameTypeBlock nameTypeBlocks1[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    struct tableOffsetBlock* writtenTableOffsetBlock1 = initTableOffsetBlock("User", 123, 321, 5, nameTypeBlocks1);
    writeTableOffsetBlock(FILE_NAME, writtenTableOffsetBlock1);
    struct tableOffsetBlock* tableOffsetBlock1 = readTableOffsetBlock(FILE_NAME, 0);
    assertEquals(tableOffsetBlock1->fieldsNumber, 5, "fieldsNumber", 2, 1);
    assertEqualsS(tableOffsetBlock1->tableName, "User", "tableName", 2, 2);
    assertEquals(tableOffsetBlock1->lastTableBLockOffset, 321, "lastTableOffsetBlock", 2, 3);
    assertEquals(tableOffsetBlock1->firsTableBlockOffset, 123, "firstTableOffsetBlock", 2, 4);
    assertEqualsS(tableOffsetBlock1->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 5);
    // 2 table
    struct NameTypeBlock nameTypeBlocks2[3] = {
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4
    };
    struct tableOffsetBlock* writtenTableOffsetBlock2 = initTableOffsetBlock("Cake", 234, 456, 3, nameTypeBlocks2);
    writeTableOffsetBlock(FILE_NAME, writtenTableOffsetBlock2);
    struct tableOffsetBlock* tableOffsetBlock2 = readTableOffsetBlock(FILE_NAME, 1);
    assertEquals(tableOffsetBlock2->fieldsNumber, 3, "fieldsNumber", 2, 6);
    assertEqualsS(tableOffsetBlock2->tableName, "Cake", "tableName", 2, 7);
    assertEquals(tableOffsetBlock2->lastTableBLockOffset, 456, "lastTableOffsetBlock", 2, 8);
    assertEquals(tableOffsetBlock2->firsTableBlockOffset, 234, "firstTableOffsetBlock", 2, 9);
    assertEqualsS(tableOffsetBlock2->nameTypeBlock[0].fieldName, "Surname", "Name 1 field", 2, 10);

    // check if table 2 overwritten table 1
    struct tableOffsetBlock* tableOffsetBlock3 = readTableOffsetBlock(FILE_NAME, 0);
    assertEquals(tableOffsetBlock3->fieldsNumber, 5, "fieldsNumber", 2, 1);
    assertEqualsS(tableOffsetBlock3->tableName, "User", "tableName", 2, 2);
    assertEquals(tableOffsetBlock3->lastTableBLockOffset, 321, "lastTableOffsetBlock", 2, 3);
    assertEquals(tableOffsetBlock3->firsTableBlockOffset, 123, "firstTableOffsetBlock", 2, 4);
    assertEqualsS(tableOffsetBlock3->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 5);

    free(tableOffsetBlock1);
    free(tableOffsetBlock2);
    free(tableOffsetBlock3);
    free(writtenTableOffsetBlock1);
    free(writtenTableOffsetBlock2);
    free(nameTypeBlock1);
    free(nameTypeBlock2);
    free(nameTypeBlock3);
    free(nameTypeBlock4);
    free(nameTypeBlock5);
}


void test3() {

}
