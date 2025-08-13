#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define  ALLOCATE_OBJ(type, size, objectType) \
    (type*)allocateObject(size, objectType)

static Obj* allocateObject(const size_t size, const ObjType type) {
    Obj* obj = reallocate(nullptr, 0, size);
    obj->type = type;

    obj->next = vm.objects;
    vm.objects = obj;
    return obj;
}

ObjString* allocateString(const int length) {
    const size_t size = sizeof(ObjString) + length * sizeof(char);
    ObjString* string = ALLOCATE_OBJ(ObjString, size, OBJ_STRING);
    string->length = length;
    return string;
}

ObjString* copyString(const char* chars, const int length) {
    ObjString* string = allocateString(length);
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    return string;
}

void printObject(const Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}