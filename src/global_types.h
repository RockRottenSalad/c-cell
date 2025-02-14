#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include "result.h"
#include "collections.h"

struct expression;

enum cell_kind : char {
    EMPTY = 0,
    EXPRESSION,
    TEXT
};

struct cell {
    union {
        struct expression *expr;
        string_view text;
    };
    enum cell_kind kind;
};

typedef struct spreadsheet spreadsheet;
struct spreadsheet {
    struct cell **sheet;
    size_t rows, columns;
};

enum expression_kind : char {
    UNARY_EXPRESSION,
    BINARY_EXPRESSION,
    QUANTITY_EXPRESSION,
    REFERENCE_EXPRESSION,
    FUNCTION_EXPRESSION,
    LITERAL
};

enum operator : char {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    NEGATION = SUBTRACTION
};

enum function_ptr_return : char {
    NUM, // NUMBER already taken
    NUMS,
    ID, // IDENTIFIER already taken
    IDS,
    EXPR, // EXPRESSION already taken
    EXPRS,
    INVALID
};

// Anything is used by function expressions for return types and arguments
typedef struct anything anything;
typedef anything* anything_ptr;
struct anything {
    union {
        double number;
        list(double) numbers;
        char *id;
        list(char_ptr) ids;
    };
    enum function_ptr_return kind;
};

typedef anything(*expression_function)(list(expression*), spreadsheet*);

typedef struct expression expression;
typedef expression* expression_ptr;
struct expression {
    union {
        struct { // Unary expr
            expression *operand;
            enum operator unary_operator;
        };
        struct { // Binary expr
            expression *left;
            expression *right;
            enum operator binary_operator;
        };
        struct { // Function expr
            expression_function function;
            list(anything) arguments;
        };
        expression *quantity_expr; // Quantity expr
        double number; // Literal
        char *identifier; // Reference expr
    };
    enum expression_kind kind;
};

DEFINE_RESULT_TYPE(expression_ptr);
DEFINE_RESULT_TYPE(expression);
DEFINE_RESULT_TYPE(anything_ptr);
DEFINE_RESULT_TYPE(anything);
DEFINE_RESULT_TYPE(expression_function);

#endif /* GLOBAL_TYPES_H */
