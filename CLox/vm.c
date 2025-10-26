#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "vm.h"

#include <time.h>

#include "debug.h"
#include "compiler.h"
#include "memory.h"
#include "object.h"

VM vm;

static bool clockNative(int _, Value* args) {
    args[-1] = NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    return true;
}

static bool errNative(int _, Value* args) {
    args[-1] = OBJ_VAL(copyString("Error!", 6));
    return false;
}

static void resetStack() {
    vm.stackTop = vm.stack;
    vm.frameCount = 0;
}

static void runtimeError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    for (int i = vm.frameCount - 1; i >= 0; i--) {
        const CallFrame* frame = &vm.frames[i];
        const ObjFunction* function = frame->function;
        const size_t instruction = frame->ip - function->chunk.code - 1;
        const int line = getLine(&frame->function->chunk, instruction);

        fprintf(stderr, "[line %d] in ", line);
        if (function->name == NULL) {
            fprintf(stderr, "script\n");
        } else {
            fprintf(stderr, "%s()\n", function->name->chars);
        }
    }

    resetStack();
}

static void defineNative(const char* name, const NativeFn function) {
    push(OBJ_VAL(copyString(name, (int)strlen(name))));
    push(OBJ_VAL(newNative(function)));
    defineGlobal(&vm.globals, AS_STRING(vm.stack[0]), vm.stack[1], true);
    popn(2);
}

void initVM() {
    resetStack();
    vm.objects = NULL;

    initGlobals(&vm.globals);
    initTable(&vm.strings);

    defineNative("clock", clockNative);
    defineNative("err", errNative);
}

void freeVM() {
    freeObjects();
    freeGlobals(&vm.globals);
    freeTable(&vm.strings);
}

