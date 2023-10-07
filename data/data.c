#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "../errors.h"


#define DEFAULT_CAPACITY 200


struct FieldValue* initFieldValue(enum DataType dataType, void* dataP, char *fieldName) {
    struct FieldValue* pFieldValue = malloc(sizeof (struct FieldValue));
    if (pFieldValue == NULL) errorAllocation("pFieldValue");
    pFieldValue->data = dataP;
    pFieldValue->type = dataType;
    pFieldValue->fieldName = fieldName;
    pFieldValue->dataSize = 0;
    return pFieldValue;
}

void freeFieldValue(struct FieldValue* fieldValue) {
    if (fieldValue != NULL) {
        if (fieldValue->fieldName != NULL) free(fieldValue->fieldName);
        if (fieldValue->data != NULL) free(fieldValue->data);
        free(fieldValue);
    }
}

struct EntityRecord* initEntityRecord(struct Table* table) {
    struct EntityRecord* pEntityRecord = malloc(sizeof (struct EntityRecord));
    if (pEntityRecord == NULL) errorAllocation("pEntityRecord");
    pEntityRecord->fields = malloc(table->fieldsNumber * sizeof (struct FieldValue));
    if (pEntityRecord->fields == NULL) errorAllocation("pEntityRecord->fields while init");
    return pEntityRecord;
}

void freeEntityRecord(struct EntityRecord* entityRecord, uint32_t fieldsNumber) {
    if (entityRecord != NULL) {
        if (entityRecord->fields != NULL) {
            for (uint32_t i = 0; i < fieldsNumber; i++) {
                freeFieldValue(&(entityRecord->fields[i]));
            }
            free(entityRecord->fields);
        }
        free(entityRecord);
    }
}

struct NameType* initNameType(const char *fieldName, enum DataType dataType) {
    struct NameType* nameType = malloc(sizeof (struct NameType));
    nameType->dataType = dataType;
    nameType->fieldName = fieldName;
    return nameType;
}

void freeNameType(struct NameType* nameType) {
    if (nameType != NULL) {
        if (nameType->fieldName != NULL) {
            free(nameType->fieldName);
        }
        free(nameType);
    }
}

struct Table* initTable(uint32_t fieldsNumber, struct NameType* nameTypes) {
    struct Table* table = malloc(sizeof (struct Table));
    table->capacity = DEFAULT_CAPACITY;
    table->recordsNumber = 0;
    table->fieldsNumber = fieldsNumber;
    table->nameTypes = nameTypes;
    return table;
}

void freeTable(struct Table* table) {
    if (table != NULL) {
        if (table->nameTypes != NULL) {
            for (uint32_t i = 0; i < table->fieldsNumber; i++) {
                freeNameType(&(table->nameTypes[i]));
            }
            free(table->nameTypes);
        }
        if (table->tableName != NULL) free(table->tableName);
        if (table->records != NULL) {
            for (uint32_t i = 0; i < table->fieldsNumber; i++) {
                freeEntityRecord(&(table->records[i]), table->fieldsNumber);
            }
            free(table->records);
        }
        free(table);
    }
}

struct EntityRecord* createEntityRecord(struct FieldValue* fieldValue, struct Table* table) {
    struct EntityRecord* entityRecord = initEntityRecord(table);
    entityRecord->fields = fieldValue;
    return entityRecord;
}

void addEntityRecordToTable(struct Table* table, struct EntityRecord* entityRecord) {
    if (table->capacity >= table->recordsNumber) {
        uint32_t newCapacity = table->capacity * 2;
        table->records = realloc(table->records, newCapacity * sizeof (struct EntityRecord));
        if (table->records == NULL) errorAllocation("dynamic->fields while adding value");
        table->capacity = newCapacity;
    }
    table->records[table->recordsNumber] = *entityRecord;
    table->recordsNumber++;
}

struct FieldValue* getValueByIndex(const struct EntityRecord *entityRecord, uint32_t index) {
    return &(entityRecord->fields[index]);
}
