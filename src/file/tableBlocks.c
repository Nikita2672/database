#include "../../include/file/tableBlocks.h"
#include "../../include/file/allocator.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

NameTypeBlock *initNameTypeBlock(const char fieldName[MAX_LENGTH_FIELD_NAME], enum DataType dataType) {
    NameTypeBlock *nameTypeBlock = malloc(sizeof(NameTypeBlock));
    strcpy(nameTypeBlock->fieldName, fieldName);
    nameTypeBlock->dataType = dataType;
    return nameTypeBlock;
}

TableOffsetBlock *initTableOffsetBlock(FILE *file, const char name[MAX_LENGTH_TABLE_NAME],
                                       uint8_t fieldsNumber, NameTypeBlock nameTypeBlock[MAX_FIELDS]) {
    TableOffsetBlock *tableOffsetBlock = malloc(sizeof(TableOffsetBlock));
    tableOffsetBlock->isActive = true;
    tableOffsetBlock->firsTableBlockOffset = allocateBlock(file, 0, 0);
    tableOffsetBlock->lastTableBLockOffset = tableOffsetBlock->firsTableBlockOffset;
    tableOffsetBlock->fieldsNumber = fieldsNumber;
    strcpy(tableOffsetBlock->tableName, name);
    for (uint8_t i = 0; i < fieldsNumber; i++) {
        tableOffsetBlock->nameTypeBlock[i] = nameTypeBlock[i];
    }
    return tableOffsetBlock;
}







