#ifndef COMPILER_C_GLOBAL_H
#define COMPILER_C_GLOBAL_H

#include "common.h"
#include "memory.h"
#include "table.h"

typedef struct {
    Value value;
    bool immutable;
} Global;

typedef struct {
    Table globalNames;
    int capacity;
    int count;
    Global* values;
} Globals;

int declareGlobal(const ObjString* name, bool immutable);
void defineGlobal(const ObjString* name, Value value, bool immutable);

#endif //COMPILER_C_GLOBAL_H