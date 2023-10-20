#ifndef LAB1_COMPARATOR_H
#define LAB1_COMPARATOR_H

#include <stdint-gcc.h>
#include <string.h>
#include <stdbool.h>
#include "data.h"

int8_t compare(struct FieldValue fieldValue1, struct FieldValue fieldValue2, enum DataType dataType);
#endif //LAB1_COMPARATOR_H
