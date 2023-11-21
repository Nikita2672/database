#include "util/testPerfomance.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
#define FILE_INSERT_DATA "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\insert_performance_data.txt"
#define FILE_UPDATE_DATA "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\update_performance_data.txt"
#define FILE_DELETE_DATA "C:\\Users\\iwaa0\\CLionProjects\\llp\\database\\test\\delete_performance_data.txt"
#else
#define FILE_INSERT_DATA "/home/iwaa0303/CLionProjects/database/src/util/insert_performance_data.txt"
#define FILE_UPDATE_DATA "/home/iwaa0303/CLionProjects/database/src/util/update_performance_data.txt"
#define FILE_DELETE_DATA "/home/iwaa0303/CLionProjects/database/src/util/delete_performance_data.txt"
#endif


void testInsertPerformance(uint32_t amountData) {
    FILE *fileData = fopen(FILE_INSERT_DATA, "r+");
    struct timeval start, end;
    double elapsed_time;
    FILE *file = fopen(FILE_NAME, "rb+");
    cutFile(file, 0);
    writeEmptyTablesBlock(file);

    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
    NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);

    // 1 table
    NameTypeBlock nameTypeBlocks[5] = {
            *nameTypeBlock1,
            *nameTypeBlock2,
            *nameTypeBlock3,
            *nameTypeBlock4,
            *nameTypeBlock5
    };
    TableOffsetBlock *writtenTableOffsetBlock1 = initTableOffsetBlock(file, "Users", 5, nameTypeBlocks);
    writeTableOffsetBlock(file, writtenTableOffsetBlock1);

    char *name;
    name = "Nikita";
    char *surname;
    surname = "Ivanov";
    int32_t *age = malloc(sizeof(int32_t));
    *age = 20;
    double *score = malloc(sizeof(double));
    *score = 123.34;
    bool *sex = malloc(sizeof(bool));
    *sex = true;
    FieldValue fieldValue1 = {name, sizeof(char) * strlen(name)};
    FieldValue fieldValue2 = {surname, sizeof(char) * strlen(surname)};
    FieldValue fieldValue3 = {age, sizeof(int32_t)};
    FieldValue fieldValue4 = {score, sizeof(score)};
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

    for (int32_t i = 0; i < amountData; i++) {
        int32_t *newData = malloc(sizeof(int32_t));
        *newData = i;
        fields[2].data = newData;
        gettimeofday(&start, NULL);
        insertRecordIntoTable(file, entityRecord, "Users");
        gettimeofday(&end, NULL);
        elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        fprintf(fileData, "%d, %f\n", i + 1, elapsed_time);
        free(newData);
    }
    fclose(file);
    fclose(fileData);
}

void testUpdatePerformance(void ) {
    struct timeval start, end;
    double elapsed_time;
    FILE *fileData = fopen(FILE_UPDATE_DATA, "r+");
    char *name;
    name = "Nikita";
    char *surname;
    surname = "Ivanov";
    int32_t *age = malloc(sizeof(int32_t));
    *age = 20;
    double *score = malloc(sizeof(double));
    *score = 123.34;
    bool *sex = malloc(sizeof(bool));
    *sex = true;
    FieldValue fieldValue1 = {name, sizeof(char) * strlen(name)};
    FieldValue fieldValue2 = {surname, sizeof(char) * strlen(surname)};
    FieldValue fieldValue3 = {age, sizeof(int32_t)};
    FieldValue fieldValue4 = {score, sizeof(score)};
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
    int32_t comparableAge = 12;
    FieldValue  fieldValue = {&comparableAge, sizeof (int32_t)};
    Predicate predicate = {&fieldValue, "Age", LESS};


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
    free(age);
    free(score);
    free(sex);
    fclose(fileData);
}

void testDeletePerformance(void ) {
    struct timeval start, end;
    double elapsed_time;
    FILE *fileData = fopen(FILE_DELETE_DATA, "r+");
    int32_t comparableAge = 12;
    FieldValue  fieldValue = {&comparableAge, sizeof (int32_t)};
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


//    NameTypeBlock *nameTypeBlock1 = initNameTypeBlock("Name", STRING);
//    NameTypeBlock *nameTypeBlock2 = initNameTypeBlock("Surname", STRING);
//    NameTypeBlock *nameTypeBlock3 = initNameTypeBlock("Age", INT);
//    NameTypeBlock *nameTypeBlock4 = initNameTypeBlock("Score", DOUBLE);
//    NameTypeBlock *nameTypeBlock5 = initNameTypeBlock("Sex", BOOL);
//    // 1 table
//    NameTypeBlock nameTypeBlocks[5] = {
//            *nameTypeBlock1,
//            *nameTypeBlock2,
//            *nameTypeBlock3,
//            *nameTypeBlock4,
//            *nameTypeBlock5
//    };
//    int32_t counter = 0;
//    printf("\n------------------------------------start-----------------------------------\n");
//
//    while (hasNext(iterator, file)) {
//        printf("id: %d ", counter);
//        counter++;
//        EntityRecord *entityRecord1 = next(iterator, file);
//        printEntityRecord(entityRecord1, 5, nameTypeBlocks);
//    }
//    printf("\n------------------------------------finish-----------------------------------\n");
//    printf("\n counter: %d", counter);
//    fclose(file);

//    Iterator *iterator = readEntityRecordWithCondition(file, "Users", NULL, 0);
//    while (hasNext(iterator, file)) {
//        EntityRecord *entityRecord1 = next(iterator, file);
//        printEntityRecord(entityRecord1, 5, nameTypeBlocks);
//    }
