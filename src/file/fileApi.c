#include <string.h>
#include "../../include/file/tableBlocks.h"
#include "stdio.h"
#include "stdlib.h"
#include "../../include/file/dataBlocks.h"
#include "../../include/file/allocator.h"
#include "../../include/query/query.h"
#include "../../include/file/fileApi.h"
#include "../../include/file/iterator.h"
#include <inttypes.h>


#define BLOCK_SIZE sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection)
// Размер буфера для того чтобы поместилось число типа uint64_t как строка 20-знаков + нуль терминатор
#define BUFFER_SIZE 21
#define NORMAL_SPACE 200

void writeEmptyTablesBlock(FILE *file) {
    struct defineTablesBlock *data = malloc(sizeof(struct defineTablesBlock));
    if (data == NULL) {
        printf("error allocation memory");
        return;
    }
    data->countTables = 0;
    data->emptySpaceOffset = sizeof(struct defineTablesBlock);
    fseek(file, 0, SEEK_SET);
    fwrite(data, sizeof(struct defineTablesBlock), 1, file);
    optimiseSpaceInFile(file);
    free(data);
}


uint32_t readTablesCount(FILE *file) {
    uint32_t tablesCount;
    fseek(file, 0, SEEK_SET);
    fread(&tablesCount, sizeof(uint32_t), 1, file);
    return tablesCount;
}


void writeTableCount(FILE *file, uint32_t tablesCount) {
    fseek(file, 0, SEEK_SET);
    fwrite(&tablesCount, sizeof(uint32_t), 1, file);
}


uint64_t readEmptySpaceOffset(FILE *file) {
    uint64_t emptyOffset;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(uint64_t)), SEEK_SET);
    fread(&emptyOffset, sizeof(uint64_t), 1, file);
    return emptyOffset;
}


void writeEmptySpaceOffset(FILE *file, uint64_t offset) {
    uint64_t emptySpaceOffset = offset;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(uint64_t)), SEEK_SET);
    fwrite(&emptySpaceOffset, sizeof(uint64_t), 1, file);
}


struct tableOffsetBlock *readTableOffsetBlock(FILE *file, uint16_t tablePosition) {
    if (tablePosition > 1000) {
        printf("Your table number is too big");
        return NULL;
    }
    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
    fseek(file, (sizeof(uint32_t) + (sizeof(struct tableOffsetBlock) * tablePosition)), SEEK_SET);
    fread(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
    return tableOffsetBlock;
}


uint64_t findOffsetForTableOffsetBlock(FILE *file) {
    unsigned char *buffer = malloc(sizeof(struct tableOffsetBlock) * MAX_TABLES);
    fseek(file, sizeof(uint32_t), SEEK_SET);
    fread(buffer, sizeof(struct tableOffsetBlock) * MAX_TABLES, 1, file);
    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof (struct tableOffsetBlock));
    for (uint64_t i = 0; i < MAX_TABLES; i++) {
        uint64_t offset = (sizeof(struct tableOffsetBlock) * i);
        memcpy(tableOffsetBlock, buffer + offset, sizeof(struct tableOffsetBlock));
        if (!(tableOffsetBlock->isActive)) {
            free(buffer);
            fseek(file, 0, SEEK_SET);
            uint32_t tableCount = readTablesCount(file);
            tableCount++;
            writeTableCount(file, tableCount);
            return offset + sizeof(uint32_t);
        }
    }
    free(tableOffsetBlock);
    free(buffer);
    return 0;
}


void writeTableOffsetBlock(FILE *file, struct tableOffsetBlock *tableOffsetBlock) {
    uint64_t offset = findOffsetForTableOffsetBlock(file);
    fseek(file, offset, SEEK_SET);
    fwrite(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
}


uint64_t countNeededSpace(struct EntityRecord *entityRecord, uint8_t fieldsNumber) {
    uint16_t neededSpace = sizeof(struct recordId);
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        neededSpace += sizeof(uint64_t);
        neededSpace += entityRecord->fields[i].dataSize;
    }
    neededSpace += sizeof (struct linkNext);
    return neededSpace;
}

