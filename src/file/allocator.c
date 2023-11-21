#include <stdio.h>
#include <string.h>
#include "../../include/file/allocator.h"
#include "../../include/file/tableBlocks.h"
#include "../../include/file/dataBlocks.h"
#include "../../include/file/fileApi.h"

uint64_t allocateBlock(FILE *file, uint64_t previousOffset, uint64_t pageNumber) {
    Iterator *iterator = readEntityRecordWithCondition(file, "Meta", NULL, 0);
    uint64_t emptySpaceOffset = 0;
    if (iterator != NULL) {
        EntityRecord *entityRecord = NULL;
        if (hasNext(iterator, file)) entityRecord = next(iterator, file);
        if (entityRecord != NULL) {
            char *dataChar = (char *) entityRecord->fields[0].data;
            char buffer[21];
            strncpy(buffer, dataChar, entityRecord->fields[0].dataSize);
            emptySpaceOffset = strtoull(buffer, NULL, 10);
            FieldValue fieldValue = {buffer, sizeof(char) * 21};
            Predicate predicate = {&fieldValue, "Offset", EQUALS};
            deleteRecordFromTable(file, "Meta", &predicate, 1);
        }
    }
    if (emptySpaceOffset == 0) {
        fseek(file, sizeof(DefineTablesBlock) - sizeof(uint64_t), SEEK_SET);
        fread(&emptySpaceOffset, sizeof(uint64_t), 1, file);
        uint64_t resultOffset =
                emptySpaceOffset + (sizeof(HeaderSection) + BLOCK_DATA_SIZE + sizeof(SpecialDataSection));
        fseek(file, sizeof(DefineTablesBlock) - sizeof(uint64_t), SEEK_SET);
        fwrite(&resultOffset, sizeof(uint64_t), 1, file);
        printf("\nEmptySpace offset: %llu\n", resultOffset);
    }
    uint64_t resultOffset = emptySpaceOffset;
    fseek(file, emptySpaceOffset, SEEK_SET);
    HeaderSection headerSection;
    headerSection.pageNumber = pageNumber;
    headerSection.recordsNumber = 0;
    headerSection.startEmptySpaceOffset = 0;
    headerSection.endEmptySpaceOffset = BLOCK_DATA_SIZE;
    SpecialDataSection specialDataSection;
    specialDataSection.previousBlockOffset = previousOffset;
    specialDataSection.nextBlockOffset = 0;
    fwrite(&headerSection, sizeof(HeaderSection), 1, file);
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fwrite(&specialDataSection, sizeof(SpecialDataSection), 1, file);
    return resultOffset;
}
