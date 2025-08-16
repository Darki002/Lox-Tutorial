#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"


void initTable(Table* table) {
    table->count = 0;
    table->capacity = 0;
    table->entries = nullptr;
}

void freeTable(Table* table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    initTable(table);
}

static Entry* findEntry(Entry* entries, const int capacity, const ObjString* key) {
    uint32_t index = key->hash % capacity;
    Entry* tombstone = nullptr;

    for (;;) {
        Entry* entry = &entries[index];
        if (entry->key == nullptr) {
            if (IS_NIL(entry->value)) {
                return tombstone != nullptr ? tombstone : entry;
            } else {
                if (tombstone == nullptr) tombstone = entry;
            }
        } else if (entry->key == key) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

bool tableGet(const Table* table, const ObjString* key, Value* value) {
    if (table->count == 0) return false;
    const Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == nullptr) return false;
    *value = entry->value;
    return true;
}

static void adjustCapacity(Table* table, const int capacity) {
    Entry* entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = nullptr;
        entries[i].value = NIL_VAL;
    }

    table->count = 0;
    for (int i = 0; i < table->capacity; i ++) {
        const Entry* entry = &table->entries[i];
        if (entry->key == nullptr) continue;
        Entry* dest = findEntry(entries, capacity, entry->key);
        dest->key = entry->key;
        dest->value = entry->value;
        table->count++;
    }

    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}

bool tableSet(Table* table, const ObjString* key, const Value value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        const int capacity = GROW_CAPACITY(table->capacity);
        adjustCapacity(table, capacity);
    }

    Entry* entry = findEntry(table->entries, table->capacity, key);
    const bool isNewKey = entry->key == nullptr;
    if (isNewKey && IS_NIL(entry->value)) table->count++;

    entry->key;
    entry->value = value;
    return isNewKey;
}

bool tableDelete(const Table* table, const ObjString* key) {
    if (table->count == 0) return false;

    Entry* entry = findEntry(table->entries, table->capacity, key);
    if (entry->key == nullptr) return false;

    entry->key = nullptr;
    entry->value = BOOL_VAL(true);
    return true;
}

void tableAddAll(const Table* from, Table* to) {
    for (int i = 0; i < from->capacity; i++) {
        const Entry* entry = &from->entries[i];
        if (entry->key != nullptr) {
            tableSet(to, entry->key, entry->value);
        }
    }
}
