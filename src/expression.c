
#include "expression.h"
#include "global_types.h"
#include "result.h"
#include "token.h"
#include "error.h"

#include<stdlib.h>
#include<ctype.h>

expression* new_binary_expression(expression *left, enum operator binary_operator, expression *right) {
    expression *expr = malloc(sizeof(struct expression));
    expr->left = left;
    expr->right = right;
    expr->binary_operator = binary_operator;
    expr->kind = BINARY_EXPRESSION;

    return expr;
}

expression* new_unary_expression(enum operator unary_operator, expression *operand) {
    expression *expr = malloc(sizeof(struct expression));
    expr->operand = operand;
    expr->unary_operator = unary_operator;
    expr->kind = UNARY_EXPRESSION;

    return expr;
}

expression *new_quantity_expression(expression *quantity_expr) {
    expression *expr = malloc(sizeof(struct expression));
    expr->quantity_expr = quantity_expr;
    expr->kind = QUANTITY_EXPRESSION;

    return expr;
}

expression* new_literal_expression(double number) {
    expression *expr = malloc(sizeof(struct expression));
    expr->number = number;
    expr->kind = LITERAL;

    return expr;
}

expression* new_reference_expression(char *id) {
    expression *expr = malloc(sizeof(struct expression));
    expr->identifier = id;
    expr->kind = REFERENCE_EXPRESSION;

    return expr;
}

result(ivec2) expression_identifier_to_cell(char *id, size_t max_columns, size_t max_rows) {
    ivec2 r = {0};
    size_t len = strlen(id);

    if(len < 2 || !isalpha(id[0])) {
        RESULT_ERROR_RETURN(ivec2, "Invalid identifier", SYNTAX_ERROR);
    }

    for(size_t i = 1; i < len; i++) {
        if(!isdigit(id[i])) {
            RESULT_ERROR_RETURN(ivec2, "Invalid identifier", SYNTAX_ERROR);
        }
    }

    // TODO
    // We know that id[0] is an alpha, but this is still sketchy.
    // please fix
    r.x = toupper(id[0]) - 65;
    // TODO
    // r.y is 32 bit, can overflow
    r.y = strtol(&(id[1]), NULL, 10);

    // TODO: Don't downcast size_t to int
    if(r.x >= (int)max_columns || r.y >= (int)max_rows) {
        RESULT_ERROR_RETURN(ivec2, "Identifier refers to a cell that is out of bounds", SEMANTIC_ERROR);
    }

    RESULT_RETURN(ivec2, r);
}

static inline
double evaluate_binary(expression *left, enum operator binary_operator, expression *right, struct spreadsheet *sheet) {
    switch(binary_operator) {
        case ADDITION:
            return evaluate(left, sheet) + evaluate(right, sheet);
        case SUBTRACTION:
            return evaluate(left, sheet) - evaluate(right, sheet);
        case MULTIPLICATION:
            return evaluate(left, sheet) * evaluate(right, sheet);
        case DIVISION:
            double tmp = evaluate(right, sheet);
            if(tmp == 0.0) {
                report_error(ARITHMETIC_ERROR, "Division by zero");
                return 1.0;
            }
            return evaluate(left, sheet) / tmp;
        default:
            report_error(ARITHMETIC_ERROR, "Invalid operator for binary operation");
            // Specifially returning 1.0 and not 0.0 to prevent further 
            // errors such as division by zero
            return 1.0;
    }
}

static inline
double evaluate_unary(enum operator unary_operator, expression *operand, struct spreadsheet *sheet) {
    switch(unary_operator) {
        case NEGATION:
            return -evaluate(operand, sheet);
        default:
            report_error(ARITHMETIC_ERROR, "Invalid operator for unary operation");
            return 1.0;
    }
}

