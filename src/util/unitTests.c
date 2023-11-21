#include "../../include/util/unitTests.h"
#include "../../include/util/testPerfomance.h"

#define BLOCK_SPACE (sizeof (HeaderSection) + BLOCK_DATA_SIZE + sizeof (SpecialDataSection))

static void
assertEquals(const long double found, const double expected, char *fieldName, uint8_t testNumber, uint8_t position) {
    if (found != expected)
        printf("%u.%u: %s don't equals, expected %f, but found %Lf\n",
               testNumber, position, fieldName, expected, found);
}

static void
assertEqualsString(const char *found, const char *expected, char *fieldName, uint8_t testNumber, uint8_t position) {
    if (strcmp(found, expected) != 0)
        printf("%u.%u: %s don't equals, expected %s, but found %s\n",
               testNumber, position, fieldName, expected, found);
}

void test1(void) {
    FILE *file = fopen(FILE_NAME, "rb+");
    writeEmptyTablesBlock(file);
    assertEquals(readEmptySpaceOffset(file), sizeof(DefineTablesBlock) + BLOCK_SPACE, "EmptySpaceOffset", 1, 1);
    assertEquals(readTablesCount(file), 1, "table count", 1, 2);
    writeTableCount(file, 3);
    assertEquals(readEmptySpaceOffset(file), sizeof(DefineTablesBlock) + BLOCK_SPACE, "EmptySpaceOffset", 1, 3);
    assertEquals(readTablesCount(file), 3, "table count", 1, 4);
    assertEquals(readTablesCount(file), 3, "table count", 1, 6);
    fclose(file);
}

void test2(void) {
    FILE *file = fopen(FILE_NAME, "rb+");
    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);

    // 1 table
    NameTypeBlock nameTypeBlocks1[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    TableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "User", 5, nameTypeBlocks1);
    writeTableOffsetBlock(file, writtenTableOffsetBlock1);
    TableOffsetBlock *tableOffsetBlock1 = readTableOffsetBlock(file, 1);

    assertEquals(tableOffsetBlock1->fieldsNumber, 5, "fieldsNumber", 2, 1);
    assertEqualsString(tableOffsetBlock1->tableName, "User", "tableName", 2, 2);
    assertEquals(tableOffsetBlock1->lastTableBLockOffset, sizeof(DefineTablesBlock) + BLOCK_SPACE,
                 "lastTableOffsetBlock", 2, 3);
    assertEquals(tableOffsetBlock1->firsTableBlockOffset, sizeof(DefineTablesBlock) + BLOCK_SPACE,
                 "firstTableOffsetBlock", 2, 4);
    assertEqualsString(tableOffsetBlock1->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 5);

    // 2 tables
    NameTypeBlock nameTypeBlocks2[3] = {
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4
    };
    TableOffsetBlock *writtenTableOffsetBlock2 = initTableOffsetBlock(file, "Cake", 3, nameTypeBlocks2);
    writeTableOffsetBlock(file, writtenTableOffsetBlock2);
    TableOffsetBlock *tableOffsetBlock2 = readTableOffsetBlock(file, 2);

    assertEquals(tableOffsetBlock2->fieldsNumber, 3, "fieldsNumber", 2, 6);
    assertEqualsString(tableOffsetBlock2->tableName, "Cake", "tableName", 2, 7);
    uint64_t expectedOffset = sizeof(DefineTablesBlock) + sizeof(HeaderSection) + BLOCK_DATA_SIZE +
                              sizeof(SpecialDataSection);
    assertEquals(tableOffsetBlock2->lastTableBLockOffset, expectedOffset + BLOCK_SPACE, "lastTableOffsetBlock", 2, 8);
    assertEquals(tableOffsetBlock2->firsTableBlockOffset, expectedOffset + BLOCK_SPACE, "firstTableOffsetBlock", 2, 9);
    assertEqualsString(tableOffsetBlock2->nameTypeBlock[0].fieldName, "Surname", "Name 1 field", 2, 10);

    // check if table 2 overwritten table 1
    TableOffsetBlock *tableOffsetBlock3 = readTableOffsetBlock(file, 1);

    assertEquals(tableOffsetBlock3->fieldsNumber, 5, "fieldsNumber", 2, 11);
    assertEqualsString(tableOffsetBlock3->tableName, "User", "tableName", 2, 12);
    assertEquals(tableOffsetBlock3->lastTableBLockOffset, sizeof(DefineTablesBlock) + BLOCK_SPACE,
                 "lastTableOffsetBlock", 2, 13);
    assertEquals(tableOffsetBlock3->firsTableBlockOffset, sizeof(DefineTablesBlock) + BLOCK_SPACE,
                 "firstTableOffsetBlock", 2, 14);
    assertEqualsString(tableOffsetBlock3->nameTypeBlock[0].fieldName, "Name", "Name 1 field", 2, 15);

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

