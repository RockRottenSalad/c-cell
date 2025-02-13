#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include "error.h"
#include "hashmap_test.h"
#include "error_test.h"
#include "expression_test.h"

void run_tests() {
    expression_test();
    hashmap_test();
    error_test();
}

#endif /* TEST_RUNNER_H */
