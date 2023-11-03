#include "../file/fileApi.h"
#include "stdio.h"
#include "../file/tableBlocks.h"
#include <string.h>
#include "tests.h"
#include "../query/query.h"
#include "../file/iterator.h"
#include "../util/util.h"
#include <fcntl.h>
#include <unistd.h>

#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"
#define FILE_NAME_1 "/home/iwaa0303/CLionProjects/lab1/test/testInsert.bin"
#define FILE_NAME_2 "/home/iwaa0303/CLionProjects/lab1/test/data.bin"

#define BLOCK_SPACE sizeof (struct headerSection) + BLOCK_DATA_SIZE + sizeof (struct specialDataSection)

void assertEquals(const double found, const double expected, char *fieldName, uint8_t testNumber, uint8_t position) {
    if (found != expected)
        printf("%u.%u: %s don't equals, expected %f, but found %f\n",
               testNumber, position, fieldName, expected, found);
}

void assertEqualsS(const char *found, const char *expected, char *fieldName, uint8_t testNumber, uint8_t position) {
    if (strcmp(found, expected) != 0)
        printf("%u.%u: %s don't equals, expected %s, but found %s\n",
               testNumber, position, fieldName, expected, found);
}

// Тест проверяет запись в файл отступа и количества таблиц
// Проверено
void test1() {
    FILE *file = fopen(FILE_NAME, "rb+");
    writeEmptyTablesBlock(file);
    assertEquals(readEmptySpaceOffset(file), sizeof(struct defineTablesBlock) + BLOCK_SPACE, "EmptySpaceOffset", 1, 1);
    assertEquals(readTablesCount(file), 1, "table count", 1, 2);
    writeTableCount(file, 3);
    assertEquals(readEmptySpaceOffset(file), sizeof(struct defineTablesBlock) + BLOCK_SPACE, "EmptySpaceOffset", 1, 3);
    assertEquals(readTablesCount(file), 3, "table count", 1, 4);
    assertEquals(readTablesCount(file), 3, "table count", 1, 6);
    fclose(file);
}

// Проверено
void test2() {
    FILE *file = fopen(FILE_NAME, "rb+");
    struct NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    struct NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    struct NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    struct NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);

    // 1 table
    struct NameTypeBlock nameTypeBlocks1[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    struct tableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "User", 5, nameTypeBlocks1);
    writeTableOffsetBlock(file, writtenTableOffsetBlock1);
    struct tableOffsetBlock *tableOffsetBlock1 = readTableOffsetBlock(file, 1);

    assertEquals(tableOffsetBlock1->fieldsNumber, 5, "fieldsNumber", 2, 1);
    assertEqualsS(tableOffsetBlock1->tableName, "User", "tableName", 2, 2);
    assertEquals(tableOffsetBlock1->lastTableBLockOffset, sizeof(struct defineTablesBlock) + BLOCK_SPACE,
                 "lastTableOffsetBlock", 2, 3);
    assertEquals(tableOffsetBlock1->firsTableBlockOffset, sizeof(struct defineTablesBlock) + BLOCK_SPACE,
                 "firstTableOffsetBlock", 2, 4);
    assertEqualsS(tableOffsetBlock1->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 5);

    // 2 tables
    struct NameTypeBlock nameTypeBlocks2[3] = {
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4
    };
    struct tableOffsetBlock *writtenTableOffsetBlock2 = initTableOffsetBlock(file, "Cake", 3, nameTypeBlocks2);
    writeTableOffsetBlock(file, writtenTableOffsetBlock2);
    struct tableOffsetBlock *tableOffsetBlock2 = readTableOffsetBlock(file, 2);

    assertEquals(tableOffsetBlock2->fieldsNumber, 3, "fieldsNumber", 2, 6);
    assertEqualsS(tableOffsetBlock2->tableName, "Cake", "tableName", 2, 7);
    uint64_t expectedOffset = sizeof(struct defineTablesBlock) + sizeof(struct headerSection) + BLOCK_DATA_SIZE +
                              sizeof(struct specialDataSection);
    assertEquals(tableOffsetBlock2->lastTableBLockOffset, expectedOffset + BLOCK_SPACE, "lastTableOffsetBlock", 2, 8);
    assertEquals(tableOffsetBlock2->firsTableBlockOffset, expectedOffset + BLOCK_SPACE, "firstTableOffsetBlock", 2, 9);
    assertEqualsS(tableOffsetBlock2->nameTypeBlock[0].fieldName, "Surname", "Name 1 field", 2, 10);

    // check if table 2 overwritten table 1
    struct tableOffsetBlock *tableOffsetBlock3 = readTableOffsetBlock(file, 1);

    assertEquals(tableOffsetBlock3->fieldsNumber, 5, "fieldsNumber", 2, 11);
    assertEqualsS(tableOffsetBlock3->tableName, "User", "tableName", 2, 12);
    assertEquals(tableOffsetBlock3->lastTableBLockOffset, sizeof(struct defineTablesBlock) + BLOCK_SPACE,
                 "lastTableOffsetBlock", 2, 13);
    assertEquals(tableOffsetBlock3->firsTableBlockOffset, sizeof(struct defineTablesBlock) + BLOCK_SPACE,
                 "firstTableOffsetBlock", 2, 14);
    assertEqualsS(tableOffsetBlock3->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 15);

    free(nameTypeBlock1);
    free(nameTypeBlock2);
    free(nameTypeBlock3);
    free(nameTypeBlock4);
    free(nameTypeBlock5);

    free(writtenTableOffsetBlock1);
    free(writtenTableOffsetBlock2);

    free(tableOffsetBlock1);
    free(tableOffsetBlock2);
    free(tableOffsetBlock3);

    fclose(file);
}

