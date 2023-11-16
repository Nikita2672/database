//
// Created by iwaa0303 on 16/11/23.
//

#include "util/testPerfomance.h"


void testInsertPerformance(void) {
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

    char *name = malloc(sizeof(char) * strlen("Nikita"));
    name = "Nikita";
    char *surname = malloc(sizeof(char) * strlen("Ivanov"));
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
    for (int32_t i = 0; i < 100000; i++) {
        int32_t *newData = malloc(sizeof(int32_t));
        *newData = i;
        fields[2].data = newData;
        insertRecordIntoTable(file, entityRecord, "Users");
        free(newData);
    }
    Iterator *iterator = readEntityRecordWithCondition(file, "Users", NULL, 0);
    while (hasNext(iterator, file)) {
        EntityRecord *entityRecord1 = next(iterator, file);
        printEntityRecord(entityRecord1, 5, nameTypeBlocks);
    }
    fclose(file);
}