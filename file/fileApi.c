#include <string.h>
#include "tableBlocks.h"
#include "stdio.h"
#include "stdlib.h"
#include "dataBlocks.h"
#include "allocator.h"
#include "../query/query.h"
#include "fileApi.h"
#include "iterator.h"
#include <inttypes.h>


#define BLOCK_SIZE sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection)
// Размер буфера для того чтобы поместилось число типа uint64_t как строка 20-знаков + нуль терминатор
#define BUFFER_SIZE 21

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

struct defineTablesBlock *readTablesBlock(FILE *file) {
    struct defineTablesBlock *defineTablesBlock = malloc(sizeof(struct defineTablesBlock));
    fread(defineTablesBlock, sizeof(struct defineTablesBlock), 1, file);
    return defineTablesBlock;
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
    uint64_t tableCount;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(uint64_t)), SEEK_SET);
    fread(&tableCount, sizeof(uint64_t), 1, file);
    return tableCount;
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
    struct tableOffsetBlock tableOffsetBlock;
    for (uint64_t i = 0; i < MAX_TABLES; i++) {
        uint64_t offset = (sizeof(struct tableOffsetBlock) * i);
        memcpy(&tableOffsetBlock, buffer + offset, sizeof(struct tableOffsetBlock));
        if (!(tableOffsetBlock.isActive)) {
            free(buffer);
            return offset + sizeof(uint32_t);
        }
    }
    free(buffer);
    return 0;
}

void writeTableOffsetBlock(FILE *file, struct tableOffsetBlock *tableOffsetBlock) {
    uint64_t offset = findOffsetForTableOffsetBlock(file);
    fseek(file, offset, SEEK_SET);
    fwrite(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
}

void writeDataBlock(uint64_t offset, FILE *file, struct headerSection *headerSection,
                    struct specialDataSection *specialDataSection) {
    fseek(file, offset, SEEK_SET);
    fwrite(headerSection, sizeof(struct headerSection), 1, file);
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fwrite(specialDataSection, sizeof(struct specialDataSection), 1, file);
}

uint64_t countNeededSpace(struct EntityRecord *entityRecord, uint8_t fieldsNumber) {
    uint16_t neededSpace = sizeof(struct recordId);
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        neededSpace += sizeof(uint64_t);
        neededSpace += entityRecord->fields[i].dataSize;
    }
    return neededSpace;
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
        fseek(file, offset + sizeof(struct headerSection) + headerSection.startEmptySpaceOffset, SEEK_SET);
        uint16_t writtenData = 0;
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
    } else {
        // дописать
        if (neededSpace >= BLOCK_DATA_SIZE) {
            printf("Your data too big");
            return;
        }
        fseek(file, offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
        struct specialDataSection specialDataSection;
        fread(&specialDataSection, sizeof(struct specialDataSection), 1, file);
        uint64_t newBlockOffset = allocateBlock(file, offset, headerSection.pageNumber + 1);
        specialDataSection.nextBlockOffset = newBlockOffset;
        fseek(file, offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
        fwrite(&specialDataSection, sizeof(struct specialDataSection), 1, file);
        insertRecord(file, entityRecord, tableOffsetBlock);
        return;
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
    free(buffer);
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

void deleteTableOffsetBlock(FILE* file, const char *tableName) {
    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
    fseek(file, sizeof(uint32_t), SEEK_SET);
    for (uint16_t i = 0; i < MAX_TABLES; i++) {
        fread(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
        if (strcmp(tableOffsetBlock->tableName, tableName) == 0) {
            tableOffsetBlock->isActive = false;
            fseek(file, -sizeof (struct tableOffsetBlock), SEEK_CUR);
            fwrite(tableOffsetBlock, sizeof (struct tableOffsetBlock), 1, file);
            break;
        }
    }
}

//struct tableOffsetBlock *findTableOffsetBlock(FILE *file, const char *tableName) {
//    unsigned char *buffer = malloc(sizeof(struct tableOffsetBlock) * MAX_TABLES);
//    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
//    fseek(file, sizeof(uint32_t), SEEK_SET);
//    fread(buffer, sizeof(struct tableOffsetBlock) * MAX_TABLES, 1, file);
//    for (uint16_t i = 0; i < MAX_TABLES; i++) {
//        uint64_t offset = sizeof(struct tableOffsetBlock) * i;
//        memcpy(tableOffsetBlock, buffer + offset, sizeof (struct tableOffsetBlock));
//        if (strcmp(tableOffsetBlock->tableName, tableName) == 0) {
//            //проблема
//            free(buffer);
//            return tableOffsetBlock;
//        }
//    }
//    free(buffer);
//    free(tableOffsetBlock);
//    return NULL;
//}

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

    iterator->predicate = predicate;
    iterator->predicateNumber = predicateNumber;
    iterator->blockOffset = tableOffsetBlock->firsTableBlockOffset;
    iterator->currentPositionInBlock = 0;
    iterator->fieldsNumber = tableOffsetBlock->fieldsNumber;
    iterator->nameTypeBlock = tableOffsetBlock->nameTypeBlock;
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

void optimiseSpaceInFile(FILE* file) {
    struct NameTypeBlock *nameTypeBlock = initNameTypeBlock("Offset", STRING);
    struct tableOffsetBlock *writtenTableMetaData = initTableOffsetBlock(file, "Meta", 1, nameTypeBlock);
    writeTableOffsetBlock(file, writtenTableMetaData);
}

void deleteTable(const char *tableName, FILE* file) {
    struct tableOffsetBlock* tableOffsetBlock = findTableOffsetBlock(file, tableName);
    uint64_t offset = tableOffsetBlock->firsTableBlockOffset;
    struct specialDataSection specialDataSection;
    struct EntityRecord entityRecord;
    struct FieldValue fieldValue;
    while (offset != 0) {
        fseek(file, offset + sizeof (struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
        fread(&specialDataSection, sizeof (struct specialDataSection), 1, file);
        char buffer[BUFFER_SIZE];
        snprintf(buffer, sizeof(char ) * BUFFER_SIZE, "%" PRIu64, offset);
        fieldValue.data = &buffer;
        fieldValue.dataSize = sizeof (char )* BUFFER_SIZE;
        entityRecord.fields = &fieldValue;
        insertRecordIntoTable(file, &entityRecord, "Meta");
        offset = specialDataSection.nextBlockOffset;
    }
    deleteTableOffsetBlock(file, tableName);
}
