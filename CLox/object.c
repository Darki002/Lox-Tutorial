#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define  ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(const size_t size, const ObjType type) {
    Obj* obj = reallocate(nullptr, 0, size);
    obj->type = type;

    obj->next = vm.objects;
    vm.objects = obj;
    return obj;
}

ObjString* allocateString(const int length) {
    const size_t size = sizeof(ObjString) + length + 1;
    ObjString* string = (ObjString*)allocateObject(size, OBJ_STRING);
    string->length = length;
    return string;
}

uint32_t hashString(const char* key, const int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString* copyString(const char* chars, const int length) {
    ObjString* string = allocateString(length);
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    string->hash = hashString(chars, length);;
    return string;
}

void printObject(const Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}