void test3() {
    FILE *file = fopen(FILE_NAME_1, "rb+");

    // test read write 1 record
    struct headerSection headerSection = {0, 0, BLOCK_DATA_SIZE, 0};
    double score = 1234.4;
    char *name = "Nikita";
    bool sex = true;
    int32_t age = 20;
    struct FieldValue fieldValue1 = {&score, sizeof(double)};
    struct FieldValue fieldValue2 = {name, sizeof(char) * strlen(name)};
    struct FieldValue fieldValue3 = {&sex, sizeof(bool)};
    struct FieldValue fieldValue4 = {&age, sizeof(int32_t)};
    struct FieldValue array[4] = {fieldValue1, fieldValue2, fieldValue3, fieldValue4};
    struct EntityRecord entityRecord = {array};

    struct NameTypeBlock nameTypeBlock1 = {"score", DOUBLE};
    struct NameTypeBlock nameTypeBlock2 = {"name", STRING};
    struct NameTypeBlock nameTypeBlock3 = {"sex", BOOL};
    struct NameTypeBlock nameTypeBlock4 = {"age", INT};
    struct tableOffsetBlock tableOffsetBlock = {true, "Users",
                                                {nameTypeBlock1, nameTypeBlock2, nameTypeBlock3, nameTypeBlock4}, 4, 0,
                                                0};
    fseek(file, 0, SEEK_SET);
    fwrite(&headerSection, sizeof(struct headerSection), 1, file);
    insertRecord(file, &entityRecord, &tableOffsetBlock);
    struct EntityRecord *entityRecord1 = readRecord(file, 0, 0, 4);
    assertEquals(*(double *) entityRecord1->fields[0].data, 1234.4, "value1", 3, 2);
    assertEqualsS(entityRecord1->fields[1].data, "Nikita", "value2", 3, 4);
    assertEquals(*(bool *) entityRecord1->fields[2].data, 1, "value3", 3, 6);
    assertEquals(*(int32_t *) entityRecord1->fields[3].data, 20, "value4", 3, 8);
    //------------------------------------------------------------------------------------------------------------------------
    // test writing the second record and don't break anything
    struct headerSection headerSection1;
    fseek(file, 0, SEEK_SET);
    double score1 = 123.3;
    char *name1 = "Ksenia Kirillova";
    bool sex1 = false;
    int32_t age1 = 19;
    struct FieldValue fieldValue21 = {&score1, sizeof(double)};
    struct FieldValue fieldValue22 = {name1, sizeof(char) * strlen(name1)};
    struct FieldValue fieldValue23 = {&sex1, sizeof(bool)};
    struct FieldValue fieldValue24 = {&age1, sizeof(int32_t)};
    struct FieldValue array1[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    struct EntityRecord entityRecord2 = {array1};
    insertRecord(file, &entityRecord2, &tableOffsetBlock);
    struct EntityRecord *entityRecord12 = readRecord(file, 0, 0, 4);
    assertEquals(*(double *) entityRecord12->fields[0].data, 1234.4, "value1", 3, 10);
    assertEqualsS(entityRecord12->fields[1].data, "Nikita", "value2", 3, 12);
    assertEquals(*(bool *) entityRecord12->fields[2].data, 1, "value3", 3, 14);
    assertEquals(*(int32_t *) entityRecord12->fields[3].data, 20, "value4", 3, 16);
    struct EntityRecord *entityRecord22 = readRecord(file, 1, 0, 4);
    assertEquals(*(double *) entityRecord22->fields[0].data, 123.3, "value1", 3, 18);
    assertEqualsS(entityRecord22->fields[1].data, "Ksenia Kirillova", "value2", 3, 20);
    assertEquals(*(bool *) entityRecord22->fields[2].data, 0, "value3", 3, 22);
    assertEquals(*(uint16_t *) entityRecord22->fields[3].data, 19, "value4", 3, 24);
    fseek(file, 0, SEEK_SET);
    fread(&headerSection1, sizeof(struct headerSection), 1, file);
    assertEquals(headerSection1.recordsNumber, 2, "recordsNumber", 3, 25);
    assertEquals(headerSection1.pageNumber, 0, "pageNUmber", 3, 26);
    assertEquals(headerSection1.endEmptySpaceOffset, BLOCK_DATA_SIZE - sizeof(struct recordId) * 2,
                 "endEmptySpaceOffset", 3, 27);
    fclose(file);
}

// test checkPredicate function
void test4() {
    struct NameTypeBlock nameTypeBlock1 = {"score", DOUBLE};
    struct NameTypeBlock nameTypeBlock2 = {"name", STRING};
    struct NameTypeBlock nameTypeBlock3 = {"sex", BOOL};
    struct NameTypeBlock nameTypeBlock4 = {"age", INT};
    struct NameTypeBlock nameTypeBlock[4] = {nameTypeBlock1, nameTypeBlock2, nameTypeBlock3, nameTypeBlock4};
    struct predicate predicate;
    double score1 = 123.3;
    char *name1 = "Ksenia Kirillova";
    bool sex1 = false;
    int32_t age1 = 19;
    struct FieldValue fieldValue21 = {&score1, sizeof(double)};
    struct FieldValue fieldValue22 = {name1, sizeof(char) * strlen(name1)};
    struct FieldValue fieldValue23 = {&sex1, sizeof(bool)};
    struct FieldValue fieldValue24 = {&age1, sizeof(int32_t)};
    struct FieldValue array1[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    struct EntityRecord entityRecord2 = {array1};
    predicate.fieldName = "sex";
    predicate.comparator = MORE_OR_EQUALS;
    bool sex2 = true;
    struct FieldValue comparableValue = {&sex2, sizeof(bool)};
    predicate.comparableValue = &comparableValue;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "sex", 4, 1);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "sex", 4, 2);
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "sex", 4, 3);
    predicate.fieldName = "score";
    double score2 = 123.31;
    struct FieldValue comparableValue1 = {&score2, sizeof(double)};
    predicate.comparableValue = &comparableValue1;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "score", 4, 4);
    predicate.comparator = MORE_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "score", 4, 5);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "score", 4, 6);
    double score3 = 123.3;
    predicate.comparableValue->data = &score3;
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "score", 4, 7);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "score", 4, 8);
    predicate.comparator = LESS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "score", 4, 9);
    char *name2 = "Ksenia Kirillova";
    struct FieldValue comparableValue2 = {name2, sizeof(char) * strlen(name2)};
    predicate.comparableValue = &comparableValue2;
    predicate.fieldName = "name";
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "name", 4, 10);
    predicate.comparator = MORE;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "name", 4, 11);
}

