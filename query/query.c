#include <stdlib.h>
#include "../errors.h"
#include "query.h"

struct query* createQuery(enum operations operation, struct predicate* predicate,
        u_int32_t predicateNumber, char *tableName) {
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
            for (u_int32_t i = 0; i < query->predicatesNumber; i++) {
                freePredicate(&(query->predicates[i]));
            }
            free(query->predicates);
        }
        free(query->tableName);
        free(query);
    }
}
