#include "compiler.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

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

typedef void (*ParseFn)();

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

Parser parser;
Chunk* compilingChunk;

static Chunk* currentChunk() {
    return compilingChunk;
}

static void errorAt(const Token* token, const char* message) {
    if (parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] 2Error", token->line);
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

static void endCompiler() {
    emitReturn();

#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) {
        disassembleChunk(currentChunk(), "code");
    }
#endif //DEBUG_PRINT_CODE
}

static void expression();
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

static void binary() {
    const TokenType operatorType = parser.previous.type;
    const ParseRule* rule = getRule(operatorType);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_PLUS:  emitByte(OP_ADD); break;
        case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR:  emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
        default: return; // Unreachable
    }
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number() {
    const double value = strtod(parser.previous.start, nullptr);
    emitConstant(NUMBER_VAL(value));
}

static void unary() {
    const TokenType operatorType = parser.previous.type;

    parsePrecedence(PREC_UNARY);

    switch (operatorType) {
        case TOKEN_MINUS: emitByte(OP_NEGATE); break;
        default: return; // Unreachable
    }
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN]  =   {grouping,    nullptr,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_COMMA]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_DOT]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,       binary,    PREC_TERM},
    [TOKEN_PLUS]          = {nullptr,     binary,    PREC_TERM},
    [TOKEN_SEMICOLON]     = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_SLASH]         = {nullptr,     binary,    PREC_FACTOR},
    [TOKEN_STAR]          = {nullptr,     binary,    PREC_FACTOR},
    [TOKEN_BANG]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_EQUAL]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_GREATER]       = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_LESS]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_LESS_EQUAL]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_IDENTIFIER]    = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_STRING]        = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,      nullptr,   PREC_NONE},
    [TOKEN_AND]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_CLASS]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_ELSE]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_FALSE]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_FOR]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_FUN]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_IF]            = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_NIL]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_OR]            = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_PRINT]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_RETURN]        = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_SUPER]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_THIS]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_TRUE]          = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_VAR]           = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_WHILE]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_ERROR]         = {nullptr,     nullptr,   PREC_NONE},
    [TOKEN_EOF]           = {nullptr,     nullptr,   PREC_NONE},
};

static void parsePrecedence(const Precedence precedence){
    advance();
    const ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.current.type)->precedence) {
        advance();
        const ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule();
    }
}

static ParseRule* getRule(const TokenType type) {
    return &rules[type];
}

static void expression() {
    parsePrecedence(PREC_ASSIGNMENT);
}

bool compile(const char* source, Chunk* chunk) {
    initScanner(source);
    compilingChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expected end of expression.");
    endCompiler();
    return !parser.hadError;
}