//check iterator
void test5() {
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 123.3;
    char *name = "Ksenia";
    char *surname = "Kirillova";
    bool sex = false;
    int32_t age = 19;
    struct FieldValue fieldValue1 = {name, sizeof(char) * strlen(name)};
    struct FieldValue fieldValue2 = {surname, sizeof(char) * strlen(surname)};
    struct FieldValue fieldValue3 = {&age, sizeof(int32_t)};
    struct FieldValue fieldValue4 = {&score, sizeof(double)};
    struct FieldValue fieldValue5 = {&sex, sizeof(bool)};
    struct FieldValue array1[5] = {fieldValue1, fieldValue2, fieldValue3, fieldValue4, fieldValue5};
    struct EntityRecord entityRecord = {array1};
    insertRecordIntoTable(file, &entityRecord, "User");

    double score2 = 128;
    char *name2 = "Nikita";
    char *surname2 = "Ivanov";
    bool sex2 = true;
    int32_t age2 = 20;
    struct FieldValue fieldValue21 = {name2, sizeof(char) * strlen(name2)};
    struct FieldValue fieldValue22 = {surname2, sizeof(char) * strlen(surname2)};
    struct FieldValue fieldValue23 = {&age2, sizeof(int32_t)};
    struct FieldValue fieldValue24 = {&score2, sizeof(double)};
    struct FieldValue fieldValue25 = {&sex2, sizeof(bool)};
    struct FieldValue array2[5] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24, fieldValue25};
    struct EntityRecord entityRecord2 = {array2};
    insertRecordIntoTable(file, &entityRecord2, "User");

    double score3 = 356;
    char *name3 = "Lubovv";
    char *surname3 = "Vitalievna";
    bool sex3 = false;
    int32_t age3 = 51;
    struct FieldValue fieldValue31 = {name3, sizeof(char) * strlen(name3)};
    struct FieldValue fieldValue32 = {surname3, sizeof(char) * strlen(surname3)};
    struct FieldValue fieldValue33 = {&age3, sizeof(int32_t)};
    struct FieldValue fieldValue34 = {&score3, sizeof(double)};
    struct FieldValue fieldValue35 = {&sex3, sizeof(bool)};
    struct FieldValue array3[5] = {fieldValue31, fieldValue32, fieldValue33, fieldValue34, fieldValue35};
    struct EntityRecord entityRecord3 = {array3};
    insertRecordIntoTable(file, &entityRecord3, "User");
    score++;
    insertRecordIntoTable(file, &entityRecord, "User");
    score++;
    insertRecordIntoTable(file, &entityRecord, "User");
    struct iterator *iterator = readEntityRecordWithCondition(file, "User", NULL, 0);
    bool isNext1 = hasNext(iterator, file);
    assertEquals(isNext1, true, "hasNext", 5, 1);
    struct EntityRecord *entityRecord1 = next(iterator, file);
    assertEqualsS((char *) entityRecord1->fields[0].data, "Ksenia", "name", 5, 3);
    assertEqualsS(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize), "Kirillova",
                  "surname", 5, 4);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 19, "age", 5, 6);
    assertEquals(*(double *) entityRecord1->fields[3].data, 123.3, "score", 5, 2);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 5);

    bool isNext2 = hasNext(iterator, file);
    assertEquals(isNext2, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsS((char *) entityRecord1->fields[0].data, "Nikita", "name", 5, 8);
    assertEqualsS((char *) entityRecord1->fields[1].data, "Ivanov", "surname", 5, 9);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 20, "age", 5, 11);
    assertEquals(*(double *) entityRecord1->fields[3].data, 128, "score", 5, 7);
    assertEquals(*(bool *) entityRecord1->fields[4].data, true, "sex", 5, 10);

    bool isNext3 = hasNext(iterator, file);
    assertEquals(isNext3, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsS((char *) entityRecord1->fields[0].data, "Lubovv", "name", 5, 13);
    assertEqualsS(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize), "Vitalievna",
                  "surname", 5, 14);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 51, "age", 5, 16);
    assertEquals(*(double *) entityRecord1->fields[3].data, 356, "score", 5, 12);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 15);

    bool isNext4 = hasNext(iterator, file);
    assertEquals(isNext4, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsS((char *) entityRecord1->fields[0].data, "Ksenia", "name", 5, 18);
    assertEqualsS(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize), "Kirillova",
                  "surname", 5, 19);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 19, "age", 5, 21);
    assertEquals(*(double *) entityRecord1->fields[3].data, 124.3, "score", 5, 17);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 20);

    bool isNext5 = hasNext(iterator, file);
    assertEquals(isNext5, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsS((char *) entityRecord1->fields[0].data, "Ksenia", "name", 5, 23);
    assertEqualsS(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize), "Kirillova",
                  "surname", 5, 24);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 19, "age", 5, 26);
    assertEquals(*(double *) entityRecord1->fields[3].data, 125.3, "score", 5, 22);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 25);

    bool isNext6 = hasNext(iterator, file);
    assertEquals(isNext6, false, "hasNext", 5, 1);
    fclose(file);
}