void test3(void) {
    FILE *file = fopen(FILE_NAME_1, "rb+");

    // test read write 1 record
    HeaderSection headerSection = {0, 0, BLOCK_DATA_SIZE, 0};
    double score = 1234.4;
    char *name = "Nikita";
    bool sex = true;
    int32_t age = 20;
    FieldValue fieldValue1 = {&score, sizeof(double)};
    FieldValue fieldValue2 = {name, sizeof(char) * strlen(name)};
    FieldValue fieldValue3 = {&sex, sizeof(bool)};
    FieldValue fieldValue4 = {&age, sizeof(int32_t)};
    FieldValue array[4] = {fieldValue1, fieldValue2, fieldValue3, fieldValue4};
    EntityRecord entityRecord = {array, NULL};

    NameTypeBlock nameTypeBlock1 = {"score", DOUBLE};
    NameTypeBlock nameTypeBlock2 = {"name", STRING};
    NameTypeBlock nameTypeBlock3 = {"sex", BOOL};
    NameTypeBlock nameTypeBlock4 = {"age", INT};
    TableOffsetBlock tableOffsetBlock = {true, "Users",
                                         {nameTypeBlock1, nameTypeBlock2, nameTypeBlock3, nameTypeBlock4}, 4, 0,
                                         0};
    fseek(file, 0, SEEK_SET);
    fwrite(&headerSection, sizeof(HeaderSection), 1, file);
    insertRecord(file, &entityRecord, &tableOffsetBlock);
    EntityRecord *entityRecord1 = readRecord(file, 0, 0, 4);
    assertEquals(*(double *) entityRecord1->fields[0].data, 1234.4, "value1", 3, 2);
    assertEqualsString(cutString(entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize), "Nikita",
                       "value2", 3, 4);
    assertEquals(*(bool *) entityRecord1->fields[2].data, 1, "value3", 3, 6);
    assertEquals(*(int32_t *) entityRecord1->fields[3].data, 20, "value4", 3, 8);
    //------------------------------------------------------------------------------------------------------------------------
    // test writing the second record and don't break anything
    HeaderSection headerSection1;
    fseek(file, 0, SEEK_SET);
    double score1 = 123.3;
    char *name1 = "Ksenia Kirillova";
    bool sex1 = false;
    int32_t age1 = 19;
    FieldValue fieldValue21 = {&score1, sizeof(double)};
    FieldValue fieldValue22 = {name1, sizeof(char) * strlen(name1)};
    FieldValue fieldValue23 = {&sex1, sizeof(bool)};
    FieldValue fieldValue24 = {&age1, sizeof(int32_t)};
    FieldValue array1[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    EntityRecord entityRecord2 = {array1, NULL};
    insertRecord(file, &entityRecord2, &tableOffsetBlock);
    EntityRecord *entityRecord12 = readRecord(file, 0, 0, 4);
    assertEquals(*(double *) entityRecord12->fields[0].data, 1234.4, "value1", 3, 10);
    assertEqualsString(cutString(entityRecord12->fields[1].data, 0, entityRecord1->fields[1].dataSize), "Nikita",
                       "value2", 3, 12);
    assertEquals(*(bool *) entityRecord12->fields[2].data, 1, "value3", 3, 14);
    assertEquals(*(int32_t *) entityRecord12->fields[3].data, 20, "value4", 3, 16);
    EntityRecord *entityRecord22 = readRecord(file, 1, 0, 4);
    assertEquals(*(double *) entityRecord22->fields[0].data, 123.3, "value1", 3, 18);
    assertEqualsString(cutString(entityRecord22->fields[1].data, 0, entityRecord22->fields[1].dataSize),
                       "Ksenia Kirillova",
                       "value2", 3, 20);
    assertEquals(*(bool *) entityRecord22->fields[2].data, 0, "value3", 3, 22);
    assertEquals(*(uint16_t *) entityRecord22->fields[3].data, 19, "value4", 3, 24);
    fseek(file, 0, SEEK_SET);
    fread(&headerSection1, sizeof(HeaderSection), 1, file);
    assertEquals(headerSection1.recordsNumber, 2, "recordsNumber", 3, 25);
    assertEquals(headerSection1.pageNumber, 0, "pageNUmber", 3, 26);
    assertEquals(headerSection1.endEmptySpaceOffset, BLOCK_DATA_SIZE - sizeof(RecordId) * 2,
                 "endEmptySpaceOffset", 3, 27);
    fclose(file);
}

// test checkPredicate function
void test4(void) {
    NameTypeBlock nameTypeBlock1 = {"score", DOUBLE};
    NameTypeBlock nameTypeBlock2 = {"name", STRING};
    NameTypeBlock nameTypeBlock3 = {"sex", BOOL};
    NameTypeBlock nameTypeBlock4 = {"age", INT};
    NameTypeBlock nameTypeBlock[4] = {nameTypeBlock1, nameTypeBlock2, nameTypeBlock3, nameTypeBlock4};
    Predicate predicate;
    double score1 = 123.3;
    char *name1 = "Ksenia Kirillova";
    bool sex1 = false;
    int32_t age1 = 19;
    FieldValue fieldValue21 = {&score1, sizeof(double)};
    FieldValue fieldValue22 = {name1, sizeof(char) * strlen(name1)};
    FieldValue fieldValue23 = {&sex1, sizeof(bool)};
    FieldValue fieldValue24 = {&age1, sizeof(int32_t)};
    FieldValue array1[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    EntityRecord entityRecord2 = {array1, NULL};
    predicate.fieldName = "sex";
    predicate.comparator = MORE_OR_EQUALS;
    bool sex2 = true;
    FieldValue comparableValue = {&sex2, sizeof(bool)};
    predicate.comparableValue = &comparableValue;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "sex", 4, 1);
    predicate.comparator = LESS_OR_EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "sex", 4, 2);
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "sex", 4, 3);
    predicate.fieldName = "score";
    double score2 = 123.31;
    FieldValue comparableValue1 = {&score2, sizeof(double)};
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
    FieldValue comparableValue2 = {name2, sizeof(char) * strlen(name2)};
    predicate.comparableValue = &comparableValue2;
    predicate.fieldName = "name";
    predicate.comparator = EQUALS;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), true, "name", 4, 10);
    predicate.comparator = MORE;
    assertEquals(checkPredicate(&predicate, &entityRecord2, 4, nameTypeBlock), false, "name", 4, 11);
}

