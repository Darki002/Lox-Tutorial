#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
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
bool writeConstant(Chunk* chunk, Value value, int line);
int getLine(const Chunk* chunk, size_t instruction);

#endif //clox_chunk_h
