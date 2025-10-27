#include <stdio.h>

#include "nativeIo.h"
#include "../object.h"
#include "../utils/io.h"

bool readNative(const int argCount, Value* args) {
    if (argCount > 1) {
        args[-1] = OBJ_VAL(copyString("Too many arguments.", 19));
        return false;
    }
    if (argCount == 1) {
        if (!IS_STRING(args[0])) {
            args[-1] = OBJ_VAL(copyString("Expected a string as argument.", 30));
            return false;
        }
        const int len = AS_STRING(args[0])->length;
        printf("%.*s", len, AS_CSTRING(args[0]));
    }

    const ObjString* result = readLine(stdin);

    if (result == NULL) {
        args[-1] = OBJ_VAL(copyString("Error during reading from stdin.", 32));
        return false;
    }

    args[-1] = OBJ_VAL(result);
    return true;
}