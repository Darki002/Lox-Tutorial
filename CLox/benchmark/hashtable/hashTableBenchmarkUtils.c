#include "hashTableBenchmarkUtils.h"

#include <stddef.h>
#include <stdlib.h>

static const char charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

void randomString(char *str, const size_t length) {
    if (length) {
        for (size_t n = 0; n < length - 1; n++) {
            const int key = rand() % (int)(sizeof(charset) - 1);
            str[n] = charset[key];
        }
        str[length - 1] = '\0';
    }
}

ObjString* makeRandomStringObj() {
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

ObjString* makeCollisionStringObj() {
    const int len = rand()%(101-10) + 10;

    // ReSharper disable once CppDFAMemoryLeak
    ObjString* strObj = malloc(sizeof(ObjString) + (size_t)len + 1);
    if (!strObj) return NULL;

    randomString(strObj->chars, len);
    strObj->hash = 10;
    strObj->length = len;
    strObj->obj.type = OBJ_STRING;

    return strObj;
}

void shuffleStringObjs(Value *array, const int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        const Value temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
