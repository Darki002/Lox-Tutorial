#include "global.h"

int declareGlobal(const ObjString* name, const bool immutable) {
    const int newIndex = vm.globals.count;

    if (vm.globals.capacity < vm.globals.count + 1) {
        const int oldCapacity = vm.globals.capacity;
        vm.globals.capacity = GROW_CAPACITY(oldCapacity);
        vm.globals.values = GROW_ARRAY(Global, vm.globals.values, oldCapacity, vm.globals.capacity);
    }

    Global* global = &vm.globals.values[vm.globals.count++];
    global->value = UNDEFINED_VAL;
    global->immutable = immutable;

    tableSet(&vm.globals.globalNames, OBJ_VAL(name), NUMBER_VAL((double)newIndex));
    return newIndex;
}

void defineGlobal(const ObjString* name, const Value value, const bool immutable) {
    const int index = declareGlobal(name, immutable);
   vm.globals.values[index].value = value;
}
