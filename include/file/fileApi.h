#ifndef LAB1_FILEAPI_H
#define LAB1_FILEAPI_H

#include <stdlib.h>
#include <stdio.h>
#include "../../include/data/data.h"
#include "dataBlocks.h"
#include "iterator.h"

void writeEmptyTablesBlock(FILE *file);

DefineTablesBlock *readTablesBlock(FILE *file);

uint32_t readTablesCount(FILE *file);

uint64_t readEmptySpaceOffset(FILE *file);

void writeTableCount(FILE *file, uint32_t tablesCount);

void writeEmptySpaceOffset(FILE *file, uint64_t offset);

TableOffsetBlock *readTableOffsetBlock(FILE *file, uint16_t tablePosition);

uint64_t findOffsetForTableOffsetBlock(FILE *file);

void writeTableOffsetBlock(FILE *file, TableOffsetBlock *tableOffsetBlock);

void insertRecord(FILE *file, EntityRecord *entityRecord, TableOffsetBlock *tableOffsetBlock);

EntityRecord *readRecord(FILE *file, uint16_t idPosition, uint64_t offset, uint16_t fieldsNumber);

void insertRecordIntoTable(FILE *file, EntityRecord *entityRecord, const char *tableName);

Iterator *readEntityRecordWithCondition(FILE *file, const char *tableName, Predicate *predicate,
                                        uint8_t predicateNumber);

void deleteRecordFromTable(FILE *file, const char *tableName, Predicate *predicate,
                           uint8_t predicateNumber);

void rebuildArrayOfRecordIds(unsigned char *buffer, RecordId *recordIdArray, uint8_t recordsNumber,
                             uint16_t positionToDelete, uint64_t deletedRecordLength);

void updateRecordsFromTable(FILE *file, const char *tableName, Predicate *predicate,
                            uint8_t predicateNumber, EntityRecord *entityRecord);

TableOffsetBlock *findTableOffsetBlock(FILE *file, const char *tableName);

void optimiseSpaceInFile(FILE *file);

void deleteTable(const char *tableName, FILE *file);

FieldValue **separateString(FieldValue *fieldValue, uint32_t capacity);

FieldValue *concatenateFieldValues(FieldValue *fieldValue1, FieldValue *fieldValue2);

EntityRecord **separateEntityRecord(EntityRecord *entityRecord, int64_t capacity,
                                    uint8_t fieldsNumber, NameTypeBlock *nameTypeBlock);

EntityRecord *compoundEntityRecords(EntityRecord *entityRecord1, EntityRecord *entityRecord2, uint8_t fieldsNumber);

#endif //LAB1_FILEAPI_H
