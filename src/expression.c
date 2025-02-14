
#include "expression.h"
#include "global_types.h"
#include "result.h"
#include "token.h"
#include "error.h"

#include<stdlib.h>
#include<ctype.h>
#include<float.h>

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

expression* new_function_expression(expression_function function, list(expression_ptr) arguments) {
    expression *expr = malloc(sizeof(struct expression));
    expr->function = function;
    expr->arguments = arguments;
    expr->kind = FUNCTION_EXPRESSION;

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
    r.y = (int)strtol(&(id[1]), NULL, 10);


    // TODO: Don't downcast size_t to int
    if(r.x >= (int)max_columns || r.y >= (int)max_rows) {
        RESULT_ERROR_RETURN(ivec2, "Identifier refers to a cell that is out of bounds", SEMANTIC_ERROR);
    }

    //LOG("Parsed following reff: col: %d, row: %d from id: %s", r.x, r.y, id);

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
        case FUNCTION_EXPRESSION:
            anything result = expr->function(expr->arguments, sheet);
            // TODO temp solution for testing
            if(result.kind != NUM) {
                PANIC("Testing");
            }
            return result.number;
        case REFERENCE_EXPRESSION:
            if(sheet == NULL) {
                report_error(NULL_PTR, "Attempted to evaluate reference expression and sheet pointer is NULL");
                return 1.0;
            }
            result(ivec2) cell_coordinate =
                expression_identifier_to_cell(expr->identifier, sheet->columns, sheet->rows);
            switch(cell_coordinate.status) {
                case OK:
                    if(sheet->sheet[cell_coordinate.result.y][cell_coordinate.result.x].kind == EXPRESSION) {
                        return evaluate(sheet->sheet[cell_coordinate.result.y][cell_coordinate.result.x].expr, sheet);
                    }else {
                        return 0.0;
                    }
                case ERROR:
                    report_error(cell_coordinate.err.err_code, cell_coordinate.err.message);
                    return 1.0;
            }
    }

    // Unreachable; all cases covered in switch.
    return 1.0; 
}

// TODO AND NOTE
// Might be worth using?
// The spreadsheet simplifies long expressions into single literals to simplify further calls to evaluate() on the same expression.
// But with reference expressions some expressions might be evaluated multiple times.
// If expression references are determined beforehand, a dependency graph can be constructed 
// and toplogical sort can be uesd to eliminate needless expression evaluations. 
void expression_determine_dependencies(expression *expr, list(ivec2) output, spreadsheet *spreadsheet_ptr) {
    switch(expr->kind) {
        case BINARY_EXPRESSION:
            expression_determine_dependencies(expr->left, output, spreadsheet_ptr);
            expression_determine_dependencies(expr->right, output, spreadsheet_ptr);
            break;
        case UNARY_EXPRESSION:
            expression_determine_dependencies(expr->operand, output, spreadsheet_ptr);
            break;
        case QUANTITY_EXPRESSION:
            expression_determine_dependencies(expr->quantity_expr, output, spreadsheet_ptr);
            break;
        case FUNCTION_EXPRESSION:
            return;
        case LITERAL:
            return;
        case REFERENCE_EXPRESSION:
            result(ivec2) r = expression_identifier_to_cell(expr->identifier, spreadsheet_ptr->rows, spreadsheet_ptr->columns);
            if(r.status == OK) {
                list_push(output, &r.result);
            }
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
        case FUNCTION_EXPRESSION:
            for(size_t i = 0; i < expr->arguments->len; i++) {
                delete_expression(((expression**)expr->arguments->list)[i]);
            }
            delete_list(expr->arguments);
            break;
        case REFERENCE_EXPRESSION:
            // TODO at some point we gotta switch to string views
            // The entire csv file is loaded into the buffered reader anyways,
            // might as well take a snippet of that
            break;
        case LITERAL:
            break;
    }

    if(expr != NULL) free(expr);
    else report_error(NULL_PTR, "Attempted to free NULL pointer in delete_expression()");
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
        case FUNCTION_EXPRESSION:
            // TODO add a way of determining the function at hand
            // Maybe add an enum?
            // Could also compare function pointers but that sounds annoying
            fprintf(out, "f(");
            for(size_t i = 0; i < expr->arguments->len; i++) {
                expression_ast_print(((expression**)expr->arguments->list)[i], out);
                fprintf(out, " ");
            }
            fprintf(out, ")");
            break;
        case REFERENCE_EXPRESSION:
            printf("%s", expr->identifier);
            break;
        case LITERAL:
            printf("%lf", expr->number);
            break;
    }
}

