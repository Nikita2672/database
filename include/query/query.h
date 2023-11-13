#include <stdbool.h>
#include "stdlib.h"
#include "../../include/data/data.h"
#include "../../include/file/tableBlocks.h"

#ifndef LAB1_QUERY_H
#define LAB1_QUERY_H

enum Compare {
    MORE,
    LESS,
    EQUALS,
    MORE_OR_EQUALS,
    LESS_OR_EQUALS
};

typedef struct {
    FieldValue *comparableValue;
    char *fieldName;
    enum Compare comparator;
} Predicate;

typedef struct {
    Predicate *predicates;
    uint32_t predicatesNumber;
    char *tableName;
} Query;

void freePredicate(Predicate *predicate);

void freeQuery(Query *query);

bool
checkPredicate(Predicate *predicate, EntityRecord *entityRecord, uint16_t fieldsNumber, NameTypeBlock *nameTypeBlock);

#endif //LAB1_QUERY_H
