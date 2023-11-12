#ifndef LAB1_UTIL_H
#define LAB1_UTIL_H

#include "../../public/data/data.h"
#include "../../public/file/tableBlocks.h"
#include "stdio.h"

void printEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber, struct NameTypeBlock *nameTypeBlock);

char *cutString(char *string, uint64_t start, uint64_t end);

uint64_t getFileSize(FILE *file);

void freeEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber);

#endif //LAB1_UTIL_H
