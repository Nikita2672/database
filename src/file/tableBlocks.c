#include "../../include/file/tableBlocks.h"
#include "../../include/file/allocator.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

NameTypeBlock *initNameTypeBlock(const char *fieldName, enum DataType dataType) {
    NameTypeBlock *nameTypeBlock = malloc(sizeof(NameTypeBlock));
    strncpy(nameTypeBlock->fieldName, fieldName, strlen(fieldName) + 1);
    nameTypeBlock->dataType = dataType;
    return nameTypeBlock;
}

TableOffsetBlock *initTableOffsetBlock(FILE *file, const char *name,
                                       uint8_t fieldsNumber, NameTypeBlock *nameTypeBlock) {
    TableOffsetBlock *tableOffsetBlock = malloc(sizeof(TableOffsetBlock));
    tableOffsetBlock->isActive = true;
    tableOffsetBlock->firsTableBlockOffset = allocateBlock(file, 0, 0);
    tableOffsetBlock->lastTableBLockOffset = tableOffsetBlock->firsTableBlockOffset;
    tableOffsetBlock->fieldsNumber = fieldsNumber;
    strncpy(tableOffsetBlock->tableName, name, strlen(name) + 1);
    for (uint8_t i = 0; i < fieldsNumber; i++) {
        tableOffsetBlock->nameTypeBlock[i] = nameTypeBlock[i];
    }
    return tableOffsetBlock;
}







