#ifndef LAB1_UTIL_H
#define LAB1_UTIL_H

#include "../../include/data/data.h"
#include "../../include/file/tableBlocks.h"
#include "stdio.h"

void printEntityRecord(EntityRecord *entityRecord, uint16_t fieldsNumber, NameTypeBlock *nameTypeBlock);

char *cutString(char *string, uint64_t start, uint64_t end);

uint64_t getFileSize(FILE *file);

void freeEntityRecord(EntityRecord *entityRecord, uint16_t fieldsNumber);

void printMetaTableRecords(FILE * file);

#endif //LAB1_UTIL_H
