#ifndef clox_vm_h
#define clox_vm_h

#include "common.h"
#include "chunk.h"
#include "table.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Value value;
    bool immutable;
} Global;

typedef struct {
    Table globalNames;
    int capacity;
    int count;
    Global* values;
} GlobalArray;

typedef struct {
    Chunk* chunk;
    uint8_t* ip; //instruction pointer
    Value stack[STACK_MAX];
    Value* stackTop;
    GlobalArray globals;
    Table strings;
    Obj* objects;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
  } InterpretResult;

extern VM vm;

void initVM();
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();
void freeVM();

#endif //clox_vm_h
