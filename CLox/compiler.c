#include "compiler.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "table.h"
#include "vm.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif //DEBUG_PRINT_CODE

typedef struct {
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
} Parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(bool canAssign);

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

typedef struct {
    Token name;
    int depth;
} Local;

typedef struct {
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
} Compiler;

Parser parser;
Compiler* current = NULL;
Chunk* compilingChunk;

static Chunk* currentChunk() {
    return compilingChunk;
}

static void errorAt(const Token* token, const char* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {

    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void error(const char* message) {
    errorAt(&parser.previous, message);
}

static void errorAtCurrent(const char* message) {
    errorAt(&parser.current, message);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR) break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(const TokenType type, const char* message) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static bool check(const TokenType type) {
    return parser.current.type == type;
}

static bool match(const TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

static void emitByte(const uint8_t byte) {
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(const uint8_t byte1, const uint8_t byte2) {
    emitByte(byte1);
    emitByte(byte2);
}

static void emitReturn() {
    emitByte(OP_RETURN);
}

static void emitConstant(const Value value) {
    const bool result = writeConstant(currentChunk(), value, parser.previous.line);
    if (!result) {
        error("Too many constants in one chunk.");
    }
}

static void initCompiler(Compiler* compiler) {
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    current = compiler;
}

static void endCompiler() {
    emitReturn();

#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif //DEBUG_PRINT_CODE
}

static void beginScope() {
    current->scopeDepth++;
}

static void endScope() {
    current->scopeDepth--;

    const int locals = current->localCount;
    while (current->localCount > 0 && current->locals[current->localCount - 1].depth > current->scopeDepth) {
        current->localCount--;
    }

    const int countPops = locals - current->localCount;
    writeIndexBytes(OP_POPN, currentChunk(), countPops);
}

static void expression();
static void statement();
static void declaration();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void emitIndex(const OpCode code, const int index, const int line) {
    const bool result = writeIndex(code, currentChunk(), index, line);
    if (!result) {
        error("Too many identifier in one chunk.");
    }
}

static int identifierConstant(const Token* name, const bool isAssignment) {
    const Value string = OBJ_VAL(copyString(name->start, name->length));

    Value index;
    if (tableGet(&vm.globalNames, string, &index)) {
        return (int)AS_NUMBER(index);
    }

    if (!isAssignment) {
        errorAt(name, "Use of undeclared variable.");
    }

    const int newIndex = vm.globalValues.count;
    writeValueArray(&vm.globalValues, UNDEFINED_VAL);
    tableSet(&vm.globalNames, string, NUMBER_VAL((double)newIndex));
    return newIndex;
}

static bool identifiersEqual(const Token* a, const Token* b) {
    if (a->length != b->length) return false;
    return memcmp(a->start, b->start, a->length) == 0;
}

static int resolveLocal(const Compiler* compiler, const Token* name) {
    for (int i = compiler->localCount - 1; i >= 0; i--) {
        const Local* local = &compiler->locals[i];
        if (identifiersEqual(name, &local->name)) {
            if (local->depth == -1) {
                error("Can't read local variable in iwtss own initializer.");
            }
            return i;
        }
    }
    return -1;
}

static void addLocal(const Token name) {
    if (current->localCount == UINT8_COUNT) {
        error("Too many local variables in function.");
    }

    Local* local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = -1;
}

static void declareVariable() {
    if (current->scopeDepth == 0) return;

    const Token* name = &parser.previous;
    for (int i = current->localCount - 1; i >= 0; i--) {
        const Local* local = &current->locals[i];
        if (local->depth != -1 && local->depth < current->scopeDepth) {
            break;
        }

        if (identifiersEqual(name, &local->name)) {
            error("Already a variable with this name in this scope.");
        }
    }
    addLocal(*name);
}

static int parseVariable(const char* errorMessage) {
    consume(TOKEN_IDENTIFIER, errorMessage);

    declareVariable();
    if (current->scopeDepth > 0) return 0;

    return identifierConstant(&parser.previous, true);
}

static void makeInitialized() {
    current->locals[current->localCount - 1].depth = current->scopeDepth;
}

static void defineVariable(const int index, const int line) {
    if (current->scopeDepth > 0) {
        makeInitialized();
        return;
    }
    emitIndex(OP_DEFINE_GLOBAL, index, line);
}

static void binary(bool _) {
    const TokenType operatorType = parser.previous.type;
    const ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_BANG_EQUAL: emitBytes(OP_EQUAL, OP_NOT); break;
        case TOKEN_EQUAL_EQUAL: emitByte(OP_EQUAL); break;
        case TOKEN_GREATER: emitByte(OP_GREATER); break;
        case TOKEN_GREATER_EQUAL: emitBytes(OP_LESS, OP_NOT); break;
        case TOKEN_LESS: emitByte(OP_LESS); break;
        case TOKEN_LESS_EQUAL: emitBytes(OP_GREATER, OP_NOT); break;
        case TOKEN_PLUS:  emitByte(OP_ADD); break;
        case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:  emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
        default: return; // Unreachable
    }
}

static void literal(bool _) {
    switch (parser.previous.type) {
        case TOKEN_FALSE: emitByte(OP_FALSE); break;
        case TOKEN_TRUE: emitByte(OP_TRUE); break;
        case TOKEN_NIL: emitByte(OP_NIL); break;
        default: break; // Unreachable
    }
}

static void grouping(bool _) {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(bool _) {
    const double value = strtod(parser.previous.start, NULL);
    if (value == -1) {
        emitByte(OP_CONSTANT_M1);
    } else if (value == 0) {
        emitByte(OP_CONSTANT_0);
    } else if (value == 1) {
        emitByte(OP_CONSTANT_1);
    } else if (value == 2) {
        emitByte(OP_CONSTANT_2);
    } else {
        emitConstant(NUMBER_VAL(value));
    }
}

static void string(bool _) {
    emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2)));
}

static void postIncrementVariable(const int index, const bool isLocal, const bool decrement, const int line) {
    if (isLocal) {
        emitIndex(decrement ? OP_DEC_LOCAL : OP_INC_LOCAL, index, line);
        emitByte(1);
    }  else {
        emitByte(OP_DUP);
        emitBytes(OP_CONSTANT_1, decrement ? OP_SUBTRACT : OP_ADD);
        emitIndex(OP_SET_GLOBAL, index, line);
    }
    emitByte(OP_POP);
}

static void namedVariable(const Token name, const bool canAssign) {
#define SELF_ASSIGN(op) \
    do { \
        advance(); \
        expression(); \
        emitIndex(getOp, arg, name.line); \
        emitByte(op); \
        emitIndex(setOp, arg, name.line); \
    } while(false);

    uint8_t getOp, setOp;
    int arg = resolveLocal(current, &name);
    if (arg != -1) {
        getOp = OP_GET_LOCAL;
        setOp = OP_SET_LOCAL;
    } else {
        arg = identifierConstant(&name, false);
        getOp = OP_GET_GLOBAL;
        setOp = OP_SET_GLOBAL;
    }

    if (canAssign) {
        switch (parser.current.type) {
            case TOKEN_EQUAL: {
                advance();
                expression();
                emitIndex(setOp, arg, name.line);
                return;
            }
            case TOKEN_PLUS_EQUAL: {
                SELF_ASSIGN(OP_ADD)
                return;
            }
            case TOKEN_MINUS_EQUAL: {
                SELF_ASSIGN(OP_SUBTRACT)
                return;
            }
            case TOKEN_STAR_EQUAL: {
                SELF_ASSIGN(OP_MULTIPLY)
                return;
            }
            case TOKEN_SLASH_EQUAL: {
                SELF_ASSIGN(OP_SUBTRACT)
                return;
            }
            default: break;
        }
    }

    emitIndex(getOp, arg, name.line);

    if (match(TOKEN_PLUS_PLUS) || match(TOKEN_MINUS_MINUS)) {
        const bool isLocal = getOp == OP_GET_LOCAL;
        const bool decrement = parser.previous.type == TOKEN_MINUS_MINUS;
        postIncrementVariable(arg, isLocal, decrement, name.line);
    }
#undef SELF_ASSIGN
}

static void variable(const bool canAssign) {
    namedVariable(parser.previous, canAssign);
}

static void preIncrementVariable(const bool _) {
    int8_t opLocal, opGlobal;
    if (parser.previous.type == TOKEN_MINUS_MINUS) {
        opLocal = OP_DEC_LOCAL;
        opGlobal = OP_SUBTRACT;
    } else {
        opLocal = OP_INC_LOCAL;
        opGlobal = OP_ADD;
    }

    consume(TOKEN_IDENTIFIER, "Expected variable name.");
    const Token name = parser.previous;
    int arg = resolveLocal(current, &name);

    if (arg != -1) {
        emitIndex(opLocal, arg, name.line);
        emitByte(1);
    } else {
        arg = identifierConstant(&name, false);
        emitIndex(OP_GET_GLOBAL, arg, name.line);
        emitBytes(OP_CONSTANT_1, opGlobal);
        emitIndex(OP_SET_GLOBAL, arg, name.line);
    }
}

static void interpolation(bool _) {
    do {
        string(false);
        expression();
        emitByte(OP_ADD); // TODO: maybe later with a better stdlib we can do this in a better way and with better performance. Goal, it is faster then concatenate  and automatically converts to string
    } while (match(TOKEN_INTERPOLATION));

    consume(TOKEN_STRING, "Expect end of string interpolation.");
    string(false);
    emitByte(OP_ADD);
}

static void unary(bool _) {
    const TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case TOKEN_BANG: emitByte(OP_NOT); break;
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default: return; // Unreachable
    }
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]  =   {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
    [TOKEN_MINUS_MINUS]   = {preIncrementVariable,NULL,PREC_UNARY},
    [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
    [TOKEN_PLUS_PLUS]     = {preIncrementVariable,NULL,PREC_UNARY},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
    [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_LESS]          = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_IDENTIFIER]    = {variable,  NULL,  PREC_NONE},
    [TOKEN_STRING]        = {string,   NULL,   PREC_NONE},
    [TOKEN_INTERPOLATION] = {interpolation,NULL,PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NIL]           = {literal,  NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static void parsePrecedence(const Precedence precedence){
    advance();
    const ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL) {
        error("Expect expression.");
        return;
    }

    const bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(canAssign);

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        const ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(canAssign);
    }

    if (canAssign && match(TOKEN_EQUAL)) {
        error("Invalid assignment target.");
    }
}

