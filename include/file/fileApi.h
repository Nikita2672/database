#ifndef LAB1_FILEAPI_H
#define LAB1_FILEAPI_H

#include <stdlib.h>
#include <stdio.h>
#include "../../include/data/data.h"
#include "dataBlocks.h"

void writeEmptyTablesBlock(FILE *file);

struct defineTablesBlock *readTablesBlock(FILE *file);

uint32_t readTablesCount(FILE *file);

uint64_t readEmptySpaceOffset(FILE *file);

void writeTableCount(FILE *file, uint32_t tablesCount);

void writeEmptySpaceOffset(FILE *file, uint64_t offset);

struct tableOffsetBlock *readTableOffsetBlock(FILE *file, uint16_t tablePosition);

uint64_t findOffsetForTableOffsetBlock(FILE *file);

void writeTableOffsetBlock(FILE *file, struct tableOffsetBlock *tableOffsetBlock);

void insertRecord(FILE *file, struct EntityRecord *entityRecord, struct tableOffsetBlock *tableOffsetBlock);

struct EntityRecord *readRecord(FILE *file, uint16_t idPosition, uint64_t offset, uint16_t fieldsNumber);

void insertRecordIntoTable(FILE *file, struct EntityRecord *entityRecord, const char *tableName);

struct iterator *readEntityRecordWithCondition(FILE *file, const char *tableName, struct predicate *predicate,
                                               uint8_t predicateNumber);

void deleteRecordFromTable(FILE *file, const char *tableName, struct predicate *predicate,
                           uint8_t predicateNumber);

void rebuildArrayOfRecordIds(unsigned char *buffer, struct recordId *recordIdArray, uint8_t recordsNumber,
                             uint16_t positionToDelete, uint64_t deletedRecordLength);

void updateRecordFromTable(FILE *file, const char *tableName, struct predicate *predicate,
                           uint8_t predicateNumber, struct EntityRecord *entityRecord);

struct tableOffsetBlock *findTableOffsetBlock(FILE *file, const char *tableName);

void optimiseSpaceInFile(FILE *file);

void deleteTable(const char *tableName, FILE *file);

struct FieldValue **separateString(struct FieldValue *fieldValue, uint32_t capacity);

struct FieldValue *concatenateFieldValues(struct FieldValue *fieldValue1, struct FieldValue *fieldValue2);

struct EntityRecord **separateEntityRecord(struct EntityRecord *entityRecord, int64_t capacity,
                                           uint8_t fieldsNumber, struct NameTypeBlock *nameTypeBlock);

struct EntityRecord *
compoundEntityRecords(struct EntityRecord *entityRecord1, struct EntityRecord *entityRecord2, uint8_t fieldsNumber);

#endif //LAB1_FILEAPI_H
