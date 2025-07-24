#include "line.h"

#include "memory.h"

void initLineArray(LineArray* array) {
    array->count = 0;
    array->capacity = 0;
    array->length = nullptr;
    array-> lines = nullptr;
}
void freeLineArray(LineArray* array) {
    FREE_ARRAY(int, array->length, array->capacity);
    FREE_ARRAY(int, array->lines, array->capacity);
    initLineArray(array);
}
void writeLineArray(LineArray* array, const int line) {
    if (array->lines != nullptr && array->lines[array->count] == line) {
        array->length[array->count]++;
    } else {
        if (array->capacity < array->count + 1) {
            const int oldCapacity = array->capacity;
            array->capacity = GROW_CAPACITY(oldCapacity);
            array->lines = GROW_ARRAY(int, array->lines, oldCapacity, array->capacity);
            array->length = GROW_ARRAY(int, array->length, oldCapacity, array->capacity);
        }

        array->lines[array->count] = line;
        array->length[array->count] = 1;
        array->count++;
    }
}

int getLine(const LineArray* array, int offset) {
    for (int i = 0; i < array->count; i++) {
        if (array->length[i] >= offset) {
            return array->lines[i];
        }
        offset -= array->length[i];
    }

    return -1;
}