//check iterator with predicates

void test6() {
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 356;
    int32_t age = 20;
    struct FieldValue fieldValue = {&score, sizeof(double)};
    struct FieldValue fieldValue1 = {&age, sizeof(int32_t)};
    struct predicate predicate[2] = {{&fieldValue,  "Score", LESS},
                                     {&fieldValue1, "Age",   MORE_OR_EQUALS}};
    struct iterator *iterator = readEntityRecordWithCondition(file, "User", predicate, 2);
    bool nextVal = hasNext(iterator, file);
    assertEquals(nextVal, true, "next", 6, 1);
    struct EntityRecord *entityRecord = next(iterator, file);
    assertEqualsS((char *) entityRecord->fields[0].data, "Nikita", "name", 6, 3);
    assertEqualsS((char *) entityRecord->fields[1].data, "Ivanov", "surname", 6, 4);
    assertEquals(*(uint16_t *) entityRecord->fields[2].data, 20, "age", 6, 6);
    assertEquals(*(double *) entityRecord->fields[3].data, 128, "score", 6, 2);
    assertEquals(*(bool *) entityRecord->fields[4].data, true, "sex", 6, 5);

    char *name = "Nikita";
    struct FieldValue fieldValue2 = {name, sizeof(char) * strlen(name)};
    struct predicate predicate1[1] = {&fieldValue2, "Name", EQUALS};
    struct iterator *iterator1 = readEntityRecordWithCondition(file, "User", predicate1, 1);
    bool nextVal1 = hasNext(iterator1, file);
    assertEquals(nextVal1, true, "next", 6, 7);
    struct EntityRecord *pEntityRecord = next(iterator1, file);
    assertEqualsS(pEntityRecord->fields[0].data, "Nikita", "name", 6, 8);
    fclose(file);
}

void testRebuild() {
    FILE *file = fopen(FILE_NAME_2, "rb+");
    struct recordId recordId1 = {0, 12};
    struct recordId recordId2 = {12, 13};
    struct recordId recordId3 = {25, 14};
    struct recordId recordId4 = {39, 15};
    struct recordId recordId5 = {54, 16};
    struct recordId recordId6 = {70, 17};
    struct recordId recordId7 = {87, 18};
    struct recordId recordIds[7] = {recordId7, recordId6, recordId5, recordId4, recordId3, recordId2, recordId1};
    struct headerSection headerSection = {0, 0, BLOCK_DATA_SIZE - sizeof(struct recordId) * 7, 7};
    struct specialDataSection specialDataSection = {0, 0};
    fwrite(&headerSection, sizeof(struct headerSection), 1, file);
    fflush(file);
    fseek(file, sizeof(struct headerSection) + BLOCK_DATA_SIZE - sizeof(struct recordId) * 7, SEEK_SET);
    fwrite(recordIds, sizeof(struct recordId) * 7, 1, file);
    fflush(file);
    fwrite(&specialDataSection, sizeof(struct specialDataSection), 1, file);
    fflush(file);
    unsigned char *buffer = malloc(sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection));
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection), 1, file);
    struct recordId *resultIds = malloc(sizeof(struct recordId) * 7);
    rebuildArrayOfRecordIds(buffer, resultIds, 7, 2, 14);
    assertEquals(resultIds[0].offset, 73, "offset", 7, 1);
    assertEquals(resultIds[0].length, 18, "length", 7, 2);
    assertEquals(resultIds[1].offset, 56, "offset", 7, 3);
    assertEquals(resultIds[1].length, 17, "length", 7, 4);
    assertEquals(resultIds[2].offset, 40, "offset", 7, 5);
    assertEquals(resultIds[2].length, 16, "length", 7, 6);
    assertEquals(resultIds[3].offset, 25, "offset", 7, 7);
    assertEquals(resultIds[3].length, 15, "length", 7, 8);
    assertEquals(resultIds[4].offset, 12, "offset", 7, 9);
    assertEquals(resultIds[4].length, 13, "length", 7, 10);
    assertEquals(resultIds[5].offset, 0, "offset", 7, 11);
    assertEquals(resultIds[5].length, 12, "length", 7, 12);
    free(buffer);
    free(resultIds);
    fclose(file);
}

