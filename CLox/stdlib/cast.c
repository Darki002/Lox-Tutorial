#include "cast.h"
#include "../utils/coerce.h"
#include "../object.h"

bool strNative(const int argCount, Value* args) {
    if (argCount != 1) {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'str'.", 41));
        return false;
    }

    args[-1] = toString(args[0]);
    return true;
}

bool numberNative(const int argCount, Value* args) {
    if (argCount != 1) {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'str'.", 41));
        return false;
    }

    Value result;
    if (toNumber(args[0], &result)) {
        args[-1] = result;
        return true;
    }

    args[-1] = OBJ_VAL(copyString("Can not convert value to number.", 43));
    return false;
}

bool boolNative(const int argCount, Value* args) {
    if (argCount != 1) {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'str'.", 41));
        return false;
    }

    args[-1] = toBool(args[0]);
    return true;
}