#include "../file/fileApi.h"
#include "stdio.h"
#include "../file/tableBlocks.h"
#include <string.h>
#include "tests.h"
#include "../query/query.h"
#include "../file/iterator.h"

#define FILE_NAME "/home/iwaa0303/CLionProjects/lab1/test/file.bin"
#define FILE_NAME_1 "/home/iwaa0303/CLionProjects/lab1/test/testInsert.bin"

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
void test1() {
    FILE *file = fopen(FILE_NAME, "rb+");
    writeEmptyTablesBlock(file);
    assertEquals(readEmptySpaceOffset(file), sizeof(struct defineTablesBlock), "EmptySpaceOffset", 1, 1);
    assertEquals(readTablesCount(file), 0, "table count", 1, 2);
    writeTableCount(file, 3);
    assertEquals(readEmptySpaceOffset(file), sizeof(struct defineTablesBlock), "EmptySpaceOffset", 1, 3);
    assertEquals(readTablesCount(file), 3, "table count", 1, 4);
    uint64_t offset = readEmptySpaceOffset(file);
    writeEmptySpaceOffset(file, offset + 34);
    assertEquals(readEmptySpaceOffset(file), sizeof(struct defineTablesBlock) + 34, "EmptySpaceOffset", 1, 5);
    assertEquals(readTablesCount(file), 3, "table count", 1, 6);
    fclose(file);
}


void test2() {
    FILE *file = fopen(FILE_NAME, "rb+");
    writeEmptyTablesBlock(file);
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
    struct tableOffsetBlock *tableOffsetBlock1 = readTableOffsetBlock(file, 0);
    assertEquals(tableOffsetBlock1->fieldsNumber, 5, "fieldsNumber", 2, 1);
    assertEqualsS(tableOffsetBlock1->tableName, "User", "tableName", 2, 2);
    assertEquals(tableOffsetBlock1->lastTableBLockOffset, sizeof(struct defineTablesBlock), "lastTableOffsetBlock", 2,
                 3);
    assertEquals(tableOffsetBlock1->firsTableBlockOffset, sizeof(struct defineTablesBlock), "firstTableOffsetBlock", 2,
                 4);
    assertEqualsS(tableOffsetBlock1->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 5);
    // 2 table
    struct NameTypeBlock nameTypeBlocks2[3] = {
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4
    };
    struct tableOffsetBlock *writtenTableOffsetBlock2 = initTableOffsetBlock(file, "Cake", 3, nameTypeBlocks2);
    writeTableOffsetBlock(file, writtenTableOffsetBlock2);
    struct tableOffsetBlock *tableOffsetBlock2 = readTableOffsetBlock(file, 1);
    assertEquals(tableOffsetBlock2->fieldsNumber, 3, "fieldsNumber", 2, 6);
    assertEqualsS(tableOffsetBlock2->tableName, "Cake", "tableName", 2, 7);
    uint64_t expectedOffset = sizeof(struct defineTablesBlock) + sizeof(struct headerSection) + BLOCK_DATA_SIZE +
                              sizeof(struct specialDataSection);
    assertEquals(tableOffsetBlock2->lastTableBLockOffset, expectedOffset, "lastTableOffsetBlock", 2, 8);
    assertEquals(tableOffsetBlock2->firsTableBlockOffset, expectedOffset, "firstTableOffsetBlock", 2, 9);
    assertEqualsS(tableOffsetBlock2->nameTypeBlock[0].fieldName, "Surname", "Name 1 field", 2, 10);

    // check if table 2 overwritten table 1
    struct tableOffsetBlock *tableOffsetBlock3 = readTableOffsetBlock(file, 0);
    assertEquals(tableOffsetBlock3->fieldsNumber, 5, "fieldsNumber", 2, 11);
    assertEqualsS(tableOffsetBlock3->tableName, "User", "tableName", 2, 12);
    assertEquals(tableOffsetBlock3->lastTableBLockOffset, sizeof(struct defineTablesBlock), "lastTableOffsetBlock", 2,
                 13);
    assertEquals(tableOffsetBlock3->firsTableBlockOffset, sizeof(struct defineTablesBlock), "firstTableOffsetBlock", 2,
                 14);
    assertEqualsS(tableOffsetBlock3->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 15);

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
    fclose(file);
}