void testRebuild1() {
    FILE *file = fopen(FILE_NAME_2, "rb+");
    struct recordId recordId1 = {0, 21};
    struct recordId recordIds[1] = {recordId1};
    struct headerSection headerSection = {0, 0, BLOCK_DATA_SIZE - sizeof(struct recordId) * 1, 2};
    struct specialDataSection specialDataSection = {0, 0};
    fwrite(&headerSection, sizeof(struct headerSection), 1, file);
    fflush(file);
    fseek(file, sizeof(struct headerSection) + BLOCK_DATA_SIZE - sizeof(struct recordId) * 1, SEEK_SET);
    fwrite(recordIds, sizeof(struct recordId) * 1, 1, file);
    fflush(file);
    fwrite(&specialDataSection, sizeof(struct specialDataSection), 1, file);
    fflush(file);
    unsigned char *buffer = malloc(sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection));
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection), 1, file);
    struct recordId *resultIds = malloc(sizeof(struct recordId) * 1);
    rebuildArrayOfRecordIds(buffer, resultIds, 1, 0, 21);
}

// deleteRecordTest
void test7() {
    // test rebuildArrayOfRecordIds function
    testRebuild();
    testRebuild1();
    struct headerSection headerSection = {0, 0, BLOCK_DATA_SIZE - sizeof(struct recordId) * 7, 7};
    //test deleteRecordFromTable function
    struct NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    struct NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    struct NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    struct NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);
    // 1 table
    struct NameTypeBlock nameTypeBlocks1[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };

    //delete 1 record
    const char *name = "Nikita";
    struct FieldValue fieldValue = {name, sizeof(char) * strlen(name)};
    FILE *file1 = fopen(FILE_NAME, "rb+");
    struct predicate predicate[1] = {&fieldValue, "Name", EQUALS};
    struct iterator *iterator = readEntityRecordWithCondition(file1, "User", predicate, 1);
    bool has = hasNext(iterator, file1);
    assertEquals(has, true, "hasNext", 7, 13);
    struct EntityRecord *entityRecord = next(iterator, file1);
    assertEqualsS(entityRecord->fields[0].data, "Nikita", "name", 7, 14);
    deleteRecordFromTable(file1, "User", predicate, 1);
    struct iterator *iterator1 = readEntityRecordWithCondition(file1, "User", predicate, 1);
    bool has1 = hasNext(iterator1, file1);
    assertEquals(has1, false, "hasNext", 7, 15);
    struct iterator *iterator2 = readEntityRecordWithCondition(file1, "User", NULL, 0);

    double array[] = {123.3, 356, 124.3, 125.3};
    uint8_t i = 0;
    while (hasNext(iterator2, file1)) {
        struct EntityRecord *entityRecord1 = next(iterator2, file1);
        assertEquals(*(double *) entityRecord1->fields[3].data, array[i], "score", 7, 16 + i);
        i++;
    }
    assertEquals(i, 4, "recordsNumber", 7, 21);
    const char *name1 = "Ksenia";
    struct FieldValue fieldValue1 = {name1, sizeof(char) * (strlen(name1) + 1)};
    struct predicate predicate1[1] = {&fieldValue1, "Name", EQUALS};
    struct iterator *iterator3 = readEntityRecordWithCondition(file1, "User", predicate1, 1);
    bool has2 = hasNext(iterator3, file1);
    assertEquals(has2, true, "hasNext", 7, 22);
    struct EntityRecord *entityRecord1 = next(iterator3, file1);
    assertEqualsS(entityRecord1->fields[0].data, "Ksenia", "surname", 7, 23);


    //delete several records in 1 operation
    double score = 140.3;
    struct FieldValue fieldValue2 = {&score, sizeof(double)};
    struct predicate predicate2[1] = {&fieldValue2, "Score", LESS};
    deleteRecordFromTable(file1, "User", predicate2, 1);
    struct iterator *iterator4 = readEntityRecordWithCondition(file1, "User", NULL, 0);
    score = 356;
    i = 0;
    while (hasNext(iterator4, file1)) {
        struct EntityRecord *entityRecord2 = next(iterator4, file1);
        assertEquals(*(double *) entityRecord2->fields[3].data, score, "score", 7, 24 + i);
        i++;
    }
    assertEquals(i, 1, "recordsNumber", 7, 25);


    fseek(file1, iterator->blockOffset, SEEK_SET);
    fread(&headerSection, sizeof(struct headerSection), 1, file1);
    assertEquals(headerSection.recordsNumber, 1, "recordsNumber", 7, 26);
    assertEquals(headerSection.endEmptySpaceOffset, BLOCK_DATA_SIZE - sizeof(struct recordId), "endEmptySpaceOffset", 7,
                 27);
    assertEquals(headerSection.pageNumber, 0, "pageNumber", 7, 28);
    struct recordId recordId;
    fseek(file1, iterator->blockOffset + sizeof(struct headerSection) + BLOCK_DATA_SIZE - sizeof(struct recordId),
          SEEK_SET);
    fread(&recordId, sizeof(struct recordId), 1, file1);
    assertEquals(recordId.offset, 0, "offset", 7, 29);
    fclose(file1);
}

