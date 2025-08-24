#include <stdlib.h>
#include <string.h>

#include "insert.h"

#include "../../object.h"
#include "../../table.h"

Table table;

Value values[1000];

void setUp() {
    initTable(&table);

    for (int i = 0; i < 1000; i++) {
        const char* str = "str";
        ObjString* strObj = realloc(nullptr, sizeof(ObjString) + strlen(str));
        strObj->length = (int)strlen(str);
        memcpy(strObj->chars, str, strObj->length);
        strObj->hash = hashString(str, strObj->length);
        strObj->obj.type = OBJ_STRING;
        values[i] = OBJ_VAL(strObj);
    }
}

void freeBench() {
    for (int i = 0; i < 1000; i++) {
        free(AS_OBJ(values[i]));
    }
}

void run() {
    for (int i = 0; i < 1000; i++) {
        tableSet(&table, values[i], NIL_VAL);
    }
}