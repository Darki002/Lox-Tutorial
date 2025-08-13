#include "memory.h"
#include <stdlib.h>

#include "value.h"
#include "vm.h"

/*
 oldSize | newSize | Operation
 0 | Non-zero | Allocate new block
 Non-zero | 0 | Free allocation
 Non-Zero | Smaller than oldSize | Shrink existing allocation
 Non-zero | Larger than oldSize | Grow existing allocation.
 */
void* reallocate(void* pointer, size_t oldSize, const size_t newSize) { //TODO: why is oldSize even there? do we use that later on?
    if (newSize == 0) {
        free(pointer);
        return nullptr;
    }

    void* result = realloc(pointer, newSize);
    if (result == nullptr) exit(1);
    return result;
}

static void freeObject(Obj* object) {
    switch (object->type) {
        case OBJ_STRING: {
            const ObjString* string = (ObjString*)object;
            reallocate(object, sizeof(ObjString) + string->length + 1, 0);
        }
    }
}

void freeObjects() {
    Obj* object = vm.objects;
    while (object != nullptr) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}