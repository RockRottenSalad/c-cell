#ifndef ERROR_H
#define ERROR_H

#include "collections.h"
#include "result.h"

typedef struct error_log error_log;
struct error_log {
    const char *message;
    enum error_code err_code;
};

struct error_state {
    queue(error_log) error_logs;
};

// NOTE: This is actually used. 
// The compiler is just retarded and won't stop complaining.
// Then I comment the line out and guess what? Undefined variable errors! bloody hell
static struct error_state __attribute__((__unused__)) err_state = {0};

void init_error_logging();

void cleanup_error_logging();

bool has_had_error();

void report_error(enum error_code err_code, const char *message);

struct error_log get_error(); 

struct error_log peek_error();

#endif /* ERROR_H */