void push(const Value value) {
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop() {
    vm.stackTop--;
    return *vm.stackTop;
}

Value popn(const int n) {
    vm.stackTop -= n;
    return *vm.stackTop;
}

Value peek(const int distance) {
    return vm.stackTop[-1 - distance];
}

void replace(const Value value) {
    *(vm.stackTop - 1) = value;
}

static bool call(ObjFunction* function, const uint8_t argCount) {
    if (argCount != function->arity) {
        runtimeError("Expected %d arguments but got %d", function->arity, argCount);
        return false;
    }

    if (vm.frameCount == FRAMES_MAX) {
        runtimeError("Stack overflow.");
        return false;
    }

    CallFrame* frame = &vm.frames[vm.frameCount++];
    frame->function = function;
    frame->ip = function->chunk.code;
    frame->slots = vm.stackTop - argCount - 1;
    return true;
}

static bool callValue(const Value callee, const uint8_t argCount) {
    if (IS_OBJ(callee)) {
        switch (OBJ_TYPE(callee)) {
            case OBJ_FUNCTION:
                return call(AS_FUNCTION(callee), argCount);
            case OBJ_NATIVE: {
                const NativeFn native = AS_NATIVE(callee);
                if (native(argCount, vm.stackTop - argCount)) {
                    vm.stackTop -= argCount;
                    return true;
                }
                runtimeError(AS_STRING(vm.stackTop[-argCount - 1])->chars);
                return false;
            }
            default:
                break;
        }
    }
    runtimeError("Can only call functions and classes.");
    return false;
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
    CallFrame* frame = &vm.frames[vm.frameCount - 1];
    register uint8_t* ip = frame->ip;

#define READ_U8() (*ip++)
#define READ_U16() (ip += 2, (uint16_t)((ip[-2] << 8) | ip[-1]))
#define READ_U24() (ip += 3, (int)((ip[-3] << 16) | (uint16_t)((ip[-2] << 8) | ip[-1])))
#define READ_INDEX() (wideInstruction ? READ_U24() : READ_U8())
#define READ_CONSTANT() (frame->function->chunk.constants.values[READ_INDEX()])
#define BINARY_OP(valueType, op) \
    do { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        const double b = AS_NUMBER(pop()); \
        const double a = AS_NUMBER(peek(0)); \
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
        disassembleInstruction(&frame->function->chunk, (int)(ip - frame->function->chunk.code));
#endif //DEBUG_TRACE_EXECUTION

        uint8_t instruction  = READ_U8();
        bool wideInstruction = false;

        if (instruction == OP_WIDE) {
            wideInstruction = true;
            instruction = READ_U8();
        }

        switch (instruction) {
            case OP_CONSTANT: {
                const Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_CONSTANT_M1: push(NUMBER_VAL(-1)); break;
            case OP_CONSTANT_0:  push(NUMBER_VAL(0)); break;
            case OP_CONSTANT_1:  push(NUMBER_VAL(1)); break;
            case OP_CONSTANT_2:  push(NUMBER_VAL(2)); break;
            case OP_NIL: push(NIL_VAL); break;
            case OP_TRUE: push(BOOL_VAL(true)); break;
            case OP_FALSE: push(BOOL_VAL(false)); break;
            case OP_POP: pop(); break;
            case OP_POPN: {
                const int popCount = READ_INDEX();
                popn(popCount);
                break;
            }
            case OP_DUP: push(peek(0)); break;
            case OP_GET_LOCAL: {
                const int slot = READ_INDEX();
                push(frame->slots[slot]);
                break;
            }
            case OP_SET_LOCAL: {
                const int slot = READ_INDEX();
                frame->slots[slot] = peek(0);
                break;
            }
            case OP_INC_LOCAL: {
                const int slot = READ_INDEX();
                const int8_t imm = READ_U8();

                const Value value = frame->slots[slot];
                if (!IS_NUMBER(value)) {
                    frame->ip = ip;
                    runtimeError("Operands must be a numbers.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                const Value newValue = NUMBER_VAL(AS_NUMBER(value) + imm);
                frame->slots[slot] = newValue;
                push(newValue);
                break;
            }
            case OP_DEC_LOCAL: {
                const int slot = READ_INDEX();
                const int8_t imm = READ_U8();

                const Value value = frame->slots[slot];
                if (!IS_NUMBER(value)) {
                    frame->ip = ip;
                    runtimeError("Operands must be a numbers.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                const Value newValue = NUMBER_VAL(AS_NUMBER(value) - imm);
                frame->slots[slot] = newValue;
                push(newValue);
                break;
            }
            case OP_GET_GLOBAL: {
                const int index = READ_INDEX();
                Value value;
                if (!getGlobal(vm.globals, index, &value)) {
                    frame->ip = ip;
                    runtimeError("Undefined variable.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(value);
                break;
            }
            case OP_DEFINE_GLOBAL: {
                const int index = READ_INDEX();
                SET_GLOBAL(index, pop());
                break;
            }
            case OP_SET_GLOBAL: {
                const int index = READ_INDEX();
                if (!setGlobal(vm.globals, index, peek(0))) {
                    frame->ip = ip;
                    runtimeError("Undefined variable.");
                    return INTERPRET_RUNTIME_ERROR;
                }
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
                    frame->ip = ip;
                    runtimeError("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -); break;
            case OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIVIDE:   BINARY_OP(NUMBER_VAL, /); break;
            case OP_MOD: {
                if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {
                    frame->ip = ip;
                    runtimeError("Operands must be numbers.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                const double b = AS_NUMBER(pop());
                const double a = AS_NUMBER(peek(0));
                const double result = fmod(a, b);
                replace(NUMBER_VAL(result));
                break;
            }
            case OP_NOT: {
                replace(BOOL_VAL(isFalsey(peek(0))));
                break;
            }
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(0))) {
                    frame->ip = ip;
                    runtimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                replace(NUMBER_VAL(-AS_NUMBER(peek(0))));
                break;
            }
            case OP_PRINT: {
                printValue(pop());
                printf("\n");
                break;
            }
            case OP_JUMP: {
                const uint16_t offset = READ_U16();
                ip += offset;
                break;
            }
            case OP_JUMP_IF_TRUE: {
                const uint16_t offset = READ_U16();
                if (!isFalsey(peek(0))) ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                const uint16_t offset = READ_U16();
                if (isFalsey(peek(0))) ip += offset;
                break;
            }
            case OP_JUMP_IF_NOT_EQUAL: {
                const uint16_t offset = READ_U16();
                if (!valuesEqual(peek(0), peek(1))) ip += offset;
                break;
            }
            case OP_LOOP: {
                const uint16_t offset = READ_U16();
                ip -= offset;
                break;
            }
            case OP_LOOP_IF_FALSE: {
                const uint16_t offset = READ_U16();
                if (isFalsey(peek(0))) ip -= offset;
                break;
            }
            case OP_CALL: {
                const uint8_t argCount = READ_U8();
                frame->ip = ip;
                if (!callValue(peek(argCount), argCount)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm.frames[vm.frameCount - 1];
                ip = frame->ip;
                break;
            }
            case OP_RETURN: {
                const Value result = pop();
                vm.frameCount--;
                if (vm.frameCount == 0) {
                    pop();
                    return INTERPRET_OK;
                }

                vm.stackTop = frame->slots;
                push(result);
                frame = &vm.frames[vm.frameCount - 1];
                ip = frame->ip;
                break;
            }
            default:
                break; // Unreachable
        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_INDEX
#undef READ_U24
#undef READ_U16
#undef READ_U8
}

InterpretResult interpret(const char* source) {
    ObjFunction* function = compile(source);
    if (function == NULL) return INTERPRET_COMPILE_ERROR;

    push(OBJ_VAL(function));
    call(function, 0);

    return run();
}
