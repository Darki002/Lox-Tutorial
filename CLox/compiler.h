#ifndef clox_compiler_h
#define clox_compiler_h

#include "object.h"

#define LOOP_STACK_MAX 256

typedef enum { BINDING_LOCAL, BINDING_GLOBAL } BindingKind;

ObjFunction* compile(const char* source);

#endif //clox_compiler_h
