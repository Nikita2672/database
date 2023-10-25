//
// Created by iwaa0303 on 07/10/23.
//

#include "comparator.h"
#include "../util/util.h"

int8_t compareInt(int32_t value1, int32_t value2) {
    if (value1 > value2) return 1;
    if (value2 > value1) return -1;
    return 0;
}

int8_t compareDouble(double value1, double value2) {
    if (value1 > value2) return 1;
    if (value2 > value1) return -1;
    return 0;
}

int8_t compareString(char * value1, char * value2, uint64_t length) {
    char* value11 = cutString(value1, 0, length);
    char* value22 = cutString(value2, 0, length);
    uint8_t result = 0;
    if (strcmp(value11, value22) < 0) result = -1;
    if (strcmp(value11, value22) > 0) result = 1;
    free(value11);
    free(value22);
    return result;
}

int8_t compareBoolean(bool value1, bool value2) {
    if (value1 > value2) return 1;
    if (value1 < value2) return -1;
    return 0;
}


int8_t compare(struct FieldValue fieldValue1, struct FieldValue fieldValue2, enum DataType dataType) {
    switch (dataType) {
        case INT:
            return compareInt(*(int32_t *)fieldValue1.data, *(int32_t *)fieldValue2.data);
        case DOUBLE:
            return compareDouble(*(double *)fieldValue1.data, *(double *)fieldValue2.data);
        case BOOL:
            return compareBoolean(*(bool *)fieldValue1.data, *(bool *)fieldValue2.data);
        default:
            return compareString((char *)fieldValue1.data, (char *)fieldValue2.data, fieldValue2.dataSize);
    }
}