void utilInsert(FILE * file, uint64_t offset, struct headerSection headerSection, uint16_t fieldsNumber, uint16_t beforeWriteOffset,
        struct EntityRecord *entityRecord) {
    fseek(file, offset + sizeof(struct headerSection) + headerSection.startEmptySpaceOffset, SEEK_SET);
    uint16_t writtenData = 0;
    if (entityRecord->linkNext == NULL || entityRecord->linkNext->blockOffset == 0) {
        struct linkNext linkNext = {0, 0, 0, 0, 0};
        uint16_t idPosition = abs(headerSection.endEmptySpaceOffset - BLOCK_DATA_SIZE) / sizeof (struct recordId);
        linkNext.idPosition = idPosition;
        writtenData += fwrite(&linkNext, sizeof (struct linkNext), 1, file) * sizeof (struct  linkNext);
    } else {
        uint16_t idPosition = abs(headerSection.endEmptySpaceOffset - BLOCK_DATA_SIZE) / sizeof (struct recordId);
        entityRecord->linkNext->idPosition = idPosition;
        writtenData += fwrite(entityRecord->linkNext, sizeof (struct linkNext), 1, file) * sizeof (struct  linkNext);
    }
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        struct FieldValue *field = &entityRecord->fields[i];
        writtenData += fwrite(&field->dataSize, sizeof(uint64_t), 1, file) * sizeof(uint64_t);
        writtenData += fwrite(field->data, 1, field->dataSize, file);
    }
    uint16_t length = writtenData;
    uint16_t offsetRecord = beforeWriteOffset;
    struct recordId recordId;
    recordId.offset = offsetRecord;
    recordId.length = length;
    uint64_t recordIdOffset =
            offset + sizeof(struct headerSection) + headerSection.endEmptySpaceOffset - sizeof(struct recordId);
    fseek(file, recordIdOffset, SEEK_SET);
    fwrite(&recordId, sizeof(struct recordId), 1, file);
    fseek(file, offset, SEEK_SET);
    headerSection.endEmptySpaceOffset -= sizeof(struct recordId);
    headerSection.startEmptySpaceOffset += writtenData;
    headerSection.recordsNumber++;
    fwrite(&headerSection, sizeof(struct headerSection), 1, file);
}

