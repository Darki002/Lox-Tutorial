#include "classUtils.h"
#include "../object.h"

bool hasPropertyNative(int argCount, Value *args) {
    if (argCount != 2)
    {
        args[-1] = OBJ_VAL(copyString("Unexpected amount of arguments for 'hasProperty'.", 41));
        return false;
    }

    if(!IS_INSTANCE(args[0])){
        args[-1] = OBJ_VAL(copyString("Expected instance as first argument for 'hasProperty'.", 41));
        return false;
    }

    if(!IS_STRING(args[1])){
        args[-1] = OBJ_VAL(copyString("Expected string as second argument for 'hasProperty'.", 41));
        return false;
    }

    const ObjInstance* instance = AS_INSTANCE(args[0]);

    Value v;
    const bool result = tableGet(&instance->fields, args[1], &v);
    args[-1] = BOOL_VAL(result);
    return true;
}