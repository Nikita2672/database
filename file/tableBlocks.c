#include "tableBlocks.h"
#include "allocator.h"
#include <stdbool.h>
#include <string.h>
#include <bits/types/FILE.h>
#include <stdio.h>


struct NameTypeBlock* initNameTypeBlock(const char fieldName[MAX_LENGTH_FIELD_NAME], enum DataType dataType) {
    struct NameTypeBlock* nameTypeBlock = malloc(sizeof (struct NameTypeBlock));
    strcpy(nameTypeBlock->fieldName, fieldName);
    nameTypeBlock->dataType = dataType;
    return nameTypeBlock;
}

struct tableOffsetBlock* initTableOffsetBlock(const char* fileName, const char name[MAX_LENGTH_TABLE_NAME],
        uint8_t fieldsNumber, const struct NameTypeBlock nameTypeBlock[MAX_FIELDS]) {
    struct tableOffsetBlock* tableOffsetBlock = malloc(sizeof (struct tableOffsetBlock));
    tableOffsetBlock->isActive = true;
    strncpy(tableOffsetBlock->tableName, name, MAX_LENGTH_TABLE_NAME);
    FILE* file = fopen(fileName, "rb+");
    tableOffsetBlock->firsTableBlockOffset = allocateBlock(file, 0, 0);
    fclose(file);
    tableOffsetBlock->lastTableBLockOffset = tableOffsetBlock->firsTableBlockOffset;
    tableOffsetBlock->fieldsNumber = fieldsNumber;
    for (uint8_t i = 0; i < fieldsNumber; i++) {
        tableOffsetBlock->nameTypeBlock[i] = nameTypeBlock[i];
    }
    return tableOffsetBlock;
}