void utilAddBlock(FILE* file, uint64_t offset, struct headerSection headerSection, struct tableOffsetBlock *tableOffsetBlock) {
    fseek(file, offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
    struct specialDataSection specialDataSection;
    fread(&specialDataSection, sizeof(struct specialDataSection), 1, file);
    uint64_t newBlockOffset = allocateBlock(file, offset, headerSection.pageNumber + 1);
    specialDataSection.nextBlockOffset = newBlockOffset;
    tableOffsetBlock->lastTableBLockOffset = newBlockOffset;
    fseek(file, offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
    fwrite(&specialDataSection, sizeof(struct specialDataSection), 1, file);
}

void updateTableOffsetBlock(struct tableOffsetBlock *tableOffsetBlock, struct linkNext *linkNext) {
    uint8_t fieldsNumber = tableOffsetBlock->fieldsNumber;
    uint8_t startPosition;
    if (linkNext->positionInField != 0) {
        tableOffsetBlock->fieldsNumber -= (linkNext->fieldNumber);
        startPosition = linkNext->fieldNumber + 1;
    } else {
        tableOffsetBlock->fieldsNumber -= linkNext->fieldNumber;
        startPosition = linkNext->fieldNumber;
    }
    uint8_t j = 0;
    for (uint8_t i = startPosition; i < fieldsNumber; i++) {
        tableOffsetBlock->nameTypeBlock[j] = tableOffsetBlock->nameTypeBlock[i];
        j++;
    }
}

void insertRecord(FILE *file, struct EntityRecord *entityRecord, struct tableOffsetBlock *tableOffsetBlock) {
    struct headerSection headerSection;
    fseek(file, tableOffsetBlock->lastTableBLockOffset, SEEK_SET);
    fread(&headerSection, sizeof(struct headerSection), 1, file);
    uint16_t fieldsNumber = tableOffsetBlock->fieldsNumber;
    uint64_t offset = tableOffsetBlock->lastTableBLockOffset;
    uint16_t space = abs(headerSection.startEmptySpaceOffset - headerSection.endEmptySpaceOffset);
    uint16_t beforeWriteOffset = headerSection.startEmptySpaceOffset;
    uint64_t neededSpace = countNeededSpace(entityRecord, fieldsNumber);
    if (neededSpace <= space) {
        utilInsert(file, offset, headerSection, fieldsNumber, beforeWriteOffset, entityRecord);
    } else {
        if (space <= NORMAL_SPACE) {
            utilAddBlock(file, offset, headerSection, tableOffsetBlock);
            insertRecord(file, entityRecord, tableOffsetBlock);
        } else {
            uint16_t capacity = (space - sizeof (struct linkNext) - sizeof (struct recordId));
            struct EntityRecord **entities = separateEntityRecord(entityRecord, capacity, fieldsNumber, tableOffsetBlock->nameTypeBlock);
            entities[0]->linkNext->blockOffset = readEmptySpaceOffset(file);
            utilInsert(file, offset, headerSection, fieldsNumber, beforeWriteOffset, entities[0]);
            utilAddBlock(file, offset, headerSection, tableOffsetBlock);
            updateTableOffsetBlock(tableOffsetBlock, entities[0]->linkNext);
            insertRecord(file, entities[1], tableOffsetBlock);
        }
    }
}

struct EntityRecord *readRecord(FILE *file, uint16_t idPosition, uint64_t offset, uint16_t fieldsNumber) {
    idPosition++;
    struct headerSection *headerSection = malloc(sizeof(struct headerSection));
    fseek(file, offset, SEEK_SET);
    unsigned char *buffer = malloc(BLOCK_SIZE);
    fread(buffer, BLOCK_SIZE, 1, file);
    memcpy(headerSection, buffer, sizeof(struct headerSection));
    uint64_t recordIdOffset = sizeof(struct headerSection) + BLOCK_DATA_SIZE - (sizeof(struct recordId) * idPosition);
    struct recordId *recordId = malloc(sizeof(struct recordId));
    memcpy(recordId, buffer + (recordIdOffset), sizeof(struct recordId));
    fseek(file, offset + sizeof(struct headerSection) + recordId->offset, SEEK_SET);
    struct EntityRecord *entityRecord = malloc(sizeof(struct EntityRecord));
    struct FieldValue *fields = malloc(sizeof(struct FieldValue) * fieldsNumber);
    uint32_t readingOffset = sizeof(struct headerSection) + recordId->offset;
    struct linkNext *linkNext = malloc(sizeof (struct linkNext));
    memcpy(linkNext, buffer + readingOffset, sizeof (struct linkNext));
    readingOffset += sizeof (struct linkNext);
    if (linkNext->blockOffset == 0) {
        for (uint16_t i = 0; i < fieldsNumber; i++) {
            struct FieldValue *field = malloc(sizeof(struct FieldValue));
            memcpy(&field->dataSize, buffer + readingOffset, sizeof(uint64_t));
            readingOffset += sizeof(uint64_t);
            field->data = malloc(field->dataSize);
            memcpy(field->data, buffer + readingOffset, field->dataSize);
            readingOffset += field->dataSize;
            fields[i] = *field;
            free(field);
        }
    } else {
        uint16_t position;
        if (linkNext->positionInField == 0) {
            position = fieldsNumber - 1;
        } else {
            position = fieldsNumber;
        }
        for (uint16_t i = 0; i < position; i++) {
            struct FieldValue *field = malloc(sizeof(struct FieldValue));
            memcpy(&field->dataSize, buffer + readingOffset, sizeof(uint64_t));
            readingOffset += sizeof(uint64_t);
            field->data = malloc(field->dataSize);
            memcpy(field->data, buffer + readingOffset, field->dataSize);
            readingOffset += field->dataSize;
            fields[i] = *field;
            free(field);
        }
        entityRecord->fields = fields;
        entityRecord->linkNext = linkNext;
        struct EntityRecord *entityRecord1 = readRecord(file, linkNext->idPosition, linkNext->blockOffset, fieldsNumber - linkNext->fieldNumber);
        struct EntityRecord *entityRecords = compoundEntityRecords(entityRecord, entityRecord1, fieldsNumber);
        free(buffer);
        free(headerSection);
        free(recordId);
        return entityRecords;
    }
    free(buffer);
    entityRecord->linkNext = linkNext;
    entityRecord->fields = fields;
    free(headerSection);
    free(recordId);
    return entityRecord;
}


struct tableOffsetBlock *findTableOffsetBlock(FILE *file, const char *tableName) {
    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
    fseek(file, sizeof(uint32_t), SEEK_SET);
    for (uint16_t i = 0; i < MAX_TABLES; i++) {
        fread(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
        if (strcmp(tableOffsetBlock->tableName, tableName) == 0) {
            return tableOffsetBlock;
        }
    }
    free(tableOffsetBlock);
    return NULL;
}


void deleteTableOffsetBlock(FILE *file, const char *tableName) {
    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
    fseek(file, sizeof(uint32_t), SEEK_SET);
    for (uint16_t i = 0; i < MAX_TABLES; i++) {
        fread(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
        if (strcmp(tableOffsetBlock->tableName, tableName) == 0) {
            tableOffsetBlock->isActive = false;
            fseek(file, -sizeof(struct tableOffsetBlock), SEEK_CUR);
            fwrite(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
            uint32_t tableCount = readTablesCount(file);
            tableCount--;
            writeTableCount(file, tableCount);
            break;
        }
    }
    free(tableOffsetBlock);
}

void insertRecordIntoTable(FILE *file, struct EntityRecord *entityRecord, const char *tableName) {
    struct tableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
    if (tableOffsetBlock == NULL) {
        printf("There is no %s table\n", tableName);
        return;
    } else {
        struct headerSection headerSection;
        fseek(file, tableOffsetBlock->firsTableBlockOffset, SEEK_CUR);
        fread(&headerSection, sizeof(struct headerSection), 1, file);
        insertRecord(file, entityRecord, tableOffsetBlock);
        free(tableOffsetBlock);
        return;
    }
}

struct iterator *readEntityRecordWithCondition(FILE *file, const char *tableName, struct predicate *predicate,
                                               uint8_t predicateNumber) {
    struct tableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
    if (tableOffsetBlock == NULL) {
        printf("there is no %s table\n", tableName);
        return NULL;
    }
    struct iterator *iterator = malloc(sizeof(struct iterator));
    struct NameTypeBlock *nameTypeBlocks = malloc(sizeof (struct NameTypeBlock) * MAX_FIELDS);
    iterator->nameTypeBlock = nameTypeBlocks;
    iterator->predicate = predicate;
    iterator->predicateNumber = predicateNumber;
    iterator->blockOffset = tableOffsetBlock->firsTableBlockOffset;
    iterator->currentPositionInBlock = 0;
    iterator->fieldsNumber = tableOffsetBlock->fieldsNumber;
    memcpy(iterator->nameTypeBlock, tableOffsetBlock->nameTypeBlock, sizeof (struct NameTypeBlock) * MAX_FIELDS);
    free(tableOffsetBlock);
    return iterator;
}

void reverseDataArray(struct recordId *array, size_t num_elements) {
    uint16_t start = 0;
    int16_t end = num_elements - 1;

    while (start < end) {
        struct recordId temp = array[start];
        array[start] = array[end];
        array[end] = temp;
        start++;
        end--;
    }
}

void rebuildArrayOfRecordIds(unsigned char *buffer, struct recordId *recordIdArray, uint8_t recordsNumber,
                             uint16_t positionToDelete, uint64_t deletedRecordLength) {
    for (uint16_t i = 0; i < recordsNumber - 1; i++) {
        if (i < (positionToDelete)) {
            memcpy(&recordIdArray[i],
                   buffer + sizeof(struct headerSection) + BLOCK_DATA_SIZE - (i + 1) * sizeof(struct recordId),
                   sizeof(struct recordId));
        } else {
            memcpy(&recordIdArray[i],
                   buffer + sizeof(struct headerSection) + BLOCK_DATA_SIZE - (i + 2) * sizeof(struct recordId),
                   sizeof(struct recordId));
            recordIdArray[i].offset -= deletedRecordLength;
        }
    }
    reverseDataArray(recordIdArray, (recordsNumber - 1));
}

void deleteRecord(FILE *file, struct iterator *iterator, unsigned char *buffer) {
    struct headerSection headerSection;
    struct specialDataSection specialDataSection;
    struct recordId recordId;
    fseek(file, iterator->blockOffset, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, file);
    memcpy(&headerSection, buffer, sizeof(struct headerSection));
    memcpy(&specialDataSection, buffer + sizeof(struct headerSection) + BLOCK_DATA_SIZE,
           sizeof(struct specialDataSection));
    memcpy(&recordId, buffer + sizeof(struct headerSection) + BLOCK_DATA_SIZE -
                      sizeof(struct recordId) * iterator->currentPositionInBlock, sizeof(struct recordId));
    struct linkNext *linkNext = malloc(sizeof (struct linkNext));
    memcpy(linkNext, buffer + sizeof (struct headerSection) + recordId.offset, sizeof (struct linkNext));
    if (linkNext->blockOffset == 0) {
        uint32_t bufferBeforeSize = recordId.offset;
        unsigned char *bufferBefore = malloc(bufferBeforeSize);
        memcpy(bufferBefore, buffer + sizeof(struct headerSection), recordId.offset);
        struct recordId *recordIdArray = malloc(sizeof(struct recordId) * (headerSection.recordsNumber - 1));
        rebuildArrayOfRecordIds(buffer, recordIdArray, headerSection.recordsNumber, iterator->currentPositionInBlock,
                                recordId.length);
        uint32_t bufferAfterSize = headerSection.endEmptySpaceOffset - (recordId.offset + recordId.length);
        uint32_t bufferAfterStartOffset = sizeof(struct headerSection) + recordId.offset + recordId.length;
        unsigned char *bufferAfter = malloc(bufferAfterSize);
        memcpy(bufferAfter, buffer + bufferAfterStartOffset, bufferAfterSize);
        headerSection.recordsNumber--;
        headerSection.startEmptySpaceOffset -= recordId.length;
        headerSection.endEmptySpaceOffset += sizeof(struct recordId);
        fseek(file, iterator->blockOffset, SEEK_SET);
        fwrite(&headerSection, sizeof(struct headerSection), 1, file);
        fwrite(bufferBefore, bufferBeforeSize, 1, file);
        fwrite(bufferAfter, bufferAfterSize, 1, file);
        uint32_t recordIdsOffset = iterator->blockOffset + sizeof(struct headerSection) + BLOCK_DATA_SIZE -
                                   sizeof(struct recordId) * headerSection.recordsNumber;
        fseek(file, recordIdsOffset, SEEK_SET);
        fwrite(recordIdArray, sizeof(struct recordId) * headerSection.recordsNumber, 1, file);
        fflush(file);
        free(bufferBefore);
        free(bufferAfter);
        free(recordIdArray);
    } else {
    }
}


void deleteRecordFromTable(FILE *file, const char *tableName, struct predicate *predicate,
                           uint8_t predicateNumber) {
    uint64_t recordsNumber = 0;
    struct iterator *iterator = readEntityRecordWithCondition(file, tableName, predicate, predicateNumber);
    while (hasNext(iterator, file)) recordsNumber++;

    unsigned char *buffer = malloc(BLOCK_SIZE);
    for (uint64_t i = 0; i < recordsNumber; i++) {
        iterator = readEntityRecordWithCondition(file, tableName, predicate, predicateNumber);
        hasNext(iterator, file);
        deleteRecord(file, iterator, buffer);
    }
    free(buffer);
}

void updateRecordFromTable(FILE *file, const char *tableName, struct predicate *predicate,
                           uint8_t predicateNumber, struct EntityRecord *entityRecord) {
    struct iterator *iterator = readEntityRecordWithCondition(file, tableName, predicate, predicateNumber);
    unsigned char *buffer = malloc(BLOCK_SIZE);
    if (hasNext(iterator, file)) {
        deleteRecord(file, iterator, buffer);
        insertRecordIntoTable(file, entityRecord, tableName);
    }
    free(buffer);
}

void optimiseSpaceInFile(FILE *file) {
    struct NameTypeBlock *nameTypeBlock = initNameTypeBlock("Offset", STRING);
    struct tableOffsetBlock *writtenTableMetaData = initTableOffsetBlock(file, "Meta", 1, nameTypeBlock);
    writeTableOffsetBlock(file, writtenTableMetaData);
}

void deleteTable(const char *tableName, FILE *file) {
    struct tableOffsetBlock *tableOffsetBlock = findTableOffsetBlock(file, tableName);
    uint64_t offset = tableOffsetBlock->firsTableBlockOffset;
    struct specialDataSection specialDataSection;
    struct EntityRecord entityRecord;
    struct FieldValue fieldValue;
    while (offset != 0) {
        fseek(file, offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
        fread(&specialDataSection, sizeof(struct specialDataSection), 1, file);
        char buffer[BUFFER_SIZE];
        snprintf(buffer, sizeof(char) * BUFFER_SIZE, "%" PRIu64, offset);
        fieldValue.data = &buffer;
        fieldValue.dataSize = sizeof(char) * BUFFER_SIZE;
        entityRecord.fields = &fieldValue;
        entityRecord.linkNext = NULL;
        insertRecordIntoTable(file, &entityRecord, "Meta");
        offset = specialDataSection.nextBlockOffset;
    }
    deleteTableOffsetBlock(file, tableName);
}

struct FieldValue **separateString(struct FieldValue *fieldValue, uint32_t capacity) {
    unsigned char *buffer1 = malloc(capacity);
    struct FieldValue *fieldValue1 = malloc(sizeof(struct FieldValue));
    fieldValue1->dataSize = capacity;
    memcpy(buffer1, fieldValue->data, capacity);
    fieldValue1->data = buffer1;

    unsigned char *buffer2 = malloc(fieldValue->dataSize - capacity);
    struct FieldValue *fieldValue2 = malloc(sizeof(struct FieldValue));
    fieldValue2->dataSize = fieldValue->dataSize - capacity;
    memcpy(buffer2, fieldValue->data + capacity, (fieldValue->dataSize - capacity));
    fieldValue2->data = buffer2;

    struct FieldValue **fieldValues = (struct FieldValue **) malloc(2 * sizeof(struct FieldValue *));
    fieldValues[0] = fieldValue1;
    fieldValues[1] = fieldValue2;
    return fieldValues;
}

struct FieldValue *concatenateFieldValues(struct FieldValue *fieldValue1, struct FieldValue *fieldValue2) {
    unsigned char *buffer = malloc(fieldValue1->dataSize + fieldValue2->dataSize);
    memcpy(buffer, fieldValue1->data, fieldValue1->dataSize);
    memcpy(buffer + fieldValue1->dataSize, fieldValue2->data, fieldValue2->dataSize);
    struct FieldValue *fieldValue = malloc(sizeof(struct FieldValue));
    fieldValue->dataSize = (fieldValue1->dataSize + fieldValue2->dataSize);
    fieldValue->data = buffer;
    free(fieldValue1->data);
    free(fieldValue2->data);
    return fieldValue;
}

struct EntityRecord **separateEntityRecord(struct EntityRecord *entityRecord, int64_t capacity,
                                           uint8_t fieldsNumber, struct NameTypeBlock* nameTypeBlock) {
    struct EntityRecord *entityRecord1 = malloc(sizeof(struct EntityRecord));
    struct EntityRecord *entityRecord2 = malloc(sizeof(struct EntityRecord));
    entityRecord1->fields = malloc(sizeof (struct FieldValue) * fieldsNumber);
    entityRecord2->fields = malloc(sizeof (struct FieldValue) * fieldsNumber);
    struct linkNext *linkNext1 = malloc(sizeof (struct linkNext));
    struct linkNext *linkNext2 = malloc(sizeof (struct linkNext));
    entityRecord1->linkNext = linkNext1;
    entityRecord2->linkNext = linkNext2;
    uint8_t fieldsNumber2 = 0;
    bool isSeparated = false;
    for (uint8_t i = 0; i < fieldsNumber; i++) {
        capacity -= sizeof(uint64_t);
        if (capacity < 0) {
            if (!isSeparated) {
                isSeparated = true;
                entityRecord1->linkNext->fieldNumber = i;
                entityRecord1->linkNext->positionInField = 0;
            }
            entityRecord2->fields[fieldsNumber2].data = entityRecord->fields[i].data;
            entityRecord2->fields[fieldsNumber2].dataSize = entityRecord->fields[i].dataSize;
            fieldsNumber2++;
            continue;
        }
        capacity -= entityRecord->fields[i].dataSize;
        if (capacity < 0) {
            if (nameTypeBlock[i].dataType == STRING) {
                capacity += entityRecord->fields[i].dataSize;
                struct FieldValue **fieldValues = separateString(&entityRecord->fields[i], capacity);
                entityRecord1->fields[i].data = fieldValues[0]->data;
                entityRecord1->fields[i].dataSize = fieldValues[0]->dataSize;

                if (!isSeparated) {
                    isSeparated = true;
                    entityRecord1->linkNext->fieldNumber = i;
                    entityRecord1->linkNext->positionInField = entityRecord1->fields[i].dataSize;
                }

                entityRecord2->fields[fieldsNumber2].data = fieldValues[1]->data;
                entityRecord2->fields[fieldsNumber2].dataSize = fieldValues[1]->dataSize;
                capacity -= entityRecord->fields[i].dataSize;
            } else {
                if (!isSeparated) {
                    isSeparated = true;
                    entityRecord1->linkNext->fieldNumber = i;
                    entityRecord1->linkNext->positionInField = 0;
                }
                entityRecord2->fields[fieldsNumber2].data = entityRecord->fields[i].data;
                entityRecord2->fields[fieldsNumber2].dataSize = entityRecord->fields[i].dataSize;
            }
            fieldsNumber2++;
            continue;
        }
        entityRecord1->fields[i].data = entityRecord->fields[i].data;
        entityRecord1->fields[i].dataSize = entityRecord->fields[i].dataSize;
    }
    struct EntityRecord **entityRecords = (struct EntityRecord **) malloc(2 * sizeof(struct EntityRecord *));
    entityRecords[0] = entityRecord1;
    entityRecords[1] = entityRecord2;
    free(entityRecord->fields);
    free(entityRecord);
    return entityRecords;
}

struct EntityRecord* compoundEntityRecords(struct EntityRecord* entityRecord1, struct EntityRecord* entityRecord2, uint8_t fieldsNumber) {
    struct EntityRecord *entityRecord = malloc(sizeof (struct EntityRecord));
    struct FieldValue *fields = malloc(sizeof (struct FieldValue) * fieldsNumber);
    struct linkNext *linkNext = malloc(sizeof (struct linkNext));
    entityRecord->fields = fields;
    entityRecord->linkNext = linkNext;
    uint8_t fieldsNumber2 = 0;
    for (uint8_t  i = 0; i < fieldsNumber; i++) {
        if (i < entityRecord1->linkNext->fieldNumber) {
            entityRecord->fields[i].data = entityRecord1->fields[i].data;
            entityRecord->fields[i].dataSize = entityRecord1->fields[i].dataSize;
            continue;
        }
        if (i == entityRecord1->linkNext->fieldNumber && entityRecord1->linkNext->fieldNumber >= 0) {
            struct FieldValue* fieldValue = concatenateFieldValues(&entityRecord1->fields[i], &entityRecord2->fields[fieldsNumber2]);
            entityRecord->fields[i].data = fieldValue->data;
            entityRecord->fields[i].dataSize = fieldValue->dataSize;
            fieldsNumber2++;
            continue;
        }
        entityRecord->fields[i].data = entityRecord2->fields[fieldsNumber2].data;
        entityRecord->fields[i].dataSize = entityRecord2->fields[fieldsNumber2].dataSize;
        fieldsNumber2++;
    }
    entityRecord->linkNext->fieldNumber = entityRecord2->linkNext->fieldNumber;
    entityRecord->linkNext->positionInField = entityRecord2->linkNext->positionInField;
    entityRecord->linkNext->blockOffset = entityRecord2->linkNext->blockOffset;
    entityRecord->linkNext->offsetInBlock = entityRecord2->linkNext->offsetInBlock;

    free(entityRecord1->fields);
    free(entityRecord1);
    free(entityRecord2->fields);
    free(entityRecord2);

    return entityRecord;
}
