#include "tableBlocks.h"
#include <stdbool.h>
#include <string.h>


struct NameTypeBlock* initNameTypeBlock(const char fieldName[MAX_LENGTH_FIELD_NAME], enum DataType dataType) {
    struct NameTypeBlock* nameTypeBlock = malloc(sizeof (struct NameTypeBlock));
    strcpy(nameTypeBlock->fieldName, fieldName);
    nameTypeBlock->dataType = dataType;
    return nameTypeBlock;
}

struct tableOffsetBlock* initTableOffsetBlock(const char name[MAX_LENGTH_TABLE_NAME],
        uint64_t firstTableOffset, uint64_t lastTableOffset, uint8_t fieldsNumber,
        const struct NameTypeBlock nameTypeBlock[MAX_FIELDS]) {
    struct tableOffsetBlock* tableOffsetBlock = malloc(sizeof (struct tableOffsetBlock));
    tableOffsetBlock->isActive = true;
    strncpy(tableOffsetBlock->tableName, name, MAX_LENGTH_TABLE_NAME);
    tableOffsetBlock->firsTableBlockOffset = firstTableOffset;
    tableOffsetBlock->lastTableBLockOffset = lastTableOffset;
    tableOffsetBlock->fieldsNumber = fieldsNumber;
    for (uint8_t i = 0; i < fieldsNumber; i++) {
        tableOffsetBlock->nameTypeBlock[i] = nameTypeBlock[i];
    }
    return tableOffsetBlock;
}







