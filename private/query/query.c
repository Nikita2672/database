#include <stdlib.h>
#include <stdio.h>
#include "../../public/query/query.h"
#include "../../public/data/comparator.h"
#include "../../public/file/tableBlocks.h"

bool checkPredicate(struct predicate *predicate, struct EntityRecord *entityRecord, uint16_t fieldsNumber,
                    struct NameTypeBlock *nameTypeBlock) {
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

void freePredicate(struct predicate *predicate) {
    if (predicate != NULL) {
        free(predicate->fieldName);
        free(predicate->comparableValue->data);
        free(predicate);
    }
}

void freeQuery(struct query *query) {
    if (query != NULL) {
        if (query->predicates != NULL) {
            for (uint32_t i = 0; i < query->predicatesNumber; i++) {
                freePredicate(&(query->predicates[i]));
            }
            free(query->predicates);
        }
        free(query->tableName);
        free(query);
    }
}
