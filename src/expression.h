#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "global_types.h"
#include "token.h"
#include "c_cell_math.h"
#include "string.h"

result(ivec2) expression_identifier_to_cell(char *id, size_t max_columns, size_t max_rows);

expression* new_binary_expression(expression *left, enum operator binary_operator, expression *right);
expression* new_unary_expression(enum operator unary_operator, expression *operand);
expression* new_quantity_expression(expression *quantity_expr);
expression* new_literal_expression(double number);
expression* new_reference_expression(char *id);
expression* new_function_expression(expression_function function, list(expression_ptr) arguments);

double evaluate(expression *expr, struct spreadsheet *sheet);
void expression_determine_dependencies(expression *expr, list(ivec2) output, spreadsheet *spreadsheet_ptr);

void delete_expression(expression *expr);

char expression_binary_operator_to_char(enum operator binary_operator);
void expression_ast_print(expression *expr, FILE *out);

// rd = recursive descent
struct rd_parser_state {
    list(token) tokens;
    size_t index;
    enum token_kind delim;
};
result(expression_ptr) new_expression_from_tokens(list(token) tokens, size_t start_index, enum token_kind terminating_delim);

enum operator token_kind_to_operator(enum token_kind kind);


#endif /* EXPRESSION_H */
