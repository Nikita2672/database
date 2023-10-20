#include <string.h>
#include "tableBlocks.h"
#include "stdio.h"
#include "stdlib.h"
#include "dataBlocks.h"
#include "allocator.h"
#include "../query/query.h"
#include "fileApi.h"
#include "iterator.h"

void writeEmptyTablesBlock(FILE *file) {
    struct defineTablesBlock *data = malloc(sizeof(struct defineTablesBlock));
    if (data == NULL) {
        printf("error allocation memory");
        return;
    }
    data->countTables = 0;
    data->emptySpaceOffset = sizeof(struct defineTablesBlock);
    fwrite(data, sizeof(struct defineTablesBlock), 1, file);
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
    for (uint64_t i = 0; i < MAX_TABLES; i++) {
        uint64_t offset = (sizeof(uint32_t) + (sizeof(struct tableOffsetBlock) * i));
        fseek(file, offset, SEEK_SET);
        struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
        fread(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
        if (!(tableOffsetBlock->isActive)) {
            free(tableOffsetBlock);
            return offset;
        }
        free(tableOffsetBlock);
    }
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
    fread(headerSection, sizeof(struct headerSection), 1, file);
    uint64_t recordIdOffset =
            offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE - (sizeof(struct recordId) * idPosition);
    fseek(file, recordIdOffset, SEEK_SET);
    struct recordId *recordId = malloc(sizeof(struct recordId));
    fread(recordId, sizeof(struct recordId), 1, file);
    fseek(file, offset + sizeof(struct headerSection) + recordId->offset, SEEK_SET);
    struct EntityRecord *entityRecord = malloc(sizeof(struct EntityRecord));
    struct FieldValue *fields = malloc(sizeof(struct FieldValue) * fieldsNumber);
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        struct FieldValue *field = malloc(sizeof(struct FieldValue));
        fread(&field->dataSize, sizeof(uint64_t), 1, file);
        field->data = malloc(field->dataSize);
        fread(field->data, field->dataSize, 1, file);
        fields[i] = *field;
    }
    entityRecord->fields = fields;
    free(headerSection);
    free(recordId);
    return entityRecord;
}

// rewrite
void deleteRecord(FILE* file, uint32_t position, uint64_t offset) {
    fseek(file, offset, SEEK_SET);
    struct headerSection headerSection;
    fread(&headerSection, sizeof(struct headerSection), 1, file);
    if (headerSection.recordsNumber < position) return;
    position++;
    fseek(file, offset + sizeof(struct headerSection) + BLOCK_DATA_SIZE - sizeof(struct recordId) * position, SEEK_SET);
    struct recordId recordId = {0, 0};
    fwrite(&recordId, sizeof(struct recordId), 1, file);
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

void insertRecordIntoTable(FILE* file, struct EntityRecord *entityRecord, const char *tableName) {
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

struct iterator *readEntityRecordWithCondition(FILE* file, const char *tableName, struct predicate *predicate,
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
