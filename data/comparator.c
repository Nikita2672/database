//
// Created by iwaa0303 on 07/10/23.
//

#include "comparator.h"
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

int8_t compareString(const char * value1, const char * value2) {
    if (strcmp(value1, value2) < 0) return -1;
    if (strcmp(value1, value2) > 0) return 1;
    return 0;
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
            return compareString((char *)fieldValue1.data, (char *)fieldValue2.data);
    }
}