//check Iterator
void test5(void) {
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 123.3;
    char *name = "Ksenia";
    char *surname = "Kirillova";
    bool sex = false;
    int32_t age = 19;
    FieldValue fieldValue1 = {name, sizeof(char) * strlen(name)};
    FieldValue fieldValue2 = {surname, sizeof(char) * strlen(surname)};
    FieldValue fieldValue3 = {&age, sizeof(int32_t)};
    FieldValue fieldValue4 = {&score, sizeof(double)};
    FieldValue fieldValue5 = {&sex, sizeof(bool)};
    FieldValue array1[5] = {fieldValue1, fieldValue2, fieldValue3, fieldValue4, fieldValue5};
    EntityRecord entityRecord = {array1, NULL};
    insertRecordIntoTable(file, &entityRecord, "User");

    double score2 = 128;
    char *name2 = "Nikita";
    char *surname2 = "Ivanov";
    bool sex2 = true;
    int32_t age2 = 20;
    FieldValue fieldValue21 = {name2, sizeof(char) * strlen(name2)};
    FieldValue fieldValue22 = {surname2, sizeof(char) * strlen(surname2)};
    FieldValue fieldValue23 = {&age2, sizeof(int32_t)};
    FieldValue fieldValue24 = {&score2, sizeof(double)};
    FieldValue fieldValue25 = {&sex2, sizeof(bool)};
    FieldValue array2[5] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24, fieldValue25};
    EntityRecord entityRecord2 = {array2, NULL};
    insertRecordIntoTable(file, &entityRecord2, "User");

    double score3 = 356;
    char *name3 = "Lubovv";
    char *surname3 = "Vitalievna";
    bool sex3 = false;
    int32_t age3 = 51;
    FieldValue fieldValue31 = {name3, sizeof(char) * strlen(name3)};
    FieldValue fieldValue32 = {surname3, sizeof(char) * strlen(surname3)};
    FieldValue fieldValue33 = {&age3, sizeof(int32_t)};
    FieldValue fieldValue34 = {&score3, sizeof(double)};
    FieldValue fieldValue35 = {&sex3, sizeof(bool)};
    FieldValue array3[5] = {fieldValue31, fieldValue32, fieldValue33, fieldValue34, fieldValue35};
    EntityRecord entityRecord3 = {array3, NULL};
    insertRecordIntoTable(file, &entityRecord3, "User");
    score++;
    insertRecordIntoTable(file, &entityRecord, "User");
    score++;
    insertRecordIntoTable(file, &entityRecord, "User");
    Iterator *iterator = readEntityRecordWithCondition(file, "User", NULL, 0);
    bool isNext1 = hasNext(iterator, file);
    assertEquals(isNext1, true, "hasNext", 5, 1);
    EntityRecord *entityRecord1 = next(iterator, file);
    assertEqualsString(cutString((char *) entityRecord1->fields[0].data, 0, entityRecord1->fields[0].dataSize),
                       "Ksenia",
                       "name", 5, 3);
    assertEqualsString(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize),
                       "Kirillova",
                       "surname", 5, 4);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 19, "age", 5, 6);
    assertEquals(*(double *) entityRecord1->fields[3].data, 123.3, "score", 5, 2);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 5);

    bool isNext2 = hasNext(iterator, file);
    assertEquals(isNext2, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsString(cutString((char *) entityRecord1->fields[0].data, 0, entityRecord1->fields[0].dataSize),
                       "Nikita",
                       "name", 5, 8);
    assertEqualsString(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[0].dataSize),
                       "Ivanov",
                       "surname", 5, 9);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 20, "age", 5, 11);
    assertEquals(*(double *) entityRecord1->fields[3].data, 128, "score", 5, 7);
    assertEquals(*(bool *) entityRecord1->fields[4].data, true, "sex", 5, 10);

    bool isNext3 = hasNext(iterator, file);
    assertEquals(isNext3, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsString(cutString((char *) entityRecord1->fields[0].data, 0, entityRecord1->fields[0].dataSize),
                       "Lubovv",
                       "name", 5, 13);
    assertEqualsString(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize),
                       "Vitalievna",
                       "surname", 5, 14);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 51, "age", 5, 16);
    assertEquals(*(double *) entityRecord1->fields[3].data, 356, "score", 5, 12);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 15);

    bool isNext4 = hasNext(iterator, file);
    assertEquals(isNext4, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsString(cutString((char *) entityRecord1->fields[0].data, 0, entityRecord1->fields[0].dataSize),
                       "Ksenia",
                       "name", 5, 18);
    assertEqualsString(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize),
                       "Kirillova",
                       "surname", 5, 19);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 19, "age", 5, 21);
    assertEquals(*(double *) entityRecord1->fields[3].data, 124.3, "score", 5, 17);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 20);

    bool isNext5 = hasNext(iterator, file);
    assertEquals(isNext5, true, "hasNext", 5, 1);
    entityRecord1 = next(iterator, file);
    assertEqualsString(cutString((char *) entityRecord1->fields[0].data, 0, entityRecord1->fields[0].dataSize),
                       "Ksenia",
                       "name", 5, 23);
    assertEqualsString(cutString((char *) entityRecord1->fields[1].data, 0, entityRecord1->fields[1].dataSize),
                       "Kirillova",
                       "surname", 5, 24);
    assertEquals(*(uint16_t *) entityRecord1->fields[2].data, 19, "age", 5, 26);
    assertEquals(*(double *) entityRecord1->fields[3].data, 125.3, "score", 5, 22);
    assertEquals(*(bool *) entityRecord1->fields[4].data, false, "sex", 5, 25);

    bool isNext6 = hasNext(iterator, file);
    assertEquals(isNext6, false, "hasNext", 5, 1);
    fclose(file);
}