void test3() {
    FILE *file = fopen(FILE_NAME_1, "rb+");

    // test read write 1 record
    struct headerSection headerSection = {0, 0, BLOCK_DATA_SIZE, 0};
    double score = 1234.4;
    char *name = "Nikita";
    bool sex = true;
    uint16_t age = 20;
    struct FieldValue fieldValue1 = {DOUBLE, "score", &score, sizeof(double)};
    struct FieldValue fieldValue2 = {STRING, "name", name, sizeof(char) * strlen(name)};
    struct FieldValue fieldValue3 = {BOOL, "sex", &sex, sizeof(bool)};
    struct FieldValue fieldValue4 = {INT, "age", &age, sizeof(uint16_t)};
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
    assertEqualsS(entityRecord1->fields[0].fieldName, "score", "field1", 3, 1);
    assertEquals(*(double *) entityRecord1->fields[0].data, 1234.4, "value1", 3, 2);
    assertEqualsS(entityRecord1->fields[1].fieldName, "name", "field2", 3, 3);
    assertEqualsS(entityRecord1->fields[1].data, "Nikita", "value2", 3, 4);
    assertEqualsS(entityRecord1->fields[2].fieldName, "sex", "field3", 3, 5);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 1, "value3", 3, 6);
    assertEqualsS(entityRecord1->fields[3].fieldName, "age", "field4", 3, 7);
    assertEquals(*(uint16_t *) entityRecord1->fields[3].data, 20, "value4", 3, 8);
    //------------------------------------------------------------------------------------------------------------------------
    // test writing the second record and don't break anything
    struct headerSection headerSection1;
    fseek(file, 0, SEEK_SET);
    double score1 = 123.3;
    char *name1 = "Ksenia Kirillova";
    bool sex1 = false;
    uint16_t age1 = 19;
    struct FieldValue fieldValue21 = {DOUBLE, "score", &score1, sizeof(double)};
    struct FieldValue fieldValue22 = {STRING, "name", name1, sizeof(char) * strlen(name1)};
    struct FieldValue fieldValue23 = {BOOL, "sex", &sex1, sizeof(bool)};
    struct FieldValue fieldValue24 = {INT, "age", &age1, sizeof(uint16_t)};
    struct FieldValue array1[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    struct EntityRecord entityRecord2 = {array1};
    insertRecord(file, &entityRecord2, &tableOffsetBlock);
    struct EntityRecord *entityRecord12 = readRecord(file, 0, 0, 4);
    assertEqualsS(entityRecord12->fields[0].fieldName, "score", "field1", 3, 9);
    assertEquals(*(double *) entityRecord12->fields[0].data, 1234.4, "value1", 3, 10);
    assertEqualsS(entityRecord12->fields[1].fieldName, "name", "field2", 3, 11);
    assertEqualsS(entityRecord12->fields[1].data, "Nikita", "value2", 3, 12);
    assertEqualsS(entityRecord12->fields[2].fieldName, "sex", "field3", 3, 13);
    assertEquals(*(uint16_t *) entityRecord12->fields[2].data, 1, "value3", 3, 14);
    assertEqualsS(entityRecord12->fields[3].fieldName, "age", "field4", 3, 15);
    assertEquals(*(uint16_t *) entityRecord12->fields[3].data, 20, "value4", 3, 16);
    struct EntityRecord *entityRecord22 = readRecord(file, 1, 0, 4);
    assertEqualsS(entityRecord22->fields[0].fieldName, "score", "field1", 3, 17);
    assertEquals(*(double *) entityRecord22->fields[0].data, 123.3, "value1", 3, 18);
    assertEqualsS(entityRecord22->fields[1].fieldName, "name", "field2", 3, 19);
    assertEqualsS(entityRecord22->fields[1].data, "Ksenia Kirillova", "value2", 3, 20);
    assertEqualsS(entityRecord22->fields[2].fieldName, "sex", "field3", 3, 21);
    assertEquals(*(uint16_t *) entityRecord22->fields[2].data, 0, "value3", 3, 22);
    assertEqualsS(entityRecord22->fields[3].fieldName, "age", "field4", 3, 23);
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
    struct predicate predicate;
    double score1 = 123.3;
    char *name1 = "Ksenia Kirillova";
    bool sex1 = false;
    int32_t age1 = 19;
    struct FieldValue fieldValue21 = {DOUBLE, "score", &score1, sizeof(double)};
    struct FieldValue fieldValue22 = {STRING, "name", name1, sizeof(char) * strlen(name1)};
    struct FieldValue fieldValue23 = {BOOL, "sex", &sex1, sizeof(bool)};
    struct FieldValue fieldValue24 = {INT, "age", &age1, sizeof(int32_t)};
    struct FieldValue array1[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    struct EntityRecord entityRecord2 = {array1};
    predicate.fieldName = "sex";
    predicate.comparator = MORE_OR_EQUALS;
    bool sex2 = true;
    struct FieldValue comparableValue = {BOOL, "sex", &sex2, sizeof(bool)};
    predicate.comparableValue = &comparableValue;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), false, "sex", 4, 1);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), true, "sex", 4, 2);
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), false, "sex", 4, 3);
    predicate.fieldName = "score";
    double score2 = 123.31;
    struct FieldValue comparableValue1 = {DOUBLE, "score", &score2, sizeof(double)};
    predicate.comparableValue = &comparableValue1;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), false, "score", 4, 4);
    predicate.comparator = MORE_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), false, "score", 4, 5);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), true, "score", 4, 6);
    double score3 = 123.3;
    predicate.comparableValue->data = &score3;
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), true, "score", 4, 7);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), true, "score", 4, 8);
    predicate.comparator = LESS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), false, "score", 4, 9);
    char *name2 = "Ksenia Kirillova";
    struct FieldValue comparableValue2 = {STRING, "name", name2, sizeof(char) * strlen(name2)};
    predicate.comparableValue = &comparableValue2;
    predicate.fieldName = "name";
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), true, "name", 4, 10);
    predicate.comparator = MORE;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4), false, "name", 4, 11);
