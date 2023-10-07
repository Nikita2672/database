#include <stdio.h>
#include "allocator.h"
#include "tableBlocks.h"
#include "dataBlocks.h"

uint64_t allocateBlock(FILE * file, uint64_t previousOffset, uint64_t pageNumber) {
    fseek(file, sizeof (struct defineTablesBlock) - sizeof (uint64_t), SEEK_SET);
    uint64_t emptySpaceOffset;
    fread(&emptySpaceOffset, sizeof (uint64_t), 1, file);
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
    fwrite(&headerSection, sizeof (struct headerSection), 1, file);
    fseek(file, BLOCK_DATA_SIZE, SEEK_CUR);
    fwrite(&specialDataSection, sizeof (struct specialDataSection), 1, file);
    fseek(file, sizeof (struct defineTablesBlock) - sizeof (uint64_t), SEEK_SET);
    emptySpaceOffset += (sizeof (struct headerSection) + BLOCK_DATA_SIZE + sizeof (struct specialDataSection));
    fwrite(&emptySpaceOffset, sizeof (uint64_t), 1, file);
    return resultOffset;
}