static ParseRule* getRule(const TokenType type) {
    return &rules[type];
}

static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

static void block() {
    while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
        declaration();
    }

    consume(TOKEN_RIGHT_BRACE, "Expected '}' after block.");
}

static void varDeclaration() {
    const int index = parseVariable("Expect variable name.");
    const int line = parser.previous.line;

    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emitByte(OP_NIL);
    }
    consume(TOKEN_SEMICOLON, "Expected ';' after variable declaration.");
    defineVariable(index, line);
}

static void expressionStatement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expected ';' after expression.");
    emitByte(OP_POP);
}

static void printStatement() {
    expression();
    consume(TOKEN_SEMICOLON, "Expected ';' after value.");
    emitByte(OP_PRINT);
}

static void synchronize() {
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON) return;
        switch (parser.current.type) {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;
            default:
                break;
        }

        advance();
    }
}

static void statement() {
    if (match(TOKEN_PRINT)) {
        printStatement();
    } else if (match(TOKEN_LEFT_BRACE)) {
        beginScope();
        block();
        endScope();
    } else {
        expressionStatement();
    }
}

static void declaration() {
    if (match(TOKEN_VAR)) {
        varDeclaration();
    } else {
        statement();
    }

    if (parser.panicMode) synchronize();
}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();

    while (!match(TOKEN_EOF)) {
        declaration();
    }

    endCompiler();
    return !parser.hadError;
}
