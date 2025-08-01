#include "value.h"

#include <stdio.h>

#include "memory.h"

void initValueArray(ValueArray* array) {
    array->count = 0;
    array->capacity = 0;
    array->values = nullptr;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}
void writeValueArray(ValueArray* array, const Value value) {
    if (array->capacity < array->count + 1) {
        const int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }

    array->values[array->count] = value;
    array->count++;
}

void printValue(const Value value) {
    printf("%g", AS_NUMBER(value));
}