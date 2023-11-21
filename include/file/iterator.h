#ifndef LAB1_ITERATOR_H
#define LAB1_ITERATOR_H

#include "dataBlocks.h"
#include <stdbool.h>
#include <stdio.h>
#include "../../include/data/data.h"
#include "../query/query.h"

typedef struct {
    Predicate *predicate;
    uint8_t predicateNumber;
    uint16_t currentPositionInBlock;
    uint64_t blockOffset;
    uint8_t fieldsNumber;
    NameTypeBlock *nameTypeBlock;
} Iterator;

bool hasNext(Iterator *iterator, FILE *file);

void freeIterator(Iterator *iterator);

EntityRecord *next(Iterator *iterator, FILE *file);

EntityRecord *nextWithJoin(Iterator *iterator1, const char *tableName, FILE *file, uint8_t fieldNumber, char *fieldName);

#endif //LAB1_ITERATOR_H