//check Iterator with predicates

void test6(void) {
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 356;
    int32_t age = 20;
    FieldValue fieldValue = {&score, sizeof(double)};
    FieldValue fieldValue1 = {&age, sizeof(int32_t)};
    Predicate predicate[2] = {{&fieldValue,  "Score", LESS},
                              {&fieldValue1, "Age",   MORE_OR_EQUALS}};
    Iterator *iterator = readEntityRecordWithCondition(file, "User", predicate, 2);
    bool nextVal = hasNext(iterator, file);
    assertEquals(nextVal, true, "next", 6, 1);
    EntityRecord *entityRecord = next(iterator, file);
    assertEqualsString(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Nikita",
                       "name", 6, 3);
    assertEqualsString(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize), "Ivanov",
                       "surname", 6, 4);
    assertEquals(*(uint16_t *) entityRecord->fields[2].data, 20, "age", 6, 6);
    assertEquals(*(double *) entityRecord->fields[3].data, 128, "score", 6, 2);
    assertEquals(*(bool *) entityRecord->fields[4].data, true, "sex", 6, 5);

    char *name = "Nikita";
    FieldValue fieldValue2 = {name, sizeof(char) * strlen(name)};
    Predicate predicate1[1] = {{&fieldValue2, "Name", EQUALS}};
    Iterator *iterator1 = readEntityRecordWithCondition(file, "User", predicate1, 1);
    bool nextVal1 = hasNext(iterator1, file);
    assertEquals(nextVal1, true, "next", 6, 7);
    EntityRecord *pEntityRecord = next(iterator1, file);
    assertEqualsString(cutString(pEntityRecord->fields[0].data, 0, pEntityRecord->fields[0].dataSize), "Nikita", "name",
                       6,
                       8);
    fclose(file);
}

static void testRebuild(void) {
    FILE *file = fopen(FILE_NAME_2, "rb+");
    RecordId recordId1 = {0, 12};
    RecordId recordId2 = {12, 13};
    RecordId recordId3 = {25, 14};
    RecordId recordId4 = {39, 15};
    RecordId recordId5 = {54, 16};
    RecordId recordId6 = {70, 17};
    RecordId recordId7 = {87, 18};
    RecordId recordIds[7] = {recordId7, recordId6, recordId5, recordId4, recordId3, recordId2, recordId1};
    HeaderSection headerSection = {0, 0, BLOCK_DATA_SIZE - sizeof(RecordId) * 7, 7};
    SpecialDataSection specialDataSection = {0, 0};
    fwrite(&headerSection, sizeof(HeaderSection), 1, file);
    fflush(file);
    fseek(file, sizeof(HeaderSection) + BLOCK_DATA_SIZE - sizeof(RecordId) * 7, SEEK_SET);
    fwrite(recordIds, sizeof(RecordId) * 7, 1, file);
    fflush(file);
    fwrite(&specialDataSection, sizeof(SpecialDataSection), 1, file);
    fflush(file);
    unsigned char *buffer = malloc(sizeof(HeaderSection) + BLOCK_DATA_SIZE + sizeof(SpecialDataSection));
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(HeaderSection) + BLOCK_DATA_SIZE + sizeof(SpecialDataSection), 1, file);
    RecordId *resultIds = malloc(sizeof(RecordId) * 7);
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

