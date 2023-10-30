#include <stdio.h>
#include <string.h>
#include "allocator.h"
#include "tableBlocks.h"
#include "dataBlocks.h"
#include "fileApi.h"
#include "iterator.h"

uint64_t allocateBlock(FILE *file, uint64_t previousOffset, uint64_t pageNumber) {
    struct iterator *iterator = readEntityRecordWithCondition(file, "Meta", NULL, 0);
    uint64_t emptySpaceOffset = 0;
    if (iterator != NULL) {
        struct EntityRecord *entityRecord = NULL;
        if (hasNext(iterator, file)) entityRecord = next(iterator, file);
        if (entityRecord != NULL) {
            char* dataChar = (char *) entityRecord->fields[0].data;
            char buffer[21];
            strncpy(buffer, dataChar, entityRecord->fields[0].dataSize);
            emptySpaceOffset = strtoull(buffer, NULL, 10);
            struct FieldValue fieldValue = {buffer, sizeof (char ) * 21};
            struct predicate predicate = {&fieldValue, "Offset", EQUALS};
            deleteRecordFromTable(file, "Meta", &predicate, 1);
        }
    }
    if (emptySpaceOffset == 0) {
        fseek(file, sizeof(struct defineTablesBlock) - sizeof(uint64_t), SEEK_SET);
        fread(&emptySpaceOffset, sizeof(uint64_t), 1, file);
        uint64_t resultOffset = emptySpaceOffset + (sizeof(struct headerSection) + BLOCK_DATA_SIZE + sizeof(struct specialDataSection));
        fseek(file, sizeof(struct defineTablesBlock) - sizeof(uint64_t), SEEK_SET);
        fwrite(&resultOffset, sizeof(uint64_t), 1, file);
        printf("\nEmptySpace offset: %lu\n", resultOffset);
    }
    uint64_t resultOffset = emptySpaceOffset;
    fseek(file, emptySpaceOffset, SEEK_SET);
    struct headerSection headerSection;
    headerSection.pageNumber = pageNumber;
    headerSection.recordsNumber = 0;
    headerSection.startEmptySpaceOffset = 0;
    headerSection.endEmptySpaceOffset = BLOCK_DATA_SIZE;
    struct specialDataSection specialDataSection;
    specialDataSection.previousBlockOffset = previousOffset;
    specialDataSection.nextBlockOffset = 0;
    fwrite(&headerSection, sizeof(struct headerSection), 1, file);
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fwrite(&specialDataSection, sizeof(struct specialDataSection), 1, file);
    return resultOffset;
}
