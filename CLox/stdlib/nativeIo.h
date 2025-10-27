#ifndef CLOX_NATIVEIO_H
#define CLOX_NATIVEIO_H

#include "../common.h"
#include "../value.h"

bool readNative(int argCount, Value* args);

#endif //CLOX_NATIVEIO_H