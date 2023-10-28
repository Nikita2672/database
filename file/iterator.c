#include "iterator.h"
#include "../data/data.h"
#include "fileApi.h"
#include "dataBlocks.h"
#include "../query/query.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "../util/util.h"

bool hasNext(struct iterator *iterator, FILE *file) {
    struct headerSection headerSection;
    fseek(file, iterator->blockOffset, SEEK_SET);
    fread(&headerSection, sizeof(struct headerSection), 1, file);
    bool hasNextVariable = false;
    for (uint16_t i = iterator->currentPositionInBlock; i < headerSection.recordsNumber; i++) {
        struct EntityRecord *entityRecord = readRecord(file, i, iterator->blockOffset, iterator->fieldsNumber);
        bool valid = true;
        for (uint16_t j = 0; j < iterator->predicateNumber; j++) {
            bool result = checkPredicate(&iterator->predicate[j], entityRecord, iterator->fieldsNumber,
                                         iterator->nameTypeBlock);
            if (!result) {
                valid = false;
                break;
            }
        }
//        freeEntityRecord(entityRecord, iterator->fieldsNumber);
        if (valid) {
            hasNextVariable = true;
            iterator->currentPositionInBlock = (i + 1);
            return hasNextVariable;
        }
    }
    fseek(file, iterator->blockOffset + sizeof(struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
    struct specialDataSection specialDataSection;
    fread(&specialDataSection, sizeof(struct specialDataSection), 1, file);
    if (specialDataSection.nextBlockOffset != 0) {
        iterator->currentPositionInBlock = 0;
        iterator->blockOffset = specialDataSection.nextBlockOffset;
        return hasNext(iterator, file);
    } else {
        return false;
    }
}


struct EntityRecord *next(struct iterator *iterator, FILE *file) {
    return readRecord(file, iterator->currentPositionInBlock - 1, iterator->blockOffset, iterator->fieldsNumber);
}

struct EntityRecord *concatenateEntityRecords(struct EntityRecord *entityRecord1,
                                              struct EntityRecord *entityRecord2, uint8_t fieldsNumber1,
                                              uint8_t fieldsNumber2) {
    if (entityRecord2 == NULL) return entityRecord1;
    struct FieldValue *newFields = malloc(sizeof(struct FieldValue) * (fieldsNumber1 + fieldsNumber2));
    memcpy(newFields, entityRecord1->fields, sizeof(struct FieldValue) * fieldsNumber1);
    memcpy(newFields + fieldsNumber1, entityRecord2->fields,sizeof(struct FieldValue) * fieldsNumber2);
    struct EntityRecord *entityRecord = malloc(sizeof(struct EntityRecord));
    entityRecord->fields = newFields;
    return entityRecord;
}


struct EntityRecord *nextWithJoin(struct iterator *iterator1, const char *tableName,
                                  FILE *file, uint8_t fieldNumber, char *fieldName) {
    struct EntityRecord *entityRecord1 = NULL;
    if (hasNext(iterator1, file)) {
        entityRecord1 = next(iterator1, file);
    } else {
        return NULL;
    }
    struct FieldValue fieldValue = entityRecord1->fields[fieldNumber];
    struct predicate predicate = {&fieldValue, fieldName, EQUALS};
    struct iterator *iterator2 = readEntityRecordWithCondition(file, tableName, &predicate, 1);
    struct EntityRecord *entityRecord2 = NULL;
    if (hasNext(iterator2, file)) {
        entityRecord2 = next(iterator2, file);
    }
    struct EntityRecord *entityRecord = concatenateEntityRecords(entityRecord1, entityRecord2,
                                                                 iterator1->fieldsNumber,
                                                                 iterator2->fieldsNumber);
    return entityRecord;
}