void test8() {
    test1();
    test2();
    test5();
    test6();
    FILE *file = fopen(FILE_NAME, "rb+");
    struct iterator *iterator = readEntityRecordWithCondition(file, "User", NULL, 0);
    printf("\n");
    double score = 124.4;
    char *surname = "Kirillova";
    bool sex = false;
    struct FieldValue fieldValue = {&score, sizeof(double)};
    struct FieldValue fieldValue1 = {surname, sizeof(char) * strlen(surname)};
    struct FieldValue fieldValue2 = {&sex, sizeof(bool)};
    struct predicate predicate[2] = {{&fieldValue,  "Score", MORE},
                                     {&fieldValue2, "Sex",   EQUALS}};
    deleteRecordFromTable(file, "User", predicate, 2);
    struct iterator *iterator1 = readEntityRecordWithCondition(file, "User", NULL, 0);

    const char *surnames[3] = {"Kirillova", "Ivanov", "Kirillova"};
    const double scores[3] = {123.3, 128, 124.3};
    uint8_t i = 0;
    while (hasNext(iterator1, file)) {
        struct EntityRecord *entityRecord = next(iterator1, file);
        assertEquals(*(double *) entityRecord->fields[3].data, scores[i], "score", 8, i * 2 + 1);
        assertEqualsS(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize),
                      surnames[i], "surname", 8, i * 2 + 2);
        i++;
    }
    assertEquals(i, 3, "recordsNUmber", 8, 12);
    fclose(file);
}

// test update
void test9() {
    struct NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    struct NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    struct NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    struct NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);
    struct NameTypeBlock nameTypeBlocks[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    test1();
    test2();
    test5();
    test6();
    FILE *file = fopen(FILE_NAME, "rb+");
    struct iterator *iterator = readEntityRecordWithCondition(file, "User", NULL, 0);
    double score = 128;
    struct FieldValue fieldValue = {&score, sizeof(score)};
    struct predicate predicate[1] = {{&fieldValue, "Score", EQUALS}};

    double score2 = 140;
    char *name2 = "Nikita";
    char *surname2 = "Pesterev";
    bool sex2 = true;
    int32_t age2 = 20;
    struct FieldValue fieldValue21 = {name2, sizeof(char) * strlen(name2)};
    struct FieldValue fieldValue22 = {surname2, sizeof(char) * strlen(surname2)};
    struct FieldValue fieldValue23 = {&age2, sizeof(int32_t)};
    struct FieldValue fieldValue24 = {&score2, sizeof(double)};
    struct FieldValue fieldValue25 = {&sex2, sizeof(bool)};
    struct FieldValue array2[5] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24, fieldValue25};
    struct EntityRecord entityRecord2 = {array2};
    updateRecordFromTable(file, "User", predicate, 1, &entityRecord2);

    struct FieldValue fieldValue1 = {name2, sizeof(char) * strlen(name2)};
    struct predicate predicate1[1] = {&fieldValue1, "Name", EQUALS};
    struct iterator *iterator1 = readEntityRecordWithCondition(file, "User", predicate1, 1);
    bool nextVal = hasNext(iterator1, file);
    assertEquals(nextVal, true, "hasNext", 9, 1);
    struct EntityRecord *entityRecord = next(iterator1, file);
    assertEqualsS((char *) entityRecord->fields[0].data, "Nikita", "name", 9, 2);
    assertEqualsS(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize), "Pesterev",
                  "surname", 9, 3);
    assertEquals(*(uint16_t *) entityRecord->fields[2].data, 20, "age", 9, 4);
    assertEquals(*(double *) entityRecord->fields[3].data, 140, "score", 9, 5);
    assertEquals(*(bool *) entityRecord->fields[4].data, true, "sex", 9, 6);
    fclose(file);
}

