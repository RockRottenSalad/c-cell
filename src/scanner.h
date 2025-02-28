#ifndef SCANNER_H
#define SCANNER_H

#include "buffered_reader.h"
#include "collections.h"

typedef struct scanner scanner;
typedef scanner* scanner_ptr;
DEFINE_RESULT_TYPE(scanner_ptr);

struct scanner {
    buffered_reader *br;
    list(token) tokens;
    bool has_scanned;
};

result(scanner_ptr) new_scanner(buffered_reader *br);
result(bool) delete_scanner(scanner *sc);
result(bool) scanner_start_scan(scanner *sc);

result(token) _scanner_identifier_token(scanner *sc, list(char) string_builder, char first_ch);
result(token) _scanner_number_token(scanner *sc, list(char) number_builder, char first_ch); 

#endif /* SCANNER_H */
