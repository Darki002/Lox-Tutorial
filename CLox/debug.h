#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

void disassembleChunk(const Chunk* chunk, const char* name);
int disassembleInstruction(const Chunk* chunk, int offset);

#define disassembleU24Constant(chunk, offset) (chunk->code[offset + 1] | (chunk->code[offset + 2] << 8) | (chunk->code[offset + 3] << 16))

#endif //clox_debug_h
