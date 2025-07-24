#include "chunk.h"

#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = nullptr;
    initLineArray(&chunk->lines);
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeLineArray(&chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, const uint8_t byte, const int line) {
    if (chunk->capacity < chunk->count + 1) {
        const int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    writeLineArray(&chunk->lines, line);
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

int addConstant(Chunk* chunk, const Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}