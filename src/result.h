#ifndef RESULT_H
#define RESULT_H

/* Preface
 * The result struct, inspired by the language Rust.
 *
 * The result struct can be used for error handling. It
 * is a tagged union which either can contain a value or
 * an error message and an error code to describe what went
 * wrong.
 */

#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>

#include "utils.h"

enum error_code {
    // General errors
    NULL_PTR = 0,
    ERROR_IN_SUBSYSTEM = 1,
    NOT_IMPLEMENTED = 2,

    // IO
    FILE_NOT_FOUND = 3,
    READ_OPERATION_FAILED = 4,
    REACHED_EOF = 5,

    // Buffered reader specific
    BUFFER_IS_EMPTY = 6,

    // Scanner/Token specific
    UNKNOWN_TOKEN = 7,
    UNEXPECTED_TOKEN = 8,

    INDEX_OUT_OF_BOUNDS = 9,
    ARITHMETIC_ERROR = 10,
    SYNTAX_ERROR,
    SEMANTIC_ERROR

};

enum result_status {
    OK,
    ERROR
};

struct error {
    const char *message;
    enum error_code err_code;
};

#define result(TYPE) struct result_##TYPE
#define DEFINE_RESULT_TYPE(TYPE) \
struct result_##TYPE {\
    enum result_status status;\
    union {\
        struct error err;\
        TYPE result;\
    };\
};

#define unwrap(VAR, RESULT)\
do{\
auto __r = RESULT;\
switch(__r.status) {\
    case ERROR:\
    PANIC("Unwrapped an error: %s", __r.err.message);\
    default:\
    break;\
  }\
VAR = __r.result;\
}while(0)

DEFINE_RESULT_TYPE(char);
DEFINE_RESULT_TYPE(short);
DEFINE_RESULT_TYPE(int);
DEFINE_RESULT_TYPE(long);
DEFINE_RESULT_TYPE(float);
DEFINE_RESULT_TYPE(double);
DEFINE_RESULT_TYPE(bool);

typedef char* char_ptr;
typedef short* short_ptr;
typedef int* int_ptr;
typedef long* long_ptr;
typedef float* float_ptr;
typedef double* double_ptr;
typedef bool* bool_ptr;
DEFINE_RESULT_TYPE(char_ptr);
DEFINE_RESULT_TYPE(short_ptr);
DEFINE_RESULT_TYPE(int_ptr);
DEFINE_RESULT_TYPE(long_ptr);
DEFINE_RESULT_TYPE(float_ptr);
DEFINE_RESULT_TYPE(double_ptr);
DEFINE_RESULT_TYPE(bool_ptr);


// Commonly used for returning an error when given null ptr as argument
#define NULL_ARGUMENT_ERROR_RETURN(TYPE, ARG){\
    if(ARG == NULL) {\
        result(TYPE) __r = \
            {.status = ERROR, .err = {"Got NULL as argument", NULL_PTR}};\
        return __r;}\
    }

// Return error with message
#define RESULT_ERROR_RETURN(TYPE, MSG, ERROR_CODE){\
    result(TYPE) __r =\
        {.status = ERROR, .err = {.message = "["__FILE__":LINE] "MSG, .err_code = ERROR_CODE}};\
    return __r;}

// Return actual result wrapped up in result struct
#define RESULT_RETURN(TYPE, RESULT){\
    result(TYPE) __r =\
        {.status = OK, .result = RESULT};\
    return __r;}

#endif /* RESULT_H */
