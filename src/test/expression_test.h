#ifndef EXPRESSION_TEST_H
#define EXPRESSION_TEST_H

#include "../collections.h"
#include "../expression.h"
#include "../utils.h"
#include "error.h"
#include "global_types.h"
#include "test.h"
#include "token.h"
#include <stdio.h>
#include <math.h>

bool expression_test_1() {
    list(token) tokens = new_list(token);


    list_push_rv(tokens, token_new_ch(EQUALS, 0, 0, 0, '='));
    list_push_rv(tokens, token_new_ch(OPEN_PAREN, 0, 0, 0, '('));
    list_push_rv(tokens, token_new_num(0, 0, 0, 12.0));
    list_push_rv(tokens, token_new_ch(PLUS, 0, 0, 0, '+'));
    list_push_rv(tokens, token_new_num(0, 0, 0, 6.0));
    list_push_rv(tokens, token_new_ch(CLOSE_PAREN, 0, 0, 0, ')'));
    list_push_rv(tokens, token_new_ch(STAR, 0, 0, 0, '*'));
    list_push_rv(tokens, token_new_num(0, 0, 0, 4.0));
    list_push_rv(tokens, token_new_ch(SLASH, 0, 0, 0, '/'));
    list_push_rv(tokens, token_new_num(0, 0, 0, 2.0));
    list_push_rv(tokens, token_new_ch(MINUS, 0, 0, 0, '-'));
    list_push_rv(tokens, token_new_ch(OPEN_PAREN, 0, 0, 0, '('));
    list_push_rv(tokens, token_new_num(0, 0, 0, 3.0));
    list_push_rv(tokens, token_new_ch(STAR, 0, 0, 0, '*'));
    list_push_rv(tokens, token_new_num(0, 0, 0, 9.0));
    list_push_rv(tokens, token_new_ch(CLOSE_PAREN, 0, 0, 0, ')'));
    list_push_rv(tokens, token_new_ch(COMMA, 0, 0, 0, ','));

    result(expression_ptr) expr_result = new_expression_from_tokens(tokens, 0, COMMA);
    ASSERT(expr_result.status == OK, "Parsing of expression from tokens failed: %s", expr_result.err.message);

    expression *expr = expr_result.result;

    double expect = 9.0;
    double result = floor(evaluate(expr, NULL));

    ASSERT(expect == result, "Expression evaluation is incorrect, expected %lf, got %lf", expect, result);

    delete_expression(expr);
    delete_list(tokens);
    return true;
}

bool expression_test_2() {
    list(token) tokens = new_list(token);


    list_push_rv(tokens, token_new_ch(EQUALS, 0, 0, 0, '='));
    list_push_rv(tokens, token_new_ch(MINUS, 0, 0, 0, '-'));
    list_push_rv(tokens, token_new_num(0, 0, 0, 12.0));
    list_push_rv(tokens, token_new_ch(MINUS, 0, 0, 0, '-'));
    list_push_rv(tokens, token_new_num(0, 0, 0, 12.0));

    result(expression_ptr) expr_result = new_expression_from_tokens(tokens, 0, NEWLINE);
    ASSERT(expr_result.status == OK, "Parsing of expression from tokens failed");

    expression *expr = expr_result.result;

    double expect = -24.0;
    double result = floor(evaluate(expr, NULL));

    ASSERT(expect == result, "Expression evaluation is incorrect, expected %lf, got %lf", expect, result);

    delete_expression(expr);
    delete_list(tokens);
    return true;
}



void expression_test(void) {

    init_error_logging();
    LOG("Starting expression test");

    RUN_TEST(expression_test_1(), "Expression parse and evaluation test of '(12.0 + 6.0) * 4.0 / 2.0 - (3.0 * 9.0)'" );
    RUN_TEST(expression_test_2(), "Expression parse and evaluation test of '-12 - 12'" );

    cleanup_error_logging();
}


#endif /* EXPRESSION_TEST_H */