// test join
void test10() {
    FILE *file = fopen(FILE_NAME, "rb+");
    struct NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    struct NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    struct NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("DepartmentId", INT);
    // 1 table
    struct NameTypeBlock nameTypeBlocks1[4] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4
    };
    struct tableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "Employee", 4, nameTypeBlocks1);
    writeTableOffsetBlock(file, writtenTableOffsetBlock1);

    struct NameTypeBlock *nameTypeBlock21 = initNameTypeBlock("DepartmentId", INT);
    struct NameTypeBlock *nameTypeBlock22 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock *nameTypeBlock23 = initNameTypeBlock("Description", STRING);
    // 2 table
    struct NameTypeBlock nameTypeBlocks2[3] = {
            *nameTypeBlock21,
            *nameTypeBlock22,
            *nameTypeBlock23
    };

    struct NameTypeBlock nameTypeBlocks3[7] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock21,
            *nameTypeBlock22,
            *nameTypeBlock23
    };

    struct tableOffsetBlock *writtenTableOffsetBlock2 = initTableOffsetBlock(file, "Department", 3, nameTypeBlocks2);
    writeTableOffsetBlock(file, writtenTableOffsetBlock2);

    char *name1 = "Nikita";
    char *surname1 = "Ivanov";
    int32_t age1 = 20;
    int32_t departmentId1 = 1;
    struct FieldValue fieldValue11 = {name1, sizeof(char) * strlen(name1)};
    struct FieldValue fieldValue12 = {surname1, sizeof(char) * strlen(name1)};
    struct FieldValue fieldValue13 = {&age1, sizeof(int32_t)};
    struct FieldValue fieldValue14 = {&departmentId1, sizeof(int32_t)};
    struct FieldValue array1[4] = {fieldValue11, fieldValue12, fieldValue13, fieldValue14};
    struct EntityRecord entityRecord1 = {array1};
    insertRecordIntoTable(file, &entityRecord1, "Employee");

    char *name2 = "Ivan";
    char *surname2 = "Bobrov";
    int32_t age2 = 21;
    int32_t departmentId2 = 2;
    struct FieldValue fieldValue21 = {name2, sizeof(char) * strlen(name2)};
    struct FieldValue fieldValue22 = {surname2, sizeof(char) * strlen(surname2)};
    struct FieldValue fieldValue23 = {&age2, sizeof(int32_t)};
    struct FieldValue fieldValue24 = {&departmentId2, sizeof(int32_t)};
    struct FieldValue array2[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    struct EntityRecord entityRecord2 = {array2};
    insertRecordIntoTable(file, &entityRecord2, "Employee");

    char *name3 = "Boris";
    char *surname3 = "Kirillov";
    int32_t age3 = 50;
    int32_t departmentId3 = 3;
    struct FieldValue fieldValue31 = {name3, sizeof(char) * strlen(name3)};
    struct FieldValue fieldValue32 = {surname3, sizeof(char) * strlen(surname3)};
    struct FieldValue fieldValue33 = {&age3, sizeof(int32_t)};
    struct FieldValue fieldValue34 = {&departmentId3, sizeof(int32_t)};
    struct FieldValue array3[4] = {fieldValue31, fieldValue32, fieldValue33, fieldValue34};
    struct EntityRecord entityRecord3 = {array3};
    insertRecordIntoTable(file, &entityRecord3, "Employee");


    char *name4 = "Entropy";
    char *description4 = "Command develop Accounting and Invoicing modules";
    struct FieldValue fieldValue41 = {&departmentId1, sizeof(int32_t)};
    struct FieldValue fieldValue42 = {name4, sizeof(char) * strlen(name4)};
    struct FieldValue fieldValue43 = {description4, sizeof(char) * strlen(description4)};
    struct FieldValue array4[3] = {fieldValue41, fieldValue42, fieldValue43};
    struct EntityRecord entityRecord4 = {array4};
    insertRecordIntoTable(file, &entityRecord4, "Department");

    char *name5 = "QuantumTeam";
    char *description5 = "Command develop loyality module";
    struct FieldValue fieldValue51 = {&departmentId2, sizeof(int32_t)};
    struct FieldValue fieldValue52 = {name5, sizeof(char) * strlen(name5)};
    struct FieldValue fieldValue53 = {description5, sizeof(char) * strlen(description5)};
    struct FieldValue array5[3] = {fieldValue51, fieldValue52, fieldValue53};
    struct EntityRecord entityRecord5 = {array5};
    insertRecordIntoTable(file, &entityRecord5, "Department");

    char *name6 = "BACKOFFICE";
    char *description6 = "Command develop backoffice module";
    struct FieldValue fieldValue61 = {&departmentId3, sizeof(int32_t)};
    struct FieldValue fieldValue62 = {name6, sizeof(char) * strlen(name6)};
    struct FieldValue fieldValue63 = {description6, sizeof(char) * strlen(description6)};
    struct FieldValue array6[3] = {fieldValue61, fieldValue62, fieldValue63};
    struct EntityRecord entityRecord6 = {array6};
    insertRecordIntoTable(file, &entityRecord6, "Department");

    struct iterator *joinIterator = readEntityRecordWithCondition(file, "Employee", NULL, 0);
    struct EntityRecord *entityRecord = nextWithJoin(joinIterator, "Department", file, 3, "DepartmentId");
    assertEqualsS((char *) entityRecord->fields[0].data, "Nikita", "Name", 10, 0);
    assertEqualsS((char *) entityRecord->fields[1].data, "Ivanov", "Surname", 10, 1);
    assertEquals(*(int32_t *) entityRecord->fields[2].data, 20, "Age", 10, 2);
    assertEquals(*(int32_t *) entityRecord->fields[3].data, 1, "DepartmentId", 10, 3);
    assertEquals(*(int32_t *) entityRecord->fields[4].data, 1, "DepartmentId", 10, 4);
    assertEqualsS((char *) entityRecord->fields[5].data, "Entropy", "Name", 10, 5);
    assertEqualsS((char *) entityRecord->fields[6].data, "Command develop Accounting and Invoicing modules",
                  "Description", 10, 6);


    entityRecord = nextWithJoin(joinIterator, "Department", file, 3, "DepartmentId");
    assertEqualsS(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Ivan", "Name",
                  10, 7);
    assertEqualsS((char *) entityRecord->fields[1].data, "Bobrov", "Surname", 10, 8);
    assertEquals(*(int32_t *) entityRecord->fields[2].data, 21, "Age", 10, 9);
    assertEquals(*(int32_t *) entityRecord->fields[3].data, 2, "DepartmentId", 10, 10);
    assertEquals(*(int32_t *) entityRecord->fields[4].data, 2, "DepartmentId", 10, 11);
    assertEqualsS(cutString((char *) entityRecord->fields[5].data, 0, entityRecord->fields[5].dataSize), "QuantumTeam",
                  "Name", 10, 12);
    assertEqualsS((char *) entityRecord->fields[6].data, "Command develop loyality module", "Description", 10, 13);

    entityRecord = nextWithJoin(joinIterator, "Department", file, 3, "DepartmentId");
    assertEqualsS(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Boris",
                  "Name", 10, 14);
    assertEqualsS(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize), "Kirillov",
                  "Surname", 10, 15);
    assertEquals(*(int32_t *) entityRecord->fields[2].data, 50, "Age", 10, 16);
    assertEquals(*(int32_t *) entityRecord->fields[3].data, 3, "DepartmentId", 10, 17);
    assertEquals(*(int32_t *) entityRecord->fields[4].data, 3, "DepartmentId", 10, 18);
    assertEqualsS(cutString((char *) entityRecord->fields[5].data, 0, entityRecord->fields[5].dataSize), "BACKOFFICE",
                  "Name", 10, 19);
    assertEqualsS((char *) entityRecord->fields[6].data, "Command develop backoffice module", "Description", 10, 20);
    fclose(file);
}

