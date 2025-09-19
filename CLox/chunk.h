#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// TODO: add op codes for increment 1, decrement 1

typedef enum {
    OP_WIDE,
    OP_CONSTANT,
    OP_CONSTANT_M1,
    OP_CONSTANT_0,
    OP_CONSTANT_1,
    OP_CONSTANT_2,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_RETURN,
} OpCode;

typedef struct {
    int offset;
    int line;
} LineStart;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    ValueArray constants;
    int lineCount;
    int lineCapacity;
    LineStart* lines;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
bool writeIndex(OpCode code, Chunk* chunk, int index, int line);
bool writeConstantCode(OpCode code, Chunk* chunk, Value value, int line);
bool writeConstant(Chunk* chunk, Value value, int line);
int getLine(const Chunk* chunk, size_t instruction);

#endif //clox_chunk_h
