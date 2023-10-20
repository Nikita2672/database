//
// Created by iwaa0303 on 20/10/23.
//

#include "util.h"

void printEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber, struct NameTypeBlock* nameTypeBlock) {
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
                printf("%s; ", (char *) entityRecord->fields[i].data);
                break;
        }
    }
    printf("\n");
}