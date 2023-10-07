#include <string.h>
#include "tableBlocks.h"
#include "stdio.h"
#include "stdlib.h"
#include "dataBlocks.h"
#include "allocator.h"
#include "../query/query.h"

void writeEmptyTablesBlock(const char *name) {
    FILE *file = fopen(name, "wb");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    struct defineTablesBlock *data = malloc(sizeof(struct defineTablesBlock));
    if (data == NULL) {
        printf("error allocation memory");
        return;
    }
    data->countTables = 0;
    data->emptySpaceOffset = sizeof(struct defineTablesBlock);
    fwrite(data, sizeof(struct defineTablesBlock), 1, file);
    free(data);
    fclose(file);
}

struct defineTablesBlock *readTablesBlock(const char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }
    struct defineTablesBlock *defineTablesBlock = malloc(sizeof(struct defineTablesBlock));
    fread(defineTablesBlock, sizeof(struct defineTablesBlock), 1, file);
    fclose(file);
    return defineTablesBlock;
}

uint32_t readTablesCount(const char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return 10001;
    }
    uint32_t tablesCount;
    fread(&tablesCount, sizeof(uint32_t), 1, file);
    fclose(file);
    return tablesCount;
}

void writeTableCount(const char *name, uint32_t tablesCount) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fwrite(&tablesCount, sizeof(uint32_t), 1, file);
    fclose(file);
}

uint64_t readEmptySpaceOffset(const char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return 0;
    }
    uint64_t tableCount;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(uint64_t)), SEEK_SET);
    fread(&tableCount, sizeof(uint64_t), 1, file);
    fclose(file);
    return tableCount;
}

void writeEmptySpaceOffset(const char *name, uint64_t offset) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    uint64_t emptySpaceOffset = offset;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(uint64_t)), SEEK_SET);
    fwrite(&emptySpaceOffset, sizeof(uint64_t), 1, file);
    fclose(file);
}

struct tableOffsetBlock *readTableOffsetBlock(const char *name, uint16_t tablePosition) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }
    if (tablePosition > 1000) {
        printf("Your table number is too big");
        return NULL;
    }
    struct tableOffsetBlock *tableOffsetBlock = malloc(sizeof(struct tableOffsetBlock));
    fseek(file, (sizeof(uint32_t) + (sizeof (struct tableOffsetBlock) * tablePosition)), SEEK_SET);
    fread(tableOffsetBlock, sizeof(struct tableOffsetBlock),1, file);
    fclose(file);
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

void writeTableOffsetBlock(const char *name, struct tableOffsetBlock *tableOffsetBlock) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    uint64_t offset = findOffsetForTableOffsetBlock(file);
    fseek(file, offset, SEEK_SET);
    fwrite(tableOffsetBlock, sizeof(struct tableOffsetBlock), 1, file);
    fclose(file);
}

void writeDataBlock(uint64_t offset, const char *name, struct headerSection* headerSection, struct specialDataSection* specialDataSection) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fseek(file, offset, SEEK_SET);
    fwrite(headerSection, sizeof(struct headerSection), 1, file);
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fwrite(specialDataSection, sizeof(struct specialDataSection), 1, file);
    fclose(file);
}

uint64_t countNeededSpace(struct EntityRecord* entityRecord, uint8_t fieldsNumber) {
    uint16_t neededSpace = sizeof (struct recordId);
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        neededSpace += sizeof (enum DataType);
        neededSpace += sizeof (uint64_t);
        neededSpace += (strlen(entityRecord->fields[i].fieldName) + 1) * sizeof (char );
        neededSpace += sizeof (uint64_t);
        neededSpace += entityRecord->fields[i].dataSize;
    }
    return neededSpace;
}

