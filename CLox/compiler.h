#ifndef clox_compiler_h
#define clox_compiler_h
#include "chunk.h"

typedef enum { BINDING_LOCAL, BINDING_GLOBAL } BindingKind;

bool compile(const char* source, Chunk* chunk);

#endif //clox_compiler_h
