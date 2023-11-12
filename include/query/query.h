#include <stdbool.h>
#include "stdlib.h"
#include "../../include/data/data.h"
#include "../../include/file/tableBlocks.h"

#ifndef LAB1_QUERY_H
#define LAB1_QUERY_H

enum compare {
    MORE,
    LESS,
    EQUALS,
    MORE_OR_EQUALS,
    LESS_OR_EQUALS
};

struct predicate {
    struct FieldValue *comparableValue;
    char *fieldName;
    enum compare comparator;
};

struct query {
    struct predicate *predicates;
    uint32_t predicatesNumber;
    char *tableName;
};

void freePredicate(struct predicate *predicate);

bool checkPredicate(struct predicate *predicate, struct EntityRecord *entityRecord, uint16_t fieldsNumber,
                    struct NameTypeBlock *nameTypeBlock);

#endif //LAB1_QUERY_H