static void testRebuild1(void) {
    FILE *file = fopen(FILE_NAME_2, "rb+");
    RecordId recordId1 = {0, 21};
    RecordId recordIds[1] = {recordId1};
    HeaderSection headerSection = {0, 0, BLOCK_DATA_SIZE - sizeof(RecordId) * 1, 2};
    SpecialDataSection specialDataSection = {0, 0};
    fwrite(&headerSection, sizeof(HeaderSection), 1, file);
    fflush(file);
    fseek(file, sizeof(HeaderSection) + BLOCK_DATA_SIZE - sizeof(RecordId) * 1, SEEK_SET);
    fwrite(recordIds, sizeof(RecordId) * 1, 1, file);
    fflush(file);
    fwrite(&specialDataSection, sizeof(SpecialDataSection), 1, file);
    fflush(file);
    unsigned char *buffer = malloc(sizeof(HeaderSection) + BLOCK_DATA_SIZE + sizeof(SpecialDataSection));
    fseek(file, 0, SEEK_SET);
    fread(buffer, sizeof(HeaderSection) + BLOCK_DATA_SIZE + sizeof(SpecialDataSection), 1, file);
    RecordId *resultIds = malloc(sizeof(RecordId) * 1);
    rebuildArrayOfRecordIds(buffer, resultIds, 1, 0, 21);
}

// deleteRecordTest
void test7(void) {
    // test rebuildArrayOfRecordIds function
    testRebuild();
    testRebuild1();
    HeaderSection headerSection = {0, 0, BLOCK_DATA_SIZE - sizeof(RecordId) * 7, 7};


    //test deleteRecordFromTable function
    //delete 1 record
    char *name = "Nikita";
    FieldValue fieldValue = {name, sizeof(char) * strlen(name)};
    FILE *file1 = fopen(FILE_NAME, "rb+");
    Predicate predicate[1] = {{&fieldValue, "Name", EQUALS}};
    Iterator *iterator = readEntityRecordWithCondition(file1, "User", predicate, 1);
    bool has = hasNext(iterator, file1);
    assertEquals(has, true, "hasNext", 7, 13);
    EntityRecord *entityRecord = next(iterator, file1);
    assertEqualsString(cutString(entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Nikita", "name",
                       7,
                       14);
    deleteRecordFromTable(file1, "User", predicate, 1);
    Iterator *iterator1 = readEntityRecordWithCondition(file1, "User", predicate, 1);
    bool has1 = hasNext(iterator1, file1);
    assertEquals(has1, false, "hasNext", 7, 15);
    Iterator *iterator2 = readEntityRecordWithCondition(file1, "User", NULL, 0);

    double array[] = {123.3, 356, 124.3, 125.3};
    uint8_t i = 0;
    while (hasNext(iterator2, file1)) {
        EntityRecord *entityRecord1 = next(iterator2, file1);
        assertEquals(*(double *) entityRecord1->fields[3].data, array[i], "score", 7, 16 + i);
        i++;
    }
    assertEquals(i, 4, "recordsNumber", 7, 21);
    char *name1 = "Ksenia";
    FieldValue fieldValue1 = {name1, sizeof(char) * (strlen(name1) + 1)};
    Predicate predicate1[1] = {{&fieldValue1, "Name", EQUALS}};
    Iterator *iterator3 = readEntityRecordWithCondition(file1, "User", predicate1, 1);
    bool has2 = hasNext(iterator3, file1);
    assertEquals(has2, true, "hasNext", 7, 22);
    EntityRecord *entityRecord1 = next(iterator3, file1);
    assertEqualsString(cutString(entityRecord1->fields[0].data, 0, entityRecord1->fields[0].dataSize), "Ksenia",
                       "surname",
                       7, 23);


    //delete several records in 1 operation
    double score = 140.3;
    FieldValue fieldValue2 = {&score, sizeof(double)};
    Predicate predicate2[1] = {{&fieldValue2, "Score", LESS}};
    deleteRecordFromTable(file1, "User", predicate2, 1);
    Iterator *iterator4 = readEntityRecordWithCondition(file1, "User", NULL, 0);
    score = 356;
    i = 0;
    while (hasNext(iterator4, file1)) {
        EntityRecord *entityRecord2 = next(iterator4, file1);
        assertEquals(*(double *) entityRecord2->fields[3].data, score, "score", 7, 24 + i);
        i++;
    }
    assertEquals(i, 1, "recordsNumber", 7, 25);


    fseek(file1, iterator->blockOffset, SEEK_SET);
    fread(&headerSection, sizeof(HeaderSection), 1, file1);
    assertEquals(headerSection.recordsNumber, 1, "recordsNumber", 7, 26);
    assertEquals(headerSection.endEmptySpaceOffset, BLOCK_DATA_SIZE - sizeof(RecordId), "endEmptySpaceOffset", 7,
                 27);
    assertEquals(headerSection.pageNumber, 0, "pageNumber", 7, 28);
    RecordId recordId;
    fseek(file1, iterator->blockOffset + sizeof(HeaderSection) + BLOCK_DATA_SIZE - sizeof(RecordId),
          SEEK_SET);
    fread(&recordId, sizeof(RecordId), 1, file1);
    assertEquals(recordId.offset, 0, "offset", 7, 29);
    fclose(file1);
}

void test8(void) {
    test1();
    test2();
    test5();
    test6();
    FILE *file = fopen(FILE_NAME, "rb+");
    printf("\n");
    double score = 124.4;
    bool sex = false;
    FieldValue fieldValue = {&score, sizeof(double)};
    FieldValue fieldValue2 = {&sex, sizeof(bool)};
    Predicate predicate[2] = {{&fieldValue,  "Score", MORE},
                              {&fieldValue2, "Sex",   EQUALS}};
    deleteRecordFromTable(file, "User", predicate, 2);
    Iterator *iterator1 = readEntityRecordWithCondition(file, "User", NULL, 0);

    const char *surnames[3] = {"Kirillova", "Ivanov", "Kirillova"};
    const double scores[3] = {123.3, 128, 124.3};
    uint8_t i = 0;
    while (hasNext(iterator1, file)) {
        EntityRecord *entityRecord = next(iterator1, file);
        assertEquals(*(double *) entityRecord->fields[3].data, scores[i], "score", 8, i * 2 + 1);
        assertEqualsString(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize),
                           surnames[i], "surname", 8, i * 2 + 2);
        i++;
    }
    assertEquals(i, 3, "recordsNUmber", 8, 12);
    fclose(file);
}

