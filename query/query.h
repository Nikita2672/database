#include <stdbool.h>
#include "stdlib.h"
#include "../data/data.h"
#include "../file/tableBlocks.h"

#ifndef LAB1_QUERY_H
#define LAB1_QUERY_H
enum operations {
    SELECT,
    INSERT,
    UPDATE,
    DELETE,
    CREATE,
    DROP
};

enum compare {
    MORE,
    LESS,
    EQUALS,
    MORE_OR_EQUALS,
    LESS_OR_EQUALS
};

struct predicate {
    struct FieldValue* comparableValue;
    char *fieldName;
    enum compare comparator;
};

struct query {
    enum operations operation;
    struct predicate* predicates;
    uint32_t predicatesNumber;
    char *tableName;
};

struct query* createQuery(enum operations operation, struct predicate* predicate,
                          uint32_t predicateNumber, char *tableName);
struct predicate* createPredicate(struct FieldValue* comparableValue, char *fieldName, enum compare comparator);
void freePredicate(struct predicate* predicate);
void freeQuery(struct query* query);
bool checkPredicate(struct predicate* predicate, struct EntityRecord* entityRecord, uint16_t fieldsNumber,
                    struct NameTypeBlock* nameTypeBlock);
#endif //LAB1_QUERY_H
