#ifndef CLOX_CAST_H
#define CLOX_CAST_H

#include "../common.h"
#include "../value.h"

bool strNative(int argCount, Value* args);
bool numberNative(int argCount, Value* args);
bool boolNative(int argCount, Value* args);

#endif //CLOX_CAST_H