double evaluate(expression *expr, struct spreadsheet *sheet) {
    switch(expr->kind) {
        case BINARY_EXPRESSION:
            return evaluate_binary(expr->left, expr->binary_operator, expr->right, sheet);
        case UNARY_EXPRESSION:
            return evaluate_unary(expr->unary_operator, expr->operand, sheet);
        case QUANTITY_EXPRESSION:
            return evaluate(expr->quantity_expr, sheet);
        case LITERAL:
            return expr->number;
        case REFERENCE_EXPRESSION:
            if(sheet == NULL) {
                report_error(NULL_PTR, "Attempted to evaluate reference expression and sheet pointer is NULL");
                return 1.0;
            }
            result(ivec2) cell_coordinate =
                expression_identifier_to_cell(expr->identifier, sheet->columns, sheet->rows);
            switch(cell_coordinate.status) {
                case OK:
                    if(sheet->sheet[cell_coordinate.result.x][cell_coordinate.result.y].kind == EXPRESSION)
                        return evaluate(sheet->sheet[cell_coordinate.result.x][cell_coordinate.result.y].expr, sheet);
                    else return 0.0;
                case ERROR:
                    report_error(cell_coordinate.err.err_code, cell_coordinate.err.message);
                    return 1.0;
            }
    }

    // Unreachable; all cases covered in switch.
    return 1.0; 
}

// TODO AND NOTE
// UNUSED, figure out what to do with this.
// Might be worth using?
void expression_determine_dependencies(expression *expr, list(ivec2) output) {
    switch(expr->kind) {
        case BINARY_EXPRESSION:
            expression_determine_dependencies(expr->left, output);
            expression_determine_dependencies(expr->right, output);
            break;
        case UNARY_EXPRESSION:
            expression_determine_dependencies(expr->operand, output);
            break;
        case QUANTITY_EXPRESSION:
            expression_determine_dependencies(expr->quantity_expr, output);
            break;
        case LITERAL:
            return;
        case REFERENCE_EXPRESSION:
            // Need to provide sheet to get spreadsheet dimensions 
            //expression_identifier_to_cell(expr->identifier, 0, 0);
            break;
    }
}

void delete_expression(expression *expr) {
    switch(expr->kind) {
        case BINARY_EXPRESSION:
            delete_expression(expr->left);
            delete_expression(expr->right);
            break;
        case UNARY_EXPRESSION:
            delete_expression(expr->operand);
            break;
        case QUANTITY_EXPRESSION:
            delete_expression(expr->quantity_expr);
            break;
        case REFERENCE_EXPRESSION:
            break;
        case LITERAL:
            break;
    }

    if(expr != NULL) free(expr);
    else {
        report_error(NULL_PTR, "Attempted to free NULL pointer in delete_expression()");
    }
}

char expression_binary_operator_to_char(enum operator binary_operator) {
    switch(binary_operator) {
        case ADDITION:
            return '+';
        case SUBTRACTION:
            return '-';
        case MULTIPLICATION:
            return '*';
        case DIVISION:
            return '/';
    }
    // Unreachable; all switch cases covered
    return '+';
}

void expression_ast_print(expression *expr, FILE *out) {
    switch(expr->kind) {
        case BINARY_EXPRESSION:
            fprintf(out, "(");
            expression_ast_print(expr->left, out);
            fprintf(out, "%c", expression_binary_operator_to_char(expr->binary_operator));
            expression_ast_print(expr->right, out);
            fprintf(out, "(");
            break;
        case UNARY_EXPRESSION:
            fprintf(out, "-");
            expression_ast_print(expr->operand, out);
            break;
        case QUANTITY_EXPRESSION:
            fprintf(out, "(");
            expression_ast_print(expr->quantity_expr, out);
            fprintf(out, ")");
            break;
        case REFERENCE_EXPRESSION:
            break;
        case LITERAL:
            printf("%lf", expr->number);
            break;
    }
}

/* RECURSIVE DESCENT PARSER IMPLEMENTATION STARTS HERE */

bool check(struct rd_parser_state *state, enum token_kind token) {
    if(state->index >= state->tokens->len) return false;
    return ((struct token*)state->tokens->list)[state->index].kind == token;
}

