#include "tableBlocks.h"
#include "stdio.h"
#include "dataBlocks.h"

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

u_int32_t readTablesCount(const char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return 10001;
    }
    u_int32_t tablesCount;
    fread(&tablesCount, sizeof(u_int32_t), 1, file);
    fclose(file);
    return tablesCount;
}

void writeTableCount(const char *name, u_int32_t tablesCount) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    fwrite(&tablesCount, sizeof(u_int32_t), 1, file);
    fclose(file);
}

u_int64_t readEmptySpaceOffset(const char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return 0;
    }
    u_int64_t tableCount;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(u_int64_t)), SEEK_SET);
    fread(&tableCount, sizeof(u_int64_t), 1, file);
    fclose(file);
    return tableCount;
}

void writeEmptySpaceOffset(const char *name, u_int64_t offset) {
    FILE *file = fopen(name, "rb+");
    if (file == NULL) {
        printf("Error opening file\n");
        return;
    }
    u_int64_t emptySpaceOffset = offset;
    fseek(file, (sizeof(struct defineTablesBlock) - sizeof(u_int64_t)), SEEK_SET);
    fwrite(&emptySpaceOffset, sizeof(u_int64_t), 1, file);
    fclose(file);
}

struct tableOffsetBlock *readTableOffsetBlock(const char *name, u_int16_t tablePosition) {
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

u_int64_t findOffsetForTableOffsetBlock(FILE *file) {
    for (u_int64_t i = 0; i < MAX_TABLES; i++) {
        u_int64_t offset = (sizeof(u_int32_t) + (sizeof(struct tableOffsetBlock) * i));
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
    u_int64_t offset = findOffsetForTableOffsetBlock(file);
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

bool isFit(struct EntityRecord* entityRecord, uint8_t fieldsNumber, uint16_t space) {
    uint16_t neededSpace = 0;
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        neededSpace += entityRecord->fields[i].dataSize;
        if (neededSpace > space) return false;
    }
    return true;
}

void insertRecord(const char* name, struct EntityRecord* entityRecord, uint8_t fieldsNumber, struct headerSection headerSection, uint64_t offset) {
    uint16_t space = abs(headerSection.startEmptySpaceOffset - headerSection.endEmptySpaceOffset);
    if (isFit(entityRecord, fieldsNumber, space)) {
        FILE *file = fopen(name, "rb+");
        if (file == NULL) {
            printf("Error opening file\n");
            return;
        }
        fseek(file, offset, SEEK_SET);


        fwrite(&fieldsNumber, sizeof ());


        fwrite(entityRecord, sizeof (struct EntityRecord), 1, file);





        fclose(file);
    }
}

