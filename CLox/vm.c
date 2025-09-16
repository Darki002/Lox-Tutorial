#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"

VM vm;

static void resetStack() {
    vm.stackTop = vm.stack;
}

static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    const size_t instruction = vm.ip - vm.chunk->code - 1;
    const int line = getLine(vm.chunk, instruction);
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

void initVM() {
    resetStack();
    vm.objects = NULL;
    initTable(&vm.globalNames);
    initValueArray(&vm.globalValues);
    initTable(&vm.strings);
}

void freeVM() {
    freeObjects();
    initTable(&vm.globalNames);
    freeValueArray(&vm.globalValues);
    freeTable(&vm.strings);
}

void push(const Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return  *vm.stackTop;
}

Value peek(const int distance) {
    return vm.stackTop[-1 - distance];
}

void replace(const Value value) {
    *(vm.stackTop - 1) = value;
}

static bool isFalsey(const Value value) {
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate() {
    const ObjString* b = AS_STRING(pop());
    const ObjString* a = AS_STRING(peek(0));

    const int length = a->length + b->length;
    ObjString* result = allocateString(length);
    memcpy(result->chars, a->chars, a->length);
    memcpy(result->chars + a->length, b->chars, b->length);
    result->chars[length] = '\0';
    result->hash = hashString(result->chars, length);

    ObjString* interned = tableFindString(&vm.strings, result->chars, length, result->hash);

    if (interned != NULL) {
        reallocate(result, sizeof(ObjString) + result->length + 1, 0);
        result = interned;
    }

    const Value resultVal = OBJ_VAL(result);
    tableSet(&vm.strings, resultVal, NIL_VAL);
    replace(resultVal);
}

static InterpretResult run() {
#define READ_U8() (*vm.ip++)
#define READ_U24() (READ_U8() | (READ_U8() << 8) | (READ_U8() << 16))
#define READ_INDEX() (wideInstruction ? READ_U24() : READ_U8())
#define READ_CONSTANT() (vm.chunk->constants.values[READ_INDEX()])
#define BINARY_OP(valueType, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operarands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = AS_NUMBER(pop()); \
        double a = AS_NUMBER(peek(0)); \
        replace(valueType(a op b));\
    } while(false);

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (const Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif //DEBUG_TRACE_EXECUTION

        uint8_t instruction  = READ_U8();
        bool wideInstruction = false;

        if (instruction == OP_WIDE) {
            wideInstruction = true;
            instruction = READ_U8();
        }

        switch (instruction) {
            case OP_RETURN:
                return INTERPRET_OK;
            case OP_CONSTANT:
                const Value constant = READ_CONSTANT();
                push(constant);
                break;
            case OP_NIL: push(NIL_VAL); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;
            case OP_POP: pop(); break;
            case OP_GET_GLOBAL: {
                const int index = READ_INDEX();
                const Value value = vm.globalValues.values[index];
                if (IS_UNDEFINED(value)) {
                    runtimeError("Undefined variable.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(value);
                break;
            }
            case OP_DEFINE_GLOBAL: {
                const int index = READ_INDEX();
                vm.globalValues.values[index] = pop();
                break;
            }
            case OP_SET_GLOBAL: {
                const int index = READ_INDEX();
                if (IS_UNDEFINED(vm.globalValues.values[index])) {
                    runtimeError("Undefined variable.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                vm.globalValues.values[index] = peek(0);
                break;
            }
            case OP_EQUAL: {
                const Value b = pop();
                const Value a = peek(0);
                replace(BOOL_VAL(valuesEqual(a, b)));
                break;
            }
            case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
            case OP_ADD: {
                if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                    concatenate();
                } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                    BINARY_OP(NUMBER_VAL, +);
                } else {
                    runtimeError("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
            case OP_NOT: {
                replace(BOOL_VAL(isFalsey(peek(0))));
                break;
            }
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(0))) {
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                replace(NUMBER_VAL(-AS_NUMBER(peek(0))));
                break;
            }
            case OP_PRINT: {
                printValue(pop());
                printf("\n");
                break;;
            }
        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_INDEX
#undef READ_U24
#undef READ_U8
}

InterpretResult interpret(const char* source) {
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    const InterpretResult result = run();
    freeChunk(&chunk);
    return result;
}
