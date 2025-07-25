#include "common.h"
#include "vm.h"
#include "debug.h"
#include <stdio.h>

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

void initVM() {
    resetStack();
}

void freeVM() {

}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG() (vm.chunk->constants.values[READ_BYTE() | (READ_BYTE() << 8) | (READ_BYTE() << 16)])

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif //DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_RETURN:
                return INTERPRET_OK;
            case OP_CONSTANT:
                const Value constant = READ_CONSTANT();
                printValue(constant);
                printf("\n");
                break;
            case OP_CONSTANT_LONG:
                const Value constantLong = READ_CONSTANT_LONG();
                printValue(constantLong);
                printf("\n");
                break;
        }
    }

#undef READ_CONSTANT_LONG
#undef READ_CONSTANT
#undef READ_BYTE
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

void push(Value value);
Value Pop();