// check memory in file
void test11() {
    FILE *file = fopen(FILE_NAME, "rb+");
    deleteTable("User", file);
    struct NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    struct NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    struct NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    struct NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    struct NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);

    // 1 table
    struct NameTypeBlock nameTypeBlocks1[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    uint64_t offset = readEmptySpaceOffset(file);
    uint64_t fileSize = getFileSize(file);
    uint32_t tableCount = readTablesCount(file);
    for (uint16_t i = 0; i < 1000; i++) {
        struct tableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "Users1", 5, nameTypeBlocks1);
        writeTableOffsetBlock(file, writtenTableOffsetBlock1);
        deleteTable("Users1", file);
    }
    assertEquals(readEmptySpaceOffset(file), offset, "offset", 11, 0);
    assertEquals(getFileSize(file), fileSize, "fileSize", 11, 1);
    assertEquals(readTablesCount(file), tableCount, "tableCount", 11, 2);
    uint8_t tablesToInsert = 5;
    for (uint16_t i = 0; i < tablesToInsert; i++) {
        writeTableOffsetBlock(file, initTableOffsetBlock(file, "Users1", 5, nameTypeBlocks1));
    }
    assertEquals(readTablesCount(file), tableCount + tablesToInsert, "tableCount", 11, 3);
    assertEquals(readEmptySpaceOffset(file), offset + (BLOCK_SPACE) * (tablesToInsert - 1), "offset", 11, 4);
    offset = readEmptySpaceOffset(file);
    ftruncate(fileno(file), offset + 1);
    printf("\nfileSize: %lu", getFileSize(file));
    fclose(file);
}


void test12() {

    struct NameTypeBlock nameTypeBlocks[4] = {{"Age",         INT},
                                              {"Sex",         BOOL},
                                              {"Description", STRING},
                                              {"Score",       DOUBLE}};
    printf("\n");
    for (uint16_t i = 50; i < 100; i++) {
        struct EntityRecord *entityRecordTest = malloc(sizeof(struct EntityRecord));

        int32_t *ageTest = malloc(sizeof(int32_t));
        *ageTest = 32;

        bool *sexTest = malloc(sizeof(bool));
        *sexTest = false;

        char *descriptionTest = malloc(sizeof(char) *
                                       strlen("Если у нас тут прям болталка, то у меня такие новости: я вот приехала домой, родителей не видела 3 месяца)"));
        strncpy(descriptionTest, "Если у нас тут прям болталка, то у меня такие новости: я вот приехала домой, родителей не видела 3 месяца)", sizeof (char ) *
                strlen("Если у нас тут прям болталка, то у меня такие новости: я вот приехала домой, родителей не видела 3 месяца)"));

        double *scoreTest = malloc(sizeof(double));
        *scoreTest = 123.324;

        struct FieldValue *fieldValueTest = malloc(sizeof(struct FieldValue) * 4);

        fieldValueTest[0].data = ageTest;
        fieldValueTest[0].dataSize = sizeof(int32_t);
        fieldValueTest[1].data = sexTest;
        fieldValueTest[1].dataSize = sizeof(bool);
        fieldValueTest[2].data = descriptionTest;
        fieldValueTest[2].dataSize = (sizeof(char) * strlen(descriptionTest));
        fieldValueTest[3].data = scoreTest;
        fieldValueTest[3].dataSize = sizeof(double);

        entityRecordTest->fields = fieldValueTest;

        struct EntityRecord **entities = separateEntityRecord(entityRecordTest, i, 4, nameTypeBlocks);
        struct EntityRecord *entityRecordCompound = compoundEntityRecords(entities[0], entities[1], 4);
        assertEquals(*(int32_t *) entityRecordCompound->fields[0].data, *ageTest, "age", 12,4 * i);
        assertEquals(*(bool *) entityRecordCompound->fields[1].data, false, "sexTest", 12, 4 * i + 1);
        assertEqualsS(cutString((char *) entityRecordCompound->fields[2].data, 0, entityRecordCompound->fields[2].dataSize), descriptionTest, "descriptionTest", 12,4 * i + 2);
        assertEquals(*(double *) entityRecordCompound->fields[3].data, 123.324, "scoreTest", 12, 4 * i + 3);
        if (entityRecordCompound->fields[0].data != NULL) free(entityRecordCompound->fields[0].data);
        if (entityRecordCompound->fields[1].data != NULL) free(entityRecordCompound->fields[1].data);
        if (entityRecordCompound->fields[2].data != NULL) free(entityRecordCompound->fields[2].data);
        if (entityRecordCompound->fields[3].data != NULL) free(entityRecordCompound->fields[3].data);
        free(entityRecordCompound->fields);
        free(entityRecordCompound);
        free(entities);
    }
}
