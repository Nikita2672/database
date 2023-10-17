#ifndef LAB1_ITERATOR_H
#define LAB1_ITERATOR_H

#include "dataBlocks.h"
#include <stdbool.h>
#include <bits/types/FILE.h>
#include <stdio.h>
#include "../data/data.h"
#include "../query/query.h"

bool hasNextBlock(FILE * file, uint64_t offset);
struct EntityRecord* next(struct iterator* iterator, char * fileName);

struct iterator {
    struct predicate* predicate;
    uint8_t predicateNumber;
    uint16_t currentPositionInBlock;
    uint64_t blockOffset;
    uint8_t fieldsNumber;
};

#endif //LAB1_ITERATOR_H
