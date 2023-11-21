#include <stdio.h>
#include "../../include/query/query.h"
#include "../../include/data/comparator.h"
#include "../../include/file/tableBlocks.h"

bool checkPredicate(Predicate *predicate, EntityRecord *entityRecord, uint16_t fieldsNumber,
                    NameTypeBlock *nameTypeBlock) {
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        char *fieldName = nameTypeBlock[i].fieldName;
        if (strcmp(fieldName, predicate->fieldName) == 0) {
            int8_t result = compare(entityRecord->fields[i], *predicate->comparableValue, nameTypeBlock[i].dataType);
            switch (predicate->comparator) {
                case EQUALS:
                    return result == 0;
                case MORE:
                    return result == 1;
                case LESS:
                    return result == -1;
                case MORE_OR_EQUALS:
                    return result >= 0;
                case LESS_OR_EQUALS:
                    return result <= 0;
                default:
                    return false;
            }
        }
    }
    printf("there is no %s field in table\n", predicate->fieldName);
    return false;
}
