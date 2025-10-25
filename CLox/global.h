#ifndef COMPILER_C_GLOBAL_H
#define COMPILER_C_GLOBAL_H

#include "common.h"
#include "memory.h"
#include "table.h"
#include "vm.h"

#define SET_GLOBAL(index, v) (vm.globals.values[index].value = v)

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

static inline bool getGlobal(int index, Value* value) {
    value = &vm.globals.values[index].value;
    return !IS_UNDEFINED(*value);
}

static inline bool setGlobal(int index, Value value) {
    if (IS_UNDEFINED(vm.globals.values[index].value)) return false;
    vm.globals.values[index].value = value;
    return true;
}

#endif //COMPILER_C_GLOBAL_H