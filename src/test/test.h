#ifndef TEST_H
#define TEST_H

#include "../utils.h"
#include <stdbool.h>

#define TEST_LOG(...)\
    fprintf(stderr, FG_BRIGHT_WHITE "|TEST: ["__FILE__":%d]| " RESET_COLOR, __LINE__ );\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n")

#define ASSERT(EQ, ...)\
if(!(EQ)) {\
TEST_LOG(__VA_ARGS__);\
return false;\
}

#define RUN_TEST(TEST, NAME)\
if(!TEST) {\
TEST_LOG("Test: " FG_BRIGHT_WHITE NAME FG_BRIGHT_RED " FAILED" RESET_COLOR);\
}else{\
TEST_LOG("Test: " FG_BRIGHT_WHITE NAME FG_BRIGHT_GREEN " PASSED" RESET_COLOR);\
}

#endif