void advance(struct rd_parser_state *state) {
    if( ((token *)state->tokens->list)[state->index].kind == state->delim) return;
    state->index++;
}

token previous(struct rd_parser_state *state) {
    return ((token*)state->tokens->list)[state->index - 1];
}

bool match(struct rd_parser_state *state, size_t types) {
    for(size_t i = 0; i < TOKEN_BIT_FLAGS_COUNT; i++) {
        if( check(state, (1 << i) & types) ) {
            state->index++;
            return true;
        }
    }
    return false;
}

// Note this is equivalent to "expr" in the productions
expression* start_recurive_descent(struct rd_parser_state *state); 

expression* primary(struct rd_parser_state *state) {
    if(match(state, NUMBER)) {
        return new_literal_expression(previous(state).number);
    }else if(match(state, IDENTIFIER)) {
        return new_reference_expression(previous(state).string);
    } else if(match(state, OPEN_PAREN)) {
        expression *expr = start_recurive_descent(state);
        if(match(state, CLOSE_PAREN)) {
            return new_quantity_expression(expr); 
        }else {
            report_error(SYNTAX_ERROR, "Expected ')'");
            return expr;
        }
    }

    report_error(SYNTAX_ERROR, "Expected literal");
    return new_literal_expression(1.0);
}

expression* unary(struct rd_parser_state *state) {
    if(match(state, MINUS)) {
        enum operator operator = token_kind_to_operator(previous(state).kind);
        expression *right = unary(state);
        return new_unary_expression(operator, right);
    }

    return primary(state);
}

expression* factor(struct rd_parser_state *state) {
    expression *expr = unary(state); 

    while(match(state, SLASH | STAR)) {
        enum operator operator = token_kind_to_operator(previous(state).kind);
        expression *right = unary(state);
        expr = new_binary_expression(expr, operator, right);
    }

    return expr;
}

expression* term(struct rd_parser_state *state) {
    expression *expr = factor(state); 

    while(match(state, MINUS | PLUS)) {
        enum operator operator = token_kind_to_operator(previous(state).kind);
        expression *right = factor(state);
        expr = new_binary_expression(expr, operator, right);
    }

    return expr;
}

// Note this is equivalent to "expr" in the productions
expression* start_recurive_descent(struct rd_parser_state *state) {
    return term(state);
}

// REPORTS ERROR
result(expression_ptr) new_expression_from_tokens(list(token) tokens, size_t start_index, enum token_kind terminating_delim) {

    token *token_list = tokens->list;
    if(start_index >= tokens->len) {
        RESULT_ERROR_RETURN(expression_ptr, "start_index is out of bounds", INDEX_OUT_OF_BOUNDS);
    }else if(token_list[start_index].kind != EQUALS) {
        RESULT_ERROR_RETURN(expression_ptr, "Expected initial equals sign in expression", UNEXPECTED_TOKEN);
    }

    struct rd_parser_state state = {.tokens = tokens, .index = start_index + 1, .delim = terminating_delim};

    if(has_had_error()) {
        RESULT_ERROR_RETURN(expression_ptr, "Error occured during parsing of tokens", ERROR_IN_SUBSYSTEM);
    }

    RESULT_RETURN(expression_ptr, start_recurive_descent(&state));

    //    RESULT_ERROR_RETURN(expression_ptr, "Not implemented yet", NOT_IMPLEMENTED);
}

enum operator token_kind_to_operator(enum token_kind kind) {
    switch(kind) {
        case PLUS:
            return ADDITION;
        case MINUS:
            return SUBTRACTION;
        case STAR:
            return MULTIPLICATION;
        case SLASH:
            return DIVISION;
        default:
            // Bruh, this error is meant for the user
            report_error(SYNTAX_ERROR, "Invalid token kind passed to token_kind_to_operator()");

            return SUBTRACTION;
    }
}


