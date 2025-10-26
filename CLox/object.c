#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define  ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(const size_t size, const ObjType type) {
    Obj* obj = reallocate(NULL, 0, size);
    obj->type = type;

    obj->next = vm.objects;
    vm.objects = obj;
    return obj;
}

ObjFunction* newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

ObjNative* newNative(const NativeFn function) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}

ObjString* allocateString(const int length) {
    ObjString* string = (ObjString*)allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
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
    const uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);

    if (interned != NULL) return interned;

    ObjString* string = allocateString(length);
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    string->hash = hash;
    tableSet(&vm.strings, OBJ_VAL(string), NIL_VAL);
    return string;
}

ObjString* concatenateStrings(const char* aChars, const int aLength, const char* bChars, const int bLength) {
    const int length = aLength + bLength;
    ObjString* result = allocateString(length);
    memcpy(result->chars, aChars, aLength);
    memcpy(result->chars + aLength, bChars, bLength);
    result->chars[length] = '\0';
    result->hash = hashString(result->chars, length);

    ObjString* interned = tableFindString(&vm.strings, result->chars, length, result->hash);

    if (interned != NULL) {
        reallocate(result, sizeof(ObjString) + result->length + 1, 0);
        result = interned;
    }

    return result;
}

static void printFunction(const ObjFunction* function) {
    if (function->name == NULL) {
        printf("<script>");
        return;
    }

    printf("<fn %s>", function->name->chars);
}

static ObjString* functionToString(const ObjFunction* function) {
    if (function->name == NULL) {
        return copyString("<script>", 8);
    }

    const int len = 5 + function->name->length;
    char str[len];
    char *p = str;

    memcpy(p, "<fn ", 4);
    p += 4;

    memcpy(p, function->name->chars, function->name->length);
    p += function->name->length;
    *p = '>';

    return copyString(str, len);
}

ObjString* objectToString(const Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_FUNCTION:
            return functionToString(AS_FUNCTION(value));
        case OBJ_NATIVE:
            return copyString("<native fn>", 11);
        case OBJ_STRING:
            return AS_STRING(value);
    }
}

void printObject(const Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_FUNCTION:
            printFunction(AS_FUNCTION(value));
            break;
        case OBJ_NATIVE:
            printf("<native fn>");
            break;
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
    }
}