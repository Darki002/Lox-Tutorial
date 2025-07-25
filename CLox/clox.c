#include <stdio.h>
#include <time.h>

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

    writeConstant(&chunk, 3.4, 123);

    writeChunk(&chunk, OP_ADD, 123);

    writeConstant(&chunk, 5.6, 123);

    writeChunk(&chunk, OP_DIVIDE, 123);

    writeChunk(&chunk, OP_NEGATE, 123);
    writeChunk(&chunk, OP_RETURN, 123);
    disassembleChunk(&chunk, "test chunk");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    interpret(&chunk);

    clock_gettime(CLOCK_MONOTONIC, &end);
    const long long time_spent_ns = (end.tv_sec - start.tv_sec) * 1000000000LL
                              + (end.tv_nsec - start.tv_nsec);
    printf("Time: %lld nanoseconds\n", time_spent_ns);

    freeVM();
    freeChunk(&chunk);

    return 0;
}