//    printEntityRecord(&entityRecord2, 4);
}

//check iterator
void test5() {
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 123.3;
    char *name = "Ksenia";
    char *surname = "Kirillova";
    bool sex = false;
    int32_t age = 19;
    struct FieldValue fieldValue1 = {DOUBLE, "score", &score, sizeof(double)};
    struct FieldValue fieldValue2 = {STRING, "name", name, sizeof(char) * strlen(name)};
    struct FieldValue fieldValue3 = {STRING, "surname", surname, sizeof(char) * strlen(surname)};
    struct FieldValue fieldValue4 = {BOOL, "sex", &sex, sizeof(bool)};
    struct FieldValue fieldValue5 = {INT, "age", &age, sizeof(int32_t)};
    struct FieldValue array1[5] = {fieldValue1, fieldValue2, fieldValue3, fieldValue4, fieldValue5};
    struct EntityRecord entityRecord = {array1};
    insertRecordIntoTable(file, &entityRecord, "User");

    double score2 = 128;
    char *name2 = "Nikita";
    char *surname2 = "Ivanov";
    bool sex2 = true;
    int32_t age2 = 20;
    struct FieldValue fieldValue21 = {DOUBLE, "score", &score2, sizeof(double)};
    struct FieldValue fieldValue22 = {STRING, "name", name2, sizeof(char) * strlen(name)};
    struct FieldValue fieldValue23 = {STRING, "surname", surname2, sizeof(char) * strlen(surname2)};
    struct FieldValue fieldValue24 = {BOOL, "sex", &sex2, sizeof(bool)};
    struct FieldValue fieldValue25 = {INT, "age", &age2, sizeof(int32_t)};
    struct FieldValue array2[5] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24, fieldValue25};
    struct EntityRecord entityRecord2 = {array2};
    insertRecordIntoTable(file, &entityRecord2, "User");

    double score3 = 356;
    char *name3 = "Lubov";
    char *surname3 = "Vitalievna";
    bool sex3 = false;
    int32_t age3 = 51;
    struct FieldValue fieldValue31 = {DOUBLE, "score", &score3, sizeof(double)};
    struct FieldValue fieldValue32 = {STRING, "name", name3, sizeof(char) * strlen(name)};
    struct FieldValue fieldValue33 = {STRING, "surname", surname3, sizeof(char) * strlen(surname3)};
    struct FieldValue fieldValue34 = {BOOL, "sex", &sex3, sizeof(bool)};
    struct FieldValue fieldValue35 = {INT, "age", &age3, sizeof(int32_t)};
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
    assertEquals(*(double *) entityRecord1->fields[0].data, 123.3, "score", 5, 2);
    assertEqualsS((char *) entityRecord1->fields[1].data, "Ksenia", "name", 5, 3);
    assertEqualsS((char *) entityRecord1->fields[2].data, "Kirillova", "surname", 5, 4);
    assertEquals(*(bool *) entityRecord1->fields[3].data, false, "sex", 5, 5);
    assertEquals(*(uint16_t *) entityRecord1->fields[4].data, 19, "age", 5, 6);

    bool isNext2 = hasNext(iterator, file);
    assertEquals(isNext2, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEquals(*(double *) entityRecord1->fields[0].data, 128, "score", 5, 7);
    assertEqualsS((char *) entityRecord1->fields[1].data, "Nikita", "name", 5, 8);
    assertEqualsS((char *) entityRecord1->fields[2].data, "Ivanov", "surname", 5, 9);
    assertEquals(*(bool *) entityRecord1->fields[3].data, true, "sex", 5, 10);
    assertEquals(*(uint16_t *) entityRecord1->fields[4].data, 20, "age", 5, 11);

    bool isNext3 = hasNext(iterator, file);
    assertEquals(isNext3, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEquals(*(double *) entityRecord1->fields[0].data, 356, "score", 5, 12);
    assertEqualsS((char *) entityRecord1->fields[1].data, "Lubov", "name", 5, 13);
    assertEqualsS((char *) entityRecord1->fields[2].data, "Vitalievna", "surname", 5, 14);
    assertEquals(*(bool *) entityRecord1->fields[3].data, false, "sex", 5, 15);
    assertEquals(*(uint16_t *) entityRecord1->fields[4].data, 51, "age", 5, 16);

    bool isNext4 = hasNext(iterator, file);
    assertEquals(isNext4, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEquals(*(double *) entityRecord1->fields[0].data, 124.3, "score", 5, 17);
    assertEqualsS((char *) entityRecord1->fields[1].data, "Ksenia", "name", 5, 18);
    assertEqualsS((char *) entityRecord1->fields[2].data, "Kirillova", "surname", 5, 19);
    assertEquals(*(bool *) entityRecord1->fields[3].data, false, "sex", 5, 20);
    assertEquals(*(uint16_t *) entityRecord1->fields[4].data, 19, "age", 5, 21);

    bool isNext5 = hasNext(iterator, file);
    assertEquals(isNext5, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEquals(*(double *) entityRecord1->fields[0].data, 125.3, "score", 5, 22);
    assertEqualsS((char *) entityRecord1->fields[1].data, "Ksenia", "name", 5, 23);
    assertEqualsS((char *) entityRecord1->fields[2].data, "Kirillova", "surname", 5, 24);
    assertEquals(*(bool *) entityRecord1->fields[3].data, false, "sex", 5, 25);
    assertEquals(*(uint16_t *) entityRecord1->fields[4].data, 19, "age", 5, 26);

    bool isNext6 = hasNext(iterator, file);
    assertEquals(isNext6, false, "hasNext", 5, 1);
    fclose(file);
}

//check iterator with predicates

void test6() {
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 356;
    int32_t age = 20;
    struct FieldValue fieldValue = {DOUBLE, "score", &score, sizeof(double)};
    struct FieldValue fieldValue1 = {INT, "age", &age, sizeof(int32_t)};
    struct predicate predicate[2] = {{&fieldValue,  "score", LESS},
                                     {&fieldValue1, "age",   MORE_OR_EQUALS}};
    struct iterator *iterator = readEntityRecordWithCondition(file, "User", predicate, 2);
    bool nextVal = hasNext(iterator, file);
    assertEquals(nextVal, true, "next", 6, 1);
    struct EntityRecord *entityRecord = next(iterator, file);
    assertEquals(*(double *) entityRecord->fields[0].data, 128, "score", 6, 2);
    assertEqualsS((char *) entityRecord->fields[1].data, "Nikita", "name", 6, 3);
    assertEqualsS((char *) entityRecord->fields[2].data, "Ivanov", "surname", 6, 4);
    assertEquals(*(bool *) entityRecord->fields[3].data, true, "sex", 6, 5);
    assertEquals(*(uint16_t *) entityRecord->fields[4].data, 20, "age", 6, 6);

    char *name = "Nikita";
    struct FieldValue fieldValue2 = {STRING, "name", name, sizeof(char) * strlen(name)};
    struct predicate predicate1[1] = {&fieldValue2, "name", EQUALS};
    struct iterator *iterator1 = readEntityRecordWithCondition(file, "User", predicate1, 1);
    bool nextVal1 = hasNext(iterator1, file);
    assertEquals(nextVal1, true, "next", 6, 7);
    struct EntityRecord *pEntityRecord = next(iterator1, file);
    assertEqualsS(pEntityRecord->fields[1].data, "Nikita", "name", 6, 8);
    fclose(file);
}
