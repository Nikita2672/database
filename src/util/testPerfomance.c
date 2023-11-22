#include "util/testPerfomance.h"
#include "util/unitTests.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

EntityRecord *buildEntityRecord(void) {
    char *name = malloc(sizeof(char) * (strlen("Nikita") + 1)); // +1 for the null terminator
    strcpy(name, "Nikita");

    char *surname = malloc(sizeof(char) * (strlen("Ivanov") + 1));
    strcpy(surname, "Ivanov");

    int32_t *age = malloc(sizeof(int32_t));
    *age = 20;

    double *score = malloc(sizeof(double));
    *score = 123.34;

    bool *sex = malloc(sizeof(bool));
    *sex = true;

    FieldValue fieldValue1 = {name, sizeof(char) * (strlen(name) + 1)};
    FieldValue fieldValue2 = {surname, sizeof(char) * (strlen(surname) + 1)};
    FieldValue fieldValue3 = {age, sizeof(int32_t)};
    FieldValue fieldValue4 = {score, sizeof(double)};
    FieldValue fieldValue5 = {sex, sizeof(bool)};

    FieldValue *fields = malloc(sizeof(FieldValue) * 5);
    fields[0] = fieldValue1;
    fields[1] = fieldValue2;
    fields[2] = fieldValue3;
    fields[3] = fieldValue4;
    fields[4] = fieldValue5;

    EntityRecord *entityRecord = malloc(sizeof(EntityRecord));
    entityRecord->fields = fields;
    entityRecord->linkNext = NULL;
    return entityRecord;
}

NameTypeBlock *buildNameTypeBlock(void) {
    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);
    NameTypeBlock *nameTypeBlocks = malloc(sizeof(NameTypeBlock) * 5);
    nameTypeBlocks[0] = *nameTypeBlock1;
    nameTypeBlocks[1] = *nameTypeBlock2;
    nameTypeBlocks[2] = *nameTypeBlock3;
    nameTypeBlocks[3] = *nameTypeBlock4;
    nameTypeBlocks[4] = *nameTypeBlock5;
    free(nameTypeBlock1);
    free(nameTypeBlock2);
    free(nameTypeBlock3);
    free(nameTypeBlock4);
    free(nameTypeBlock5);
    return nameTypeBlocks;
}

void testInsertPerformance(int64_t amountData) {
    FILE *fileData = fopen(FILE_INSERT_DATA, "r+");
    struct timeval start, end;
    double elapsed_time;
    FILE *file = fopen(FILE_NAME, "rb+");
    cutFile(file, 0);
    writeEmptyTablesBlock(file);

    NameTypeBlock *nameTypeBlocks = buildNameTypeBlock();
    TableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "Users", 5, nameTypeBlocks);
    writeTableOffsetBlock(file, writtenTableOffsetBlock1);

    EntityRecord *entityRecord = buildEntityRecord();

    for (int32_t i = 0; i < amountData; i++) {
        int32_t *newData = malloc(sizeof(int32_t));
        *newData = i;
        entityRecord->fields[2].data = newData;
        gettimeofday(&start, NULL);
        insertRecordIntoTable(file, entityRecord, "Users");
        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        fprintf(fileData, "%d, %f\n", i + 1, elapsed_time);
        if (i < amountData - 1) {
            free(newData);
        }
    }
    freeEntityRecord(entityRecord, 5);
    free(nameTypeBlocks);
    fclose(file);
    fclose(fileData);
}

void testUpdatePerformance(void) {
    struct timeval start, end;
    double elapsed_time;
    FILE *fileData = fopen(FILE_UPDATE_DATA, "r+");
    EntityRecord *entityRecord = buildEntityRecord();
    int32_t comparableAge = 12;
    FieldValue fieldValue = {&comparableAge, sizeof(int32_t)};
    Predicate predicate = {&fieldValue, "Age", LESS};
    int32_t *age = malloc(sizeof(int32_t));
    *age = 20;
    entityRecord->fields[2].data = age;

    for (uint16_t i = 0; i < 400; i++) {
        testInsertPerformance(i);
        FILE *file = fopen(FILE_NAME, "rb+");
        gettimeofday(&start, NULL);
        updateRecordsFromTable(file, "Users", &predicate, 1, entityRecord);
        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        fprintf(fileData, "%d, %f\n", i + 1, elapsed_time);
        fclose(file);
    }
    freeEntityRecord(entityRecord, 5);
    fclose(fileData);
}

void testDeletePerformance(void) {
    struct timeval start, end;
    double elapsed_time;
    FILE *fileData = fopen(FILE_DELETE_DATA, "r+");
    int32_t comparableAge = 12;
    FieldValue fieldValue = {&comparableAge, sizeof(int32_t)};
    Predicate predicate = {&fieldValue, "Age", LESS};
    for (uint16_t i = 0; i < 400; i++) {
        testInsertPerformance(i);
        FILE *file = fopen(FILE_NAME, "rb+");
        gettimeofday(&start, NULL);
        deleteRecordFromTable(file, "Users", &predicate, 1);
        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        fprintf(fileData, "%d, %f\n", i + 1, elapsed_time);
        fclose(file);
    }
    fclose(fileData);
}

double testDeleteInsertPerformance(int64_t insertAmount, int32_t deleteAmount, uint16_t test_number, FILE *fileDataInsert, FILE* fileDataDelete) {
    struct timeval start, end;
    double elapsed_time;
    double time;
    FILE *file = fopen(FILE_NAME, "rb+");
    EntityRecord *entityRecord = buildEntityRecord();

    for (uint8_t j = 1; j < 6; j++) {
        gettimeofday(&start, NULL);
        for (int32_t i = 0; i < 100; i++) {
            int32_t *newData = malloc(sizeof(int32_t));
            *newData = (j - 1) * 100 + i;
            entityRecord->fields[2].data = newData;
            insertRecordIntoTable(file, entityRecord, "Users");
        }
        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        fprintf(fileDataInsert, "%d, %f\n", test_number * 5 + j, elapsed_time);
        fflush(fileDataInsert);
    }
    gettimeofday(&end, NULL);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    freeEntityRecord(entityRecord, 5);
    int32_t age = deleteAmount;
    FieldValue fieldValue = {&age, sizeof(int32_t)};

    Predicate predicate = {&fieldValue, "Age", EQUALS};
    struct timeval start_del, end_del;

//    gettimeofday(&start_del, NULL);
    for (int32_t j = 1; j < 5; j++) {
        gettimeofday(&start, NULL);
        for (int32_t i = 0; i < 100; i++) {
            int32_t *newData = malloc(sizeof(int32_t));
            *newData = (j - 1) * 100 + i;
            predicate.comparableValue->data = newData;
            deleteRecordFromTable(file, "Users", &predicate, 1);
        }
        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        fprintf(fileDataDelete, "%d, %lf\n", test_number * 4 + j, elapsed_time);
        fflush(fileDataDelete);
    }
//    gettimeofday(&end_del, NULL);
//    time = (end_del.tv_sec - start_del.tv_sec) + (end_del.tv_usec - start_del.tv_usec) / 1000000.0;
//
//    fprintf(fileDataDelete, "%lf\n", time);

    Iterator *iterator = readEntityRecordWithCondition(file, "Users", NULL, 0);

    uint16_t counter = 0;
    while (hasNext(iterator, file)) {
        counter++;
    }
    printf("\n counter: %d", counter);

    fclose(file);
    return elapsed_time;
}
