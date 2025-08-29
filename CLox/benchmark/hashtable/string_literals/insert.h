#ifndef CLOX_INSERT_H
#define CLOX_INSERT_H

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../../../object.h"
#include "../../../table.h"

#define COUNT 109

extern Table table;
extern Value values[COUNT];

void setUp();
void freeBench();
void run();

static void rstripNewline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r')) {
        s[--n] = '\0';
    }
}

static ObjString* makeObjString(const char* bytes, const int len) {
    ObjString* strObj = malloc(sizeof(ObjString) + (size_t)len + 1);
    if (!strObj) return nullptr;

    strObj->length = len;
    memcpy(strObj->chars, bytes, (size_t)len);
    strObj->chars[len] = '\0';
    strObj->hash = hashString(strObj->chars, len);
    strObj->obj.type = OBJ_STRING;

    return strObj;
}

static int loadStringsFromFile(const char* path, Value* outValues, const int maxValues) {
    assert(outValues && maxValues > 0);

    FILE* f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open '%s'\n", path);
        return 0;
    }

    char buf[8192];
    int count = 0;

    while (count < maxValues && fgets(buf, sizeof(buf), f)) {
        rstripNewline(buf);

        if (buf[0] == '\0') {
            continue;
        }

        const int len = (int)strlen(buf);
        ObjString* s = makeObjString(buf, len);
        if (!s) {
            fprintf(stderr, "Error: OOM when creating ObjString\n");
            break;
        }

        outValues[count++] = OBJ_VAL(s);
    }

    fclose(f);
    return count;
}

#endif //CLOX_INSERT_H