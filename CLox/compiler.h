#ifndef clox_compiler_h
#define clox_compiler_h

#include "object.h"

typedef enum { BINDING_LOCAL, BINDING_UPVALUE, BINDING_GLOBAL } BindingKind;

ObjFunction* compile(const char* source);

#endif //clox_compiler_h
