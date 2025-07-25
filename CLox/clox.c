#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

// TODO: how to make tests for a language. Write it in the language its self, but how to automatically run the tests and validate them?

int main(int argc, const char* argv[]) {
    initVM();

    Chunk chunk;
    initChunk(&chunk);

    writeConstant(&chunk, 1.2, 123);
    writeChunk(&chunk, OP_RETURN, 123);
    disassembleChunk(&chunk, "test chunk");
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);

    return 0;
}