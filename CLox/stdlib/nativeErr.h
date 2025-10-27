#ifndef CLOX_NATIVEERR_H
#define CLOX_NATIVEERR_H

#include "../common.h"
#include "../value.h"

bool errNative(int argCount, Value* args);

#endif //CLOX_NATIVEERR_H