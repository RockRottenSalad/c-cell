
#include "error.h"

void init_error_logging() {
    err_state.error_logs = new_queue(struct error_log);
}

void cleanup_error_logging() {
    delete_queue(err_state.error_logs);
}

bool has_had_error() {
    return err_state.error_logs->len > 0;
}

void report_error(enum error_code err_code, const char *message) {
    if(err_state.error_logs == NULL) {
        PANIC("init_error_logging() must be called before report_error() can be used");
    }

    struct error_log err_log = (struct error_log){.message = message, .err_code = err_code};
    queue_add(err_state.error_logs, &err_log);
}

struct error_log get_error() {
    if(!has_had_error()) {
        PANIC("Called get_error() with no errors logged");
    }
    struct error_log e = *(struct error_log*)queue_poll(err_state.error_logs);
    queue_pop(err_state.error_logs);
    return e;
}
struct error_log peek_error() {
    if(!has_had_error()) {
        PANIC("Called peek_error() with no errors logged");
    }
    return *(struct error_log*)queue_poll(err_state.error_logs);
}


