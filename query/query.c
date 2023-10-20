#include <stdlib.h>
#include <stdio.h>
#include "../errors.h"
#include "query.h"
#include "../data/comparator.h"

struct query* createQuery(enum operations operation, struct predicate* predicate,
        uint32_t predicateNumber, char *tableName) {
    struct query* newQuery = (struct query*) malloc(sizeof (struct query));
    if (newQuery == NULL) {
        errorAllocation("query");
        return NULL;
    }
    newQuery->tableName = tableName;
    newQuery->operation = operation;
    newQuery->predicates = predicate;
    newQuery->predicatesNumber = predicateNumber;
    return newQuery;
}

struct predicate* createPredicate(struct FieldValue* comparableValue, char *fieldName, enum compare comparator) {
    struct predicate* newPredicate = (struct predicate*)malloc(sizeof(struct predicate));
    if (newPredicate == NULL) {
        errorAllocation("predicate");
        return NULL;
    }
    newPredicate->comparableValue = comparableValue;
    newPredicate->fieldName = fieldName;
    newPredicate->comparator = comparator;
    return newPredicate;
}

bool checkPredicate(struct predicate* predicate, struct EntityRecord* entityRecord, uint16_t fieldsNumber) {
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        if (strcmp(entityRecord->fields[i].fieldName, predicate->fieldName) == 0) {
            int8_t result = compare(entityRecord->fields[i], *predicate->comparableValue);
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
    printf("there is no %s field in table", predicate->fieldName);
    return false;
}

void freePredicate(struct predicate* predicate) {
    if (predicate != NULL) {
        free(predicate->fieldName);
        free(predicate->comparableValue->data);
        free(predicate);
    }
}

void freeQuery(struct query* query) {
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
