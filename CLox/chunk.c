#include "chunk.h"

#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lineCount = 0;
    chunk->lineCapacity = 0;
    chunk->lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(LineStart, chunk->lines, chunk->capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

void writeChunk(Chunk* chunk, const uint8_t byte, const int line) {
    if (chunk->capacity < chunk->count + 1) {
        const int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count++] = byte;

    if (chunk->lineCount > 0 && chunk->lines[chunk->lineCount - 1].line == line) {
        return;
    }

    if (chunk->lineCapacity < chunk->lineCount + 1) {
        const int oldCapacity = chunk->lineCapacity;
        chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
        chunk->lines = GROW_ARRAY(LineStart, chunk->lines, oldCapacity, chunk->lineCapacity);
    }

    LineStart* lineStart = &chunk->lines[chunk->lineCount++];
    lineStart->offset = chunk->count - 1;
    lineStart->line = line;
}

int addConstant(Chunk* chunk, const Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

bool writeConstantCode(const OpCode code, Chunk* chunk, const Value value, const int line) {
    const int offset = addConstant(chunk, value);
    if (offset < 256) {
        writeChunk(chunk, code, line);
        writeChunk(chunk, offset, line);
        return true;
    }
    if (offset < 0xFFFFFF) {
        writeChunk(chunk, OP_WIDE, line);
        writeChunk(chunk, code, line);
        writeChunk(chunk, offset & 0xff, line);
        writeChunk(chunk, (offset >> 8) & 0xff, line);
        writeChunk(chunk, (offset >> 16) & 0xff, line);
        return true;
    }

    return false;
}

bool writeConstant(Chunk* chunk, const Value value, const int line) {
    return writeConstantCode(OP_CONSTANT, chunk, value, line);
}

int getLine(const Chunk* chunk, const size_t instruction) {
    int start = 0;
    int end = chunk->lineCount - 1;

    while (true) {
        const int mid = (start + end) / 2;
        const LineStart* line = &chunk->lines[mid];

        if (instruction < line->offset) {
            end = mid - 1;
        } else if (mid == chunk->lineCount - 1 || instruction < chunk->lines[mid + 1].offset) {
            return line->line;
        } else {
            start = mid + 1;
        }
    }
}