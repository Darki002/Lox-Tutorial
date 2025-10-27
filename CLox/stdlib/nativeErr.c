#include "nativeErr.h"
#include "../object.h"
#include <string.h>

bool errNative(const int argCount, Value* args) {
    switch (argCount) {
        case 0: {
            args[-1] = OBJ_VAL(copyString("Error!", 6));
            return false;
        }
        case 1: {
            if (!IS_STRING(args[0])) {
                args[-1] = OBJ_VAL(copyString("Expected a string as argument.", 30));
                return false;
            }

            const ObjString* message = AS_STRING(args[0]);
            const int len = 7 + message->length;

            ObjString* errMessage = allocateString(len);
            memcpy(errMessage->chars, "Error: ", 7);
            memcpy(errMessage->chars + 7, message->chars, message->length);
            errMessage->chars[len] = '\0';

            args[-1] = OBJ_VAL(errMessage);
            return false;
        }
        default: {
            args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'err'.", 41));
            return false;
        }
    }
}