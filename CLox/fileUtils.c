#include <stdlib.h>

#include "fileUtils.h"
#include "object.h"

ObjString* readLine(FILE* in) {
    int bufferSize = 128;
    int len = 0;
    char* buffer = malloc(bufferSize);
    if (!buffer) return NULL;

    for (;;) {
        const int c = fgetc(in);
        if (c == EOF) {
            if (ferror(in) || len == 0) {
                free(buffer);
                return NULL;
            }
            break;
        }
        if (c == '\n') break;
        if (len + 1 >= bufferSize) {
            const int newCap = bufferSize * 2;
            char* newBuffer = realloc(buffer, newCap);
            if (!newBuffer) {
                free(buffer);
                return NULL;
            }
            buffer = newBuffer;
            bufferSize = newCap;
        }
        buffer[len++] = (char)c;
    }

    if (len > 0 && buffer[len - 1] == '\r') len--;
    buffer[len] = '\0';

    ObjString* string = copyString(buffer, len);
    free(buffer);
    return string;
}
