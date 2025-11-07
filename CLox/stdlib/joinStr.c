#include "joinStr.h"
#include "../utils/stringUtils.h"

#include <string.h>

bool joinStrNative(int argCount, Value *args)
{
    args[-1] = joinString(argCount, args);
    return true;
}