void insertRecord(const char* name, struct EntityRecord* entityRecord, uint16_t fieldsNumber, struct headerSection headerSection, uint64_t offset) {
    uint16_t space = abs(headerSection.startEmptySpaceOffset - headerSection.endEmptySpaceOffset);
    uint16_t beforeWriteOffset = headerSection.startEmptySpaceOffset;
    uint64_t neededSpace = countNeededSpace(entityRecord, fieldsNumber);
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    if (neededSpace <= space) {
        fseek(file, offset + sizeof(struct headerSection) + headerSection.startEmptySpaceOffset, SEEK_SET);
        for (uint16_t i = 0; i < fieldsNumber; i++) {
            struct FieldValue *field = &entityRecord->fields[i];
            headerSection.startEmptySpaceOffset +=
                    fwrite(&field->type, sizeof(enum DataType), 1, file) * sizeof(enum DataType);
            uint64_t fieldNameLength = strlen(field->fieldName) + 1;
            headerSection.startEmptySpaceOffset +=
                    fwrite(&fieldNameLength, sizeof(uint64_t), 1, file) * sizeof(uint64_t);
            headerSection.startEmptySpaceOffset +=
                    fwrite(field->fieldName, sizeof(char), fieldNameLength, file) * sizeof(char);
            headerSection.startEmptySpaceOffset +=
                    fwrite(&field->dataSize, sizeof(uint64_t), 1, file) * sizeof(uint64_t);
            headerSection.startEmptySpaceOffset += fwrite(field->data, 1, field->dataSize, file);
        }
        uint16_t length = abs(beforeWriteOffset - headerSection.startEmptySpaceOffset);
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
        headerSection.recordsNumber++;
        fwrite(&headerSection, sizeof(struct headerSection), 1, file);
        fclose(file);
    } else {
        if (neededSpace >= BLOCK_DATA_SIZE) {
            printf("Your data too big");
            return;
        }
        fseek(file, offset + sizeof (struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
        struct specialDataSection specialDataSection;
        fread(&specialDataSection, sizeof(struct specialDataSection), 1, file);
        uint64_t newBlockOffset = allocateBlock(file, offset, headerSection.pageNumber + 1);
        specialDataSection.nextBlockOffset = newBlockOffset;
        fseek(file, offset + sizeof (struct headerSection) + BLOCK_DATA_SIZE, SEEK_SET);
        fwrite(&specialDataSection, sizeof (struct specialDataSection), 1, file);
        fclose(file);
        insertRecord(name, entityRecord, fieldsNumber, headerSection, newBlockOffset);
        return;
    }
}

struct EntityRecord* readRecord(const char* name, uint16_t idPosition, uint64_t offset, uint16_t fieldsNumber) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }
    idPosition++;
    struct headerSection* headerSection = malloc(sizeof (struct headerSection));
    fseek(file, offset, SEEK_SET);
    fread(headerSection, sizeof (struct headerSection), 1, file);
    uint64_t recordIdOffset = offset + sizeof (struct headerSection) + BLOCK_DATA_SIZE - (sizeof (struct recordId) * idPosition);
    fseek(file, recordIdOffset, SEEK_SET);
    struct recordId* recordId = malloc(sizeof (struct recordId));
    fread(recordId, sizeof (struct recordId), 1, file);
    fseek(file, offset + sizeof (struct headerSection) + recordId->offset, SEEK_SET);
    struct EntityRecord *entityRecord = malloc(sizeof (struct EntityRecord));
    struct FieldValue* fields = malloc(sizeof (struct FieldValue) * fieldsNumber);
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        struct FieldValue *field = malloc(sizeof (struct FieldValue));
        fread(&field->type, sizeof (enum DataType), 1, file);
        uint64_t fieldNameLength;
        fread(&fieldNameLength, sizeof (uint64_t), 1, file);
        field->fieldName = malloc(fieldNameLength);
        fread(field->fieldName, sizeof (char ), fieldNameLength, file);
        fread(&field->dataSize, sizeof (uint64_t), 1, file);
        field->data = malloc(field->dataSize);
        fread(field->data, field->dataSize, 1, file);
        fields[i] = *field;
    }
    entityRecord->fields = fields;
    free(headerSection);
    free(recordId);
    fclose(file);
    return entityRecord;
}

void deleteRecord(const char* fileName, uint32_t position, uint64_t offset) {
    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fseek(file, offset, SEEK_SET);
    struct headerSection headerSection;
    fread(&headerSection, sizeof (struct headerSection), 1, file);
    if (headerSection.recordsNumber < position) return;
    position++;
    fseek(file, offset + sizeof (struct headerSection) + BLOCK_DATA_SIZE - sizeof (struct recordId) * position, SEEK_SET);
    struct recordId recordId = {0, 0};
    fwrite(&recordId, sizeof (struct recordId), 1, file);
}

void insertRecordIntoTable(const char* fileName, struct EntityRecord* entityRecord, const char * tableName) {
    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        printf("Error openinкак проверить разрядность системы на линуксg file\n");
        return;
    }
    fseek(file, sizeof (uint32_t), SEEK_SET);
    struct tableOffsetBlock tableOffsetBlock;
    for (uint16_t i = 0; i < MAX_TABLES; i++) {
        fread(&tableOffsetBlock, sizeof (struct tableOffsetBlock), 1, file);
        if (tableOffsetBlock.tableName == tableName) {
            struct headerSection headerSection;
            fseek(file, tableOffsetBlock.firsTableBlockOffset, SEEK_CUR);
            fread(&headerSection, sizeof (struct headerSection), 1, file);
            fclose(file);
            return insertRecord(fileName, entityRecord, tableOffsetBlock.fieldsNumber, headerSection, tableOffsetBlock.firsTableBlockOffset);
        }
    }
    fclose(file);
    printf("There is no %s table", tableName);
}

struct EntityRecord* readEntityRecordWithCondition(const char* fileName, const char* tableName, struct predicate* predicate,
        uint8_t predicateNumber) {


}
