//
// Created by iwaa0303 on 20/10/23.
//

#ifndef LAB1_UTIL_H
#define LAB1_UTIL_H
#include "../data/data.h"
#include "../file/tableBlocks.h"
#include "stdio.h"
void printEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber, struct NameTypeBlock* nameTypeBlock);
char* cutString(char* string, uint64_t start, uint64_t end);
uint64_t getFileSize(FILE * file);
#endif //LAB1_UTIL_H
