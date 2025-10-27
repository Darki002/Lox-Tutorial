#include <stdio.h>
#include <string.h>

#include "coerce.h"

#include <stdlib.h>

#include "../object.h"

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

static ObjString* objectToString(const Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_FUNCTION:
            return functionToString(AS_FUNCTION(value));
        case OBJ_NATIVE:
            return copyString("<native fn>", 11);
        case OBJ_STRING:
            return AS_STRING(value);
        default: return NULL; // Unreachable
    }
}

Value toString(const Value value) {
    switch (value.type) {
        case VAL_BOOL: {
            ObjString* string = AS_BOOL(value)
                ? copyString("true", 4)
                : copyString("false", 5);
            return OBJ_VAL(string);
        }
        case VAL_NIL: copyString("nil", 3);
        case VAL_NUMBER: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%.17g", AS_NUMBER(value));
            return OBJ_VAL(copyString(buffer, (int)strlen(buffer)));
        }
        case VAL_OBJ: return OBJ_VAL(objectToString(value));
        case VAL_UNDEFINED: return  OBJ_VAL(copyString( "undefined", 9));
        default: return UNDEFINED_VAL; // Unreachable
    }
}

Value toBool(const Value value) {
    if (IS_NIL(value)) return BOOL_VAL(false);
    if (IS_BOOL(value)) return value;
    return BOOL_VAL(true);
}

bool toNumber(const Value value, Value* out) {
    if (IS_NUMBER(value)) { *out = value; return true; }
    if (IS_BOOL(value))   { *out = NUMBER_VAL(AS_BOOL(value) ? 1 : 0); return true; }
    if (IS_NIL(value))    { return false; }

    const char* start = AS_CSTRING(value);
    char* end = NULL;

    errno = 0;
    const double result = strtod(start, &end); // Uses current locale for decimal point

    while (*end == ' ' || *end == '\r' || *end == '\t' || *end == '\n') end++;

    if (*end != '\0') return false;
    if (errno == ERANGE) return false;

    *out = NUMBER_VAL(result);
    return true;
}
