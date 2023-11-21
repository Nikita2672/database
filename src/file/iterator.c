#include "../../include/file/iterator.h"
#include "../../include/data/data.h"
#include "../../include/file/fileApi.h"
#include "../../include/file/dataBlocks.h"
#include "../../include/query/query.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "../../include/util/util.h"


bool hasNext(Iterator *iterator, FILE *file) {
    HeaderSection headerSection;
    fseek(file, iterator->blockOffset, SEEK_SET);
    fread(&headerSection, sizeof(HeaderSection), 1, file);
    SpecialDataSection specialDataSection;
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fread(&specialDataSection, sizeof (specialDataSection), 1, file);
    bool hasNextVariable = false;
    for (uint16_t i = iterator->currentPositionInBlock; i < headerSection.recordsNumber; i++) {
        EntityRecord *entityRecord = readRecord(file, i, iterator->blockOffset, iterator->fieldsNumber);
        bool valid = true;
        for (uint16_t j = 0; j < iterator->predicateNumber; j++) {
            bool result = checkPredicate(&iterator->predicate[j], entityRecord, iterator->fieldsNumber,
                                         iterator->nameTypeBlock);
            if (!result) {
                valid = false;
                break;
            }
        }
        freeEntityRecord(entityRecord, iterator->fieldsNumber);
        if (valid) {
            hasNextVariable = true;
            iterator->currentPositionInBlock = (i + 1);
            return hasNextVariable;
        }
    }
    if (specialDataSection.nextBlockOffset != 0) {
        iterator->currentPositionInBlock = 0;
        iterator->blockOffset = specialDataSection.nextBlockOffset;
        return hasNext(iterator, file);
    } else {
        return false;
    }
}


EntityRecord *next(Iterator *iterator, FILE *file) {
    return readRecord(file, iterator->currentPositionInBlock - 1, iterator->blockOffset, iterator->fieldsNumber);
}

static EntityRecord *concatenateEntityRecords(EntityRecord *entityRecord1,
                                              EntityRecord *entityRecord2, uint8_t fieldsNumber1,
                                              uint8_t fieldsNumber2) {
    if (entityRecord2 == NULL) return entityRecord1;
    FieldValue *newFields = malloc(sizeof(FieldValue) * (fieldsNumber1 + fieldsNumber2));
    memcpy(newFields, entityRecord1->fields, sizeof(FieldValue) * fieldsNumber1);
    memcpy(newFields + fieldsNumber1, entityRecord2->fields, sizeof(FieldValue) * fieldsNumber2);
    EntityRecord *entityRecord = malloc(sizeof(EntityRecord));
    entityRecord->fields = newFields;
    return entityRecord;
}


EntityRecord *nextWithJoin(Iterator *iterator1, const char *tableName,
                           FILE *file, uint8_t fieldNumber, char *fieldName) {
    EntityRecord *entityRecord1 = NULL;
    if (hasNext(iterator1, file)) {
        entityRecord1 = next(iterator1, file);
    } else {
        return NULL;
    }
    FieldValue fieldValue = entityRecord1->fields[fieldNumber];
    Predicate predicate = {&fieldValue, fieldName, EQUALS};
    Iterator *iterator2 = readEntityRecordWithCondition(file, tableName, &predicate, 1);
    EntityRecord *entityRecord2 = NULL;
    if (hasNext(iterator2, file)) {
        entityRecord2 = next(iterator2, file);
    }
    EntityRecord *entityRecord = concatenateEntityRecords(entityRecord1, entityRecord2,
                                                          iterator1->fieldsNumber,
                                                          iterator2->fieldsNumber);
    return entityRecord;
}

void freeIterator(Iterator* iterator) {
    free(iterator->nameTypeBlock);
    free(iterator);
}