// test update
void test9(void) {
    test1();
    test2();
    test5();
    test6();
    FILE *file = fopen(FILE_NAME, "rb+");
    double score = 128;
    FieldValue fieldValue = {&score, sizeof(score)};
    Predicate predicate[1] = {{&fieldValue, "Score", EQUALS}};

    double score2 = 140;
    char *name2 = "Nikita";
    char *surname2 = "Pesterev";
    bool sex2 = true;
    int32_t age2 = 20;
    FieldValue fieldValue21 = {name2, sizeof(char) * strlen(name2)};
    FieldValue fieldValue22 = {surname2, sizeof(char) * strlen(surname2)};
    FieldValue fieldValue23 = {&age2, sizeof(int32_t)};
    FieldValue fieldValue24 = {&score2, sizeof(double)};
    FieldValue fieldValue25 = {&sex2, sizeof(bool)};
    FieldValue array2[5] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24, fieldValue25};
    EntityRecord entityRecord2 = {array2, NULL};
    updateRecordsFromTable(file, "User", predicate, 1, &entityRecord2);

    FieldValue fieldValue1 = {name2, sizeof(char) * strlen(name2)};
    Predicate predicate1[1] = {{&fieldValue1, "Name", EQUALS}};
    Iterator *iterator1 = readEntityRecordWithCondition(file, "User", predicate1, 1);
    bool nextVal = hasNext(iterator1, file);
    assertEquals(nextVal, true, "hasNext", 9, 1);
    EntityRecord *entityRecord = next(iterator1, file);
    assertEqualsString(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Nikita",
                       "name", 9, 2);
    assertEqualsString(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize),
                       "Pesterev",
                       "surname", 9, 3);
    assertEquals(*(uint16_t *) entityRecord->fields[2].data, 20, "age", 9, 4);
    assertEquals(*(double *) entityRecord->fields[3].data, 140, "score", 9, 5);
    assertEquals(*(bool *) entityRecord->fields[4].data, true, "sex", 9, 6);
    fclose(file);
}

