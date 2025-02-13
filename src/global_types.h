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
    LITERAL
};

enum operator : char {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    NEGATION = SUBTRACTION
};

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
        expression *quantity_expr; // Quantity expr
        double number; // Literal
        char *identifier; // Reference expr
    };
    enum expression_kind kind;
};

DEFINE_RESULT_TYPE(expression_ptr);

#endif /* GLOBAL_TYPES_H */
