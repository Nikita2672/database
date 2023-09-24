#include <stdio.h>
#include <stdlib.h>
#include "errors.h"


#define DEFAULT_CAPACITY 200

enum DataType {
    INT,
    DOUBLE,
    STRING,
    BOOL
};

struct FieldValue {
    enum DataType type;
    void* data;
};

struct NameValue {
    char* name;
    void* value;
};

struct TypeName {
    enum DataType type;
    char* name;
};

struct EntityRecord {
    struct FieldValue *fields;
};

struct Table {
    char* tableName;
    struct EntityRecord *records;
    struct TypeName *typeNames;
    u_int32_t fieldsNumber;
    u_int32_t recordsNumber;
    u_int32_t capacity;
};

struct Schema {
    struct Table* tables;
};


struct Table* initTable(struct TypeName* typeNames, u_int32_t fieldsNumber) {
    struct Table* table = malloc(sizeof (struct Table));
    table->capacity = DEFAULT_CAPACITY;
    table->recordsNumber = 0;
    table->typeNames = typeNames;
    table->fieldsNumber = fieldsNumber;
    return table;
}

struct EntityRecord* initEntityRecord(u_int32_t fieldsNumber) {
    struct EntityRecord* pEntityRecord = malloc(sizeof (struct EntityRecord));
    if (pEntityRecord == NULL) errorAllocation("pEntityRecord");
    pEntityRecord->fields = malloc(fieldsNumber * sizeof (struct FieldValue));
    if (pEntityRecord->fields == NULL) errorAllocation("pEntityRecord->fields while init");
    return pEntityRecord;
}

struct FieldValue* initFieldValue(enum DataType dataType, void* dataP) {
    struct FieldValue* dataValue = malloc(sizeof (struct FieldValue));
    if (dataValue == NULL) errorAllocation("dataValue");
    dataValue->data = dataP;
    dataValue->type = dataType;
    return dataValue;
}

void addValueToEntityRecord(struct EntityRecord* entityRecord,
                            enum DataType dataType,
                            void *value,
                            u_int32_t position) {
    struct FieldValue* fieldValue = initFieldValue(dataType, value);
    entityRecord->fields[position] = *fieldValue;
}

struct EntityRecord* createEntityRecord(struct Table* table, struct NameValue* nameValue) {
    struct EntityRecord* entityRecord = initEntityRecord(table->fieldsNumber);
    for (u_int32_t position = 0; position < table->fieldsNumber; position++) {
        addValueToEntityRecord(entityRecord,
                               table->typeNames[position].type,
                               nameValue[position].value,
                               position);
    }
}



void addEntityRecord(struct Table* table, struct EntityRecord* entityRecord) {
    if (table->capacity >= table->recordsNumber) {
        u_int32_t newCapacity = table->capacity * 2;
        table->records = realloc(table->records, newCapacity * sizeof (struct EntityRecord));
        if (table->records == NULL) errorAllocation("dynamic->fields while adding value");
        table->capacity = newCapacity;
    }
    table->records[table->recordsNumber] = *entityRecord;
    table->recordsNumber++;
}

struct FieldValue* getValueByIndex(const struct EntityRecord *dynArray, u_int32_t index) {
    return &(dynArray->fields[index]);
}

void freeEntityRecords(struct EntityRecord *entityRecords, u_int32_t fieldsNumber) {
    for (int i = 0; i < fieldsNumber; i++) {
        if (entityRecords->fields[i].type == STRING) {
            free(entityRecords->fields[i].data);
        }
    }
    free(entityRecords->fields);
    free(entityRecords);
}

void

void freeTable(struct Table *table) {
    freeEntityRecords(table->records, table->fieldsNumber);
    table.
}




int main() {

}
