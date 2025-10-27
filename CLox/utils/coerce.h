#ifndef CLOX_COERCE_H
#define CLOX_COERCE_H

#include "../common.h"
#include "../value.h"

Value toString(Value value);
Value toBool(Value value);
bool toNumber(Value value, Value* out);

#endif //CLOX_COERCE_H