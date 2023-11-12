#ifndef LAB1_ITERATOR_H
#define LAB1_ITERATOR_H

#include "dataBlocks.h"
#include <stdbool.h>
#include <bits/types/FILE.h>
#include <stdio.h>
#include "../../include/data/data.h"
#include "../query/query.h"

struct iterator {
    struct predicate *predicate;
    uint8_t predicateNumber;
    uint16_t currentPositionInBlock;
    uint64_t blockOffset;
    uint8_t fieldsNumber;
    struct NameTypeBlock *nameTypeBlock;
};

bool hasNext(struct iterator *iterator, FILE *file);

struct EntityRecord *next(struct iterator *iterator, FILE *file);

struct EntityRecord *nextWithJoin(struct iterator *iterator1, const char *tableName,
                                  FILE *file, uint8_t fieldNumber, char *fieldName);

#endif //LAB1_ITERATOR_H
