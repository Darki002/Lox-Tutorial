#include "joinStr.h"
#include "../utils/stringUtils.h"

bool joinStrNative(int argCount, Value *args)
{
    args[-1] = joinString(argCount, args);
    return true;
}