/* EXPRESSION FUNCTIONS DEFINED HERE */

anything _min(list(expression_ptr) arguments, spreadsheet *spreadsheet_ptr) {
    if(arguments->len == 0) {
        report_error(SYNTAX_ERROR, "MIN() expects at least 1 argument");
        return ((anything){.kind = INVALID});
    }

    expression **args = (expression**)arguments->list;
    double min = DBL_MAX, tmp;
    for(size_t i = 0; i < arguments->len; i++) {
        tmp = evaluate(args[i], spreadsheet_ptr);
        if(tmp < min) min = tmp;
    }

    return ((anything){.kind = NUM, .number = min});
}

anything _sum(list(expression_ptr) arguments, spreadsheet *spreadsheet_ptr) {
    if(arguments->len == 0) {
        report_error(SYNTAX_ERROR, "SUM() expects at least 1 argument");
        return ((anything){.kind = INVALID});
    }

    expression **args = (expression**)arguments->list;

    double sum = 0.0;
    for(size_t i = 0; i < arguments->len; i++) {
        sum += evaluate(args[i], spreadsheet_ptr);
    }

    if(has_had_error()) {
        return ((anything){.kind = INVALID});
    }

    return ((anything){.kind = NUM, .number = sum});
}

// TODO
// Use a map when this gets big enough
result(expression_function) expression_str_to_function(char *id) {
    size_t len = strlen(id);
    if(len == 3) {
        if(strncmp(id, "MIN", 3) == 0) {
            RESULT_RETURN(expression_function, _min);
        }else if(strncmp(id, "SUM", 3) == 0) {

            RESULT_RETURN(expression_function, _sum);
        } 
    }
    RESULT_ERROR_RETURN(expression_function, "Function does not exist", SEMANTIC_ERROR);
}

/* RECURSIVE DESCENT PARSER IMPLEMENTATION STARTS HERE */

bool check(struct rd_parser_state *state, enum token_kind token) {
    if(state->index >= state->tokens->len) return false;
    return ((struct token*)state->tokens->list)[state->index].kind == token;
}

void advance(struct rd_parser_state *state) {
    if(((token *)state->tokens->list)[state->index].kind == state->delim) return;
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

bool finished(struct rd_parser_state *state) {
    return state->index >= state->tokens->len;
}

// Note this is equivalent to "expr" in the productions
expression* start_recurive_descent(struct rd_parser_state *state); 

expression* primary(struct rd_parser_state *state) {
    if(match(state, NUMBER)) {
        return new_literal_expression(previous(state).number);
    }else if(match(state, IDENTIFIER)) {
        char *id = previous(state).string;

        // TODO
        // Function parsing is still WIP
        if(match(state, OPEN_PAREN)) { // Function
            dynamic_list *arguments = new_list(expression_ptr);

            while(!match(state, CLOSE_PAREN) && !finished(state)) {
                list_push_rv(arguments, start_recurive_descent(state));
                if(has_had_error()) {
                    break;
                }
            }

            if(previous(state).kind != CLOSE_PAREN || has_had_error()) {
                for(size_t i = 0; i < arguments->len; i++) {
                    delete_expression(((expression**)arguments->list)[i]);
                }
                delete_list(arguments);
            }else {
                result(expression_function) r = expression_str_to_function(id);
                if(r.status == OK) {
                    return new_function_expression(r.result, arguments);
                }else {
                    report_error(SEMANTIC_ERROR, r.err.message);
                }
            }
            // Default error return in case
            return new_literal_expression(1.0);
        }else { // Reference to cell
            return new_reference_expression(id);
        }
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