// test join
void test10(void) {
    FILE *file = fopen(FILE_NAME, "rb+");
    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("DepartmentId", INT);
    // 1 table
    NameTypeBlock nameTypeBlocks1[4] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4
    };
    TableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "Employee", 4, nameTypeBlocks1);
    writeTableOffsetBlock(file, writtenTableOffsetBlock1);

    NameTypeBlock *nameTypeBlock21 = initNameTypeBlock("DepartmentId", INT);
    NameTypeBlock *nameTypeBlock22 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock23 = initNameTypeBlock("Description", STRING);
    // 2 table
    NameTypeBlock nameTypeBlocks2[3] = {
            *nameTypeBlock21,
            *nameTypeBlock22,
            *nameTypeBlock23
    };

    TableOffsetBlock *writtenTableOffsetBlock2 = initTableOffsetBlock(file, "Department", 3, nameTypeBlocks2);
    writeTableOffsetBlock(file, writtenTableOffsetBlock2);

    char *name1 = "Nikita";
    char *surname1 = "Ivanov";
    int32_t age1 = 20;
    int32_t departmentId1 = 1;
    FieldValue fieldValue11 = {name1, sizeof(char) * strlen(name1)};
    FieldValue fieldValue12 = {surname1, sizeof(char) * strlen(name1)};
    FieldValue fieldValue13 = {&age1, sizeof(int32_t)};
    FieldValue fieldValue14 = {&departmentId1, sizeof(int32_t)};
    FieldValue array1[4] = {fieldValue11, fieldValue12, fieldValue13, fieldValue14};
    EntityRecord entityRecord1 = {array1, NULL};
    insertRecordIntoTable(file, &entityRecord1, "Employee");

    char *name2 = "Ivan";
    char *surname2 = "Bobrov";
    int32_t age2 = 21;
    int32_t departmentId2 = 2;
    FieldValue fieldValue21 = {name2, sizeof(char) * strlen(name2)};
    FieldValue fieldValue22 = {surname2, sizeof(char) * strlen(surname2)};
    FieldValue fieldValue23 = {&age2, sizeof(int32_t)};
    FieldValue fieldValue24 = {&departmentId2, sizeof(int32_t)};
    FieldValue array2[4] = {fieldValue21, fieldValue22, fieldValue23, fieldValue24};
    EntityRecord entityRecord2 = {array2, NULL};
    insertRecordIntoTable(file, &entityRecord2, "Employee");

    char *name3 = "Boris";
    char *surname3 = "Kirillov";
    int32_t age3 = 50;
    int32_t departmentId3 = 3;
    FieldValue fieldValue31 = {name3, sizeof(char) * strlen(name3)};
    FieldValue fieldValue32 = {surname3, sizeof(char) * strlen(surname3)};
    FieldValue fieldValue33 = {&age3, sizeof(int32_t)};
    FieldValue fieldValue34 = {&departmentId3, sizeof(int32_t)};
    FieldValue array3[4] = {fieldValue31, fieldValue32, fieldValue33, fieldValue34};
    EntityRecord entityRecord3 = {array3, NULL};
    insertRecordIntoTable(file, &entityRecord3, "Employee");


    char *name4 = "Entropy";
    char *description4 = "Command develop Accounting and Invoicing modules";
    FieldValue fieldValue41 = {&departmentId1, sizeof(int32_t)};
    FieldValue fieldValue42 = {name4, sizeof(char) * strlen(name4)};
    FieldValue fieldValue43 = {description4, sizeof(char) * strlen(description4)};
    FieldValue array4[3] = {fieldValue41, fieldValue42, fieldValue43};
    EntityRecord entityRecord4 = {array4, NULL};
    insertRecordIntoTable(file, &entityRecord4, "Department");

    char *name5 = "QuantumTeam";
    char *description5 = "Command develop loyality module";
    FieldValue fieldValue51 = {&departmentId2, sizeof(int32_t)};
    FieldValue fieldValue52 = {name5, sizeof(char) * strlen(name5)};
    FieldValue fieldValue53 = {description5, sizeof(char) * strlen(description5)};
    FieldValue array5[3] = {fieldValue51, fieldValue52, fieldValue53};
    EntityRecord entityRecord5 = {array5, NULL};
    insertRecordIntoTable(file, &entityRecord5, "Department");

    char *name6 = "BACKOFFICE";
    char *description6 = "Command develop backoffice module";
    FieldValue fieldValue61 = {&departmentId3, sizeof(int32_t)};
    FieldValue fieldValue62 = {name6, sizeof(char) * strlen(name6)};
    FieldValue fieldValue63 = {description6, sizeof(char) * strlen(description6)};
    FieldValue array6[3] = {fieldValue61, fieldValue62, fieldValue63};
    EntityRecord entityRecord6 = {array6, NULL};
    insertRecordIntoTable(file, &entityRecord6, "Department");

    Iterator *joinIterator = readEntityRecordWithCondition(file, "Employee", NULL, 0);
    EntityRecord *entityRecord = nextWithJoin(joinIterator, "Department", file, 3, "DepartmentId");
    assertEqualsString(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Nikita",
                       "Name", 10, 0);
    assertEqualsString(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize), "Ivanov",
                       "Surname", 10, 1);
    assertEquals(*(int32_t *) entityRecord->fields[2].data, 20, "Age", 10, 2);
    assertEquals(*(int32_t *) entityRecord->fields[3].data, 1, "DepartmentId", 10, 3);
    assertEquals(*(int32_t *) entityRecord->fields[4].data, 1, "DepartmentId", 10, 4);
    assertEqualsString(cutString((char *) entityRecord->fields[5].data, 0, entityRecord->fields[5].dataSize), "Entropy",
                       "Name", 10, 5);
    assertEqualsString(cutString((char *) entityRecord->fields[6].data, 0, entityRecord->fields[6].dataSize),
                       "Command develop Accounting and Invoicing modules",
                       "Description", 10, 6);


    entityRecord = nextWithJoin(joinIterator, "Department", file, 3, "DepartmentId");
    assertEqualsString(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Ivan",
                       "Name",
                       10, 7);
    assertEqualsString(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize), "Bobrov",
                       "Surname", 10, 8);
    assertEquals(*(int32_t *) entityRecord->fields[2].data, 21, "Age", 10, 9);
    assertEquals(*(int32_t *) entityRecord->fields[3].data, 2, "DepartmentId", 10, 10);
    assertEquals(*(int32_t *) entityRecord->fields[4].data, 2, "DepartmentId", 10, 11);
    assertEqualsString(cutString((char *) entityRecord->fields[5].data, 0, entityRecord->fields[5].dataSize),
                       "QuantumTeam",
                       "Name", 10, 12);
    assertEqualsString(cutString((char *) entityRecord->fields[6].data, 0, entityRecord->fields[6].dataSize),
                       "Command develop loyality module", "Description", 10, 13);

    entityRecord = nextWithJoin(joinIterator, "Department", file, 3, "DepartmentId");
    assertEqualsString(cutString((char *) entityRecord->fields[0].data, 0, entityRecord->fields[0].dataSize), "Boris",
                       "Name", 10, 14);
    assertEqualsString(cutString((char *) entityRecord->fields[1].data, 0, entityRecord->fields[1].dataSize),
                       "Kirillov",
                       "Surname", 10, 15);
    assertEquals(*(int32_t *) entityRecord->fields[2].data, 50, "Age", 10, 16);
    assertEquals(*(int32_t *) entityRecord->fields[3].data, 3, "DepartmentId", 10, 17);
    assertEquals(*(int32_t *) entityRecord->fields[4].data, 3, "DepartmentId", 10, 18);
    assertEqualsString(cutString((char *) entityRecord->fields[5].data, 0, entityRecord->fields[5].dataSize),
                       "BACKOFFICE",
                       "Name", 10, 19);
    assertEqualsString(cutString((char *) entityRecord->fields[6].data, 0, entityRecord->fields[6].dataSize),
                       "Command develop backoffice module", "Description", 10, 20);
    fclose(file);
}

