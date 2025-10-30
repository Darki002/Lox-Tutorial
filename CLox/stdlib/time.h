#ifndef CLOX_TIME_H
#define CLOX_TIME_H

#include "../common.h"
#include "../value.h"

bool clockNative(int _, Value* args);
bool sleepNative(int argCount, Value* args);

#endif //CLOX_TIME_H