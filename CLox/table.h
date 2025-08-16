#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

typedef struct {
    ObjString* key;
    Value value;
} Entry;

typedef struct {
    int count;
    int capacity;
    Entry* entries;
} Table;

#define TABLE_MAX_LOAD 0.75

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(const Table* table, const ObjString* key, Value* value);
bool tableSet(Table* table, const ObjString* key, Value value);
bool tableDelete(const Table* table, const ObjString* key);
void tableAddAll(const Table* from, Table* to);

#endif //clox_table_h
