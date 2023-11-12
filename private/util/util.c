#include <stdint-gcc.h>
#include "../../public/util/util.h"
#include "string.h"

void printEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber, struct NameTypeBlock *nameTypeBlock) {
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        printf("%s: ", nameTypeBlock[i].fieldName);
        switch (nameTypeBlock[i].dataType) {
            case INT:
                printf("%d; ", *(int32_t *) entityRecord->fields[i].data);
                break;
            case DOUBLE:
                printf("%f; ", *(double *) entityRecord->fields[i].data);
                break;
            case BOOL:
                if (*(bool *) entityRecord->fields[i].data) {
                    printf("true; ");
                } else {
                    printf("false; ");
                }
                break;
            default:
                printf("%s; ", cutString((char *) entityRecord->fields[i].data, 0, entityRecord->fields[i].dataSize));
                break;
        }
    }
    printf("\n");
}

char *cutString(char *string, uint64_t start, uint64_t end) {
    if (string == NULL || start >= end) {
        return NULL;
    }
    size_t length = end - start;
    char *newString = (char *) malloc(length + 1);

    if (newString == NULL) {
        return NULL;
    }
    strncpy(newString, string + start, length);
    newString[length] = '\0';
    return newString;
}

uint64_t getFileSize(FILE *file) {
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

void freeEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber) {
    for (uint16_t i = 0; i < fieldsNumber; i++) {
        free(entityRecord->fields[i].data);
    }
    free(entityRecord->fields);
    free(entityRecord->linkNext);
    free(entityRecord);
}
