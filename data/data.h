#include <stdint-gcc.h>
#include "stdlib.h"

#ifndef LAB1_DATA_H
#define LAB1_DATA_H


enum DataType {
    INT,
    DOUBLE,
    STRING,
    BOOL
};


struct FieldValue {
    enum DataType type;
    char* fieldName;
    void* data;
    uint64_t dataSize;
};

struct NameType {
    char *fieldName;
    enum DataType dataType;
};


struct EntityRecord {
    struct FieldValue *fields;
};

struct Table {
    char* tableName;
    struct EntityRecord *records;
    struct NameType* nameTypes;
    uint32_t fieldsNumber;
    uint32_t recordsNumber;
    uint32_t capacity;

};

struct Schema {
    struct Table* tables;
};

struct Table* initTable(uint32_t fieldsNumber, struct NameType* nameTypes);

struct EntityRecord* initEntityRecord(struct Table* table);

struct FieldValue* initFieldValue(enum DataType dataType, void* dataP, char* fieldName);

void addValueToEntityRecord(struct EntityRecord* entityRecord,
                            struct FieldValue* fieldValue,
                            uint32_t position);

struct EntityRecord* createEntityRecord(struct FieldValue* fieldValue, struct Table* table);
void addEntityRecordToTable(struct Table* table, struct EntityRecord* entityRecord);

void freeEntityRecord(struct EntityRecord *entityRecord, uint32_t fieldsNumber);

struct FieldValue* getValueByIndex(const struct EntityRecord *entityRecord, uint32_t index);
void printEntityRecord(struct EntityRecord *entityRecord, uint16_t fieldsNumber);
#endif //LAB1_DATA_H
