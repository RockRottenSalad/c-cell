#ifndef TOKEN_H
#define TOKEN_H

#include<stdlib.h>

#include "result.h"
#include "collections.h"

// NOTE for the love of god; don't forgot to update this
#define TOKEN_BIT_FLAGS_COUNT 10

enum token_kind : size_t {
    // These are used as bit flags in the recursive descent parser(expression.c)
    
    IDENTIFIER  = 1,
    COLON       = 2,
    PLUS        = 4,
    MINUS       = 8,
    EQUALS      = 16,
    SLASH       = 32,
    STAR        = 64,
    NUMBER      = 128,
    OPEN_PAREN  = 256,
    CLOSE_PAREN = 512,

    COMMA,
    PERIOD,

    NEWLINE,
    STRING,
    STRING_VIEW

};

typedef struct token token;
struct token {
    union {
        char character;
        char *string;
        double number;
        string_view sv;
    };
    size_t line, column, index;
    enum token_kind kind;
};

DEFINE_RESULT_TYPE(token);

static inline 
struct token token_new_ch(enum token_kind kind, size_t line, size_t column, size_t index, char ch) {
    return (struct token){.kind = kind, .line = line, .column = column, .index = index, .character = ch};
}
static inline
struct token token_new_str(size_t line, size_t column, size_t index, char *str) {
    return (struct token){.kind = STRING, .line = line, .column = column, .index = index, .string = str};
}
static inline
struct token token_new_id(size_t line, size_t column, size_t index, char *str) {
    return (struct token){.kind = IDENTIFIER, .line = line, .column = column, .index = index, .string = str};
}
static inline
struct token token_new_num(size_t line, size_t column, size_t index, double num) {
    return (struct token){.kind = NUMBER, .line = line, .column = column, .index = index, .number = num};
}
static inline
struct token token_new_sv(size_t line, size_t column, size_t index, string_view sv) {
    return (struct token){.kind = STRING_VIEW, .line = line, .column = column, .index = index, .sv = sv};
}

#endif /* TOKEN_H */
