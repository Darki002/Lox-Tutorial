#include "cast.h"
#include "../utils/coerce.h"
#include "../object.h"

bool strNative(const int argCount, Value *args)
{
    if (argCount != 1)
    {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'str'.", 41));
        return false;
    }

    args[-1] = toString(args[0]);
    return true;
}

bool numberNative(const int argCount, Value *args)
{
    if (argCount != 1)
    {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'number'.", 44));
        return false;
    }

    Value result;
    if (toNumber(args[0], &result))
    {
        args[-1] = result;
        return true;
    }

    args[-1] = OBJ_VAL(copyString("Can not convert value to number.", 32));
    return false;
}

bool tryNumberNative(const int argCount, Value *args)
{
    if (argCount != 1)
    {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'tryNumber'.", 47));
        return false;
    }

    Value result;
    args[-1] = toNumber(args[0], &result) ? result : NIL_VAL;
    return true;
}

bool boolNative(const int argCount, Value *args)
{
    if (argCount != 1)
    {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'bool'.", 42));
        return false;
    }

    args[-1] = toBool(args[0]);
    return true;
}