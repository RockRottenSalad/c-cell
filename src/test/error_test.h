#ifndef ERROR_TEST_H
#define ERROR_TEST_H

#include "result.h"
#include "test.h"
#include "../error.h"
#include<string.h>

bool empty_error_log_test() {
    ASSERT(!has_had_error(), "Expected error log to be empty");
    return true;
}

bool report_error_test() {

    report_error(INDEX_OUT_OF_BOUNDS, "This is a test");

    ASSERT(has_had_error(), "Expected error to be logged");
    
    error_log err_log = get_error();

    ASSERT(!has_had_error(), "Expected error to be gone after calling get_error()");

    ASSERT(err_log.err_code == INDEX_OUT_OF_BOUNDS, "Expected error code to be INDEX_OUT_OF_BOUNDS");
    ASSERT(strcmp(err_log.message, "This is a test") == 0, "Incorrect error message logged");

    return true;
}

bool report_multiple_errors_test() {

    report_error(NULL_PTR, "Null pointer test");
    report_error(UNEXPECTED_TOKEN, "Unexpected token test");
    report_error(ERROR_IN_SUBSYSTEM, "Subsystem test");

    ASSERT(has_had_error(), "Expected error to be logged");

    error_log a = {0};

    a = get_error();
    ASSERT(strcmp(a.message, "Null pointer test") == 0, "Incorrect error message");
    ASSERT(NULL_PTR == a.err_code, "Incorrect error code");

    a = get_error();
    ASSERT(strcmp(a.message, "Unexpected token test") == 0, "Incorrect error message");
    ASSERT(UNEXPECTED_TOKEN == a.err_code, "Incorrect error code");

    a = get_error();
    ASSERT(strcmp(a.message, "Subsystem test") == 0, "Incorrect error message");
    ASSERT(ERROR_IN_SUBSYSTEM == a.err_code, "Incorrect error code");

    return true;
}


void error_test() {
    init_error_logging();
    LOG("Starting error test");

    RUN_TEST(empty_error_log_test(), "Error log empty test");
    RUN_TEST(report_error_test(), "Error report test");
    RUN_TEST(report_multiple_errors_test(), "Multiple error report test");

    cleanup_error_logging();
}


#endif /* ERROR_TEST_H */