// check memory in file
void test11(void) {
    FILE *file = fopen(FILE_NAME, "rb+");
    deleteTable("User", file);
    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);

    // 1 table
    NameTypeBlock nameTypeBlocks1[5] = {
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
        TableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "Users1", 5, nameTypeBlocks1);
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
    cutFile(file, offset + 1);
    fclose(file);
}


void test12(void) {

    NameTypeBlock nameTypeBlocks[4] = {{"Age",         INT},
                                       {"Sex",         BOOL},
                                       {"Description", STRING},
                                       {"Score",       DOUBLE}};
    printf("\n");
    for (uint16_t i = 40; i < 100; i++) {
        EntityRecord *entityRecordTest = malloc(sizeof(EntityRecord));

        int32_t *ageTest = malloc(sizeof(int32_t));
        *ageTest = 32;

        bool *sexTest = malloc(sizeof(bool));
        *sexTest = false;
        uint64_t length = strlen(
                "If we have a chatterbox here, then I have such news: I've come home, I haven't seen my parents for 3 months)");
        char *descriptionTest = malloc(sizeof(char) *
                                       length);
        strncpy(descriptionTest,
                "If we have a chatterbox here, then I have such news: I've come home, I haven't seen my parents for 3 months)",
                sizeof(char) *
                length);

        double *scoreTest = malloc(sizeof(double));
        *scoreTest = 123.324;

        FieldValue *fieldValueTest = malloc(sizeof(FieldValue) * 4);

        fieldValueTest[0].data = ageTest;
        fieldValueTest[0].dataSize = sizeof(int32_t);
        fieldValueTest[1].data = sexTest;
        fieldValueTest[1].dataSize = sizeof(bool);
        fieldValueTest[2].data = descriptionTest;
        fieldValueTest[2].dataSize = (sizeof(char) * length);
        fieldValueTest[3].data = scoreTest;
        fieldValueTest[3].dataSize = sizeof(double);

        entityRecordTest->fields = fieldValueTest;

        EntityRecord **entities = separateEntityRecord(entityRecordTest, i, 4, nameTypeBlocks);
        EntityRecord *entityRecordCompound = compoundEntityRecords(entities[0], entities[1], 4);
        assertEquals(*(int32_t *) entityRecordCompound->fields[0].data, *ageTest, "age", 12, 4 * i);
        assertEquals(*(bool *) entityRecordCompound->fields[1].data, false, "sexTest", 12, 4 * i + 1);
        assertEqualsString(
                cutString((char *) entityRecordCompound->fields[2].data, 0, length),
                cutString(descriptionTest, 0, length), "descriptionTest", 12, 4 * i + 2);
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

// check Meta table
void test13(void) {
    testInsertPerformance(1000);
    FILE *file = fopen(FILE_NAME, "rb+");
    Iterator *iterator = readEntityRecordWithCondition(file, "Users", NULL, 0);
    NameTypeBlock *nameTypeBlocks = buildNameTypeBlock();
    while (hasNext(iterator, file)) {
        EntityRecord *entityRecord = next(iterator, file);
        printEntityRecord(entityRecord, 5, nameTypeBlocks);
        freeEntityRecord(entityRecord, 5);
    }
    freeIterator(iterator);


    int32_t age = 216;
    FieldValue fieldValue = {&age, sizeof (int32_t)};
    Predicate predicate = {&fieldValue, "Age", LESS_OR_EQUALS};
    deleteRecordFromTable(file, "Users", &predicate, 1);

    printMetaTableRecords(file);
    EntityRecord *entityRecord1 = buildEntityRecord();
    int32_t amountData = 1000;
    for (int32_t i = 0; i < amountData; i++) {
        int32_t *newData = malloc(sizeof(int32_t));
        *newData = i;
        entityRecord1->fields[2].data = newData;
        insertRecordIntoTable(file, entityRecord1, "Users");
        if (i < amountData - 1) {
            free(newData);
        }
    }
    deleteRecordFromTable(file, "Users", &predicate, 1);
    freeEntityRecord(entityRecord1, 5);
    printf("\n");
    printMetaTableRecords(file);
    Iterator *iterator1 = readEntityRecordWithCondition(file, "Users", NULL, 0);
    while (hasNext(iterator1, file)) {
        EntityRecord *entityRecord = next(iterator1, file);
        printEntityRecord(entityRecord, 5, nameTypeBlocks);
        freeEntityRecord(entityRecord, 5);
    }
    freeIterator(iterator);

    fclose(file);
}
