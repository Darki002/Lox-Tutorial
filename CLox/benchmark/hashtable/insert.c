#include "insert.h"

#include <stdlib.h>

static Table table;
static Value values[COUNT];

static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

static void randomString(char *str, const size_t length) {
    if (length) {
        for (size_t n = 0; n < length - 1; n++) {
            const int key = rand() % (int)(sizeof(charset) - 1);
            str[n] = charset[key];
        }
        str[length - 1] = '\0';
    }
}

static ObjString* makeRandomStringObj() {
    const int len = rand()%(101-10) + 10;

    // ReSharper disable once CppDFAMemoryLeak
    ObjString* strObj = malloc(sizeof(ObjString) + (size_t)len + 1);
    if (!strObj) return NULL;

    randomString(strObj->chars, len);
    strObj->hash = hashString(strObj->chars, len);
    strObj->length = len;
    strObj->obj.type = OBJ_STRING;

    return strObj;
}

void setUpInsert() {
    initTable(&table);
    for (int i = 0; i < COUNT; i++) {
        values[i] = OBJ_VAL(makeRandomStringObj());
    }
}

void freeInsert() {
    for (int i = 0; i < COUNT; i++) {
        free(AS_OBJ(values[i]));
    }
}

void runInsert() {
    for (int i = 0; i < COUNT; i++) {
        tableSet(&table, values[i], NUMBER_VAL(i));
    }
}