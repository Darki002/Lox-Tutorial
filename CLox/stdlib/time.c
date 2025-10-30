#include <time.h>
#include "time.h"
#include "../object.h"

#include <unistd.h>

bool clockNative(int _, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}

bool sleepNative(const int argCount, Value* args) {
    if (argCount != 1) {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'sleep'.", 43));
        return false;
    }

    const Value value = args[0];
    if (!IS_NUMBER(value)) {
        args[-1] = OBJ_VAL(copyString("Expected number as argument for 'sleep'.", 40));
        return false;
    }

    sleep(AS_NUMBER(value));
    args[-1] = NIL_VAL;
    return true;
}