
#include "collections.h"
#include "result.h"
#include "token.h"
#include "scanner.h"

#include "buffered_reader.h"
#include<ctype.h>
#include<string.h>

result(scanner_ptr) new_scanner(buffered_reader *br) {
    NULL_ARGUMENT_ERROR_RETURN(scanner_ptr, br);

    if(br->has_had_error) {
        RESULT_ERROR_RETURN( scanner_ptr,
            "Buffered reader has had an internal error, cannot construct scanner", 
            ERROR_IN_SUBSYSTEM)
    }

    scanner *scanner = (struct scanner*)malloc(sizeof(struct scanner));

    scanner->br = br;
    scanner->tokens = new_list(token);
    scanner->has_scanned = true;

    RESULT_RETURN(scanner_ptr, scanner);
}

// NOTE: buffered reader is freed by function that allocated the scanner
result(bool) delete_scanner(scanner *sc) {
    NULL_ARGUMENT_ERROR_RETURN(bool, sc);

    size_t len = sc->tokens->len;
    token *list = (token*)sc->tokens->list;

    for(size_t i = 0; i < len; i++) {
        switch(list[i].kind) {
            case STRING:
            case IDENTIFIER:
                free(list[i].string);
                break;
            default:
        }
    }

    delete_list(sc->tokens);
    free(sc);

    RESULT_RETURN(bool, true);
}

result(bool) _scanner_expand_range_macro(scanner *sc, list(char) string_builder, char *start, char *end) {
    // Error checks
    if(!isalpha(start[0]) || !isalpha(end[0])) {
        RESULT_ERROR_RETURN(bool, "Identifiers syntax is invalid", SYNTAX_ERROR);
    }else if(start[0] > end[0]) {
        RESULT_ERROR_RETURN(bool, "Backwards ranges not supported yet", NOT_IMPLEMENTED);
    }else {
        int len = strlen(start);
        for(int i = 1; i < len; i++) {
            if(!isdigit(start[i])) {
                RESULT_ERROR_RETURN(bool, "Identifiers syntax is invalid", SYNTAX_ERROR);
            }
        }
        len = strlen(end);
        for(int i = 1; i < len; i++) {
            if(!isdigit(end[i])) {
                RESULT_ERROR_RETURN(bool, "Identifiers syntax is invalid", SYNTAX_ERROR);
            }
        }
    }

    // NOTE: Might overflow
    const int start_row = strtol(&start[1], NULL, 10);
    const int end_row = strtol(&end[1], NULL, 10);
    printf("START ROW: %d, END ROW: %d\n", start_row, end_row);
    if(start_row > 10 || end_row > 10) {
        RESULT_ERROR_RETURN(bool, "Rows > 10 not supported yet", NOT_IMPLEMENTED);
    }

    const char start_column = start[0];
    const char end_column = end[0];

    // Caller's job to make sure this doesn't blow shit up
    free(((token*)sc->tokens->list)[sc->tokens->len-1].string);
    free(end);
    list_pop(sc->tokens);

    for(char c = start_column; c <= end_column; c++) {
        for(int r = start_row; r <= end_row; r++) {
            list_clear(string_builder);
            list_push_rv(string_builder, c);

            list_push_rv(string_builder, ((char)(r+48)));
            list_push_rv(string_builder, '\0');
            list_push_rv(sc->tokens,
                         token_new_id(sc->br->line, sc->br->column, sc->br->index, strdup(string_builder->list))
                         );
        }
    }


    RESULT_RETURN(bool, true);
}

/*
* Currently, cells that start with a digit are assumed to be an expression.
* Missing a check for cases where the cell has text after digit
*/
result(bool) _scanner_tokenize_expression(scanner *sc, list(char) string_builder) {
    list_push_rv(sc->tokens, token_new_ch(EQUALS, sc->br->line, sc->br->column, sc->br->index, '='));

    { 
        result(char) eq_check = bf_peek_char(sc->br);
        if(eq_check.status == OK && eq_check.result == '=') bf_get_char(sc->br);
    }

    bool ignore_next_comma = false;

    // TODO
    // The comma shit is absolutely hideous and it doesn't even fucking work 
    // Find a way to remove it without making everything else blow up please
    for(result(char) expr_ch_result = bf_get_char(sc->br);
    expr_ch_result.status == OK;
    expr_ch_result = bf_get_char(sc->br)) {
        if(expr_ch_result.result == ',') {
            if(!ignore_next_comma) {
                list_push_rv(sc->tokens, token_new_ch(COMMA, sc->br->line, sc->br->column, sc->br->index, ',')); 
                break;
            }else {
                ignore_next_comma = false;
            }
        }else if(expr_ch_result.result == '\n') {
            list_push_rv(sc->tokens, token_new_ch(NEWLINE, sc->br->line, sc->br->column, sc->br->index, '\n')); 
            break;
        }else {
            token t = (token){.character = expr_ch_result.result,
                .line = sc->br->line, .column = sc->br->column, .index = sc->br->index};

            switch(expr_ch_result.result) {
                case ':': // The one and only macro. I pinky promise
                    if(sc->tokens->len == 0 || ((token*)sc->tokens->list)[sc->tokens->len-1].kind != IDENTIFIER) {
                        RESULT_ERROR_RETURN(bool, "Expected identifier before ':'", SYNTAX_ERROR);
                    }

                    result(char) next = bf_get_char(sc->br);
                    if(next.status == ERROR || !isalpha(next.result)) {
                        RESULT_ERROR_RETURN(bool, "Expected identifier after ':'", SYNTAX_ERROR);
                    }

                    result(token) to = _scanner_identifier_token(sc, string_builder, next.result);
                    if(to.status == ERROR) {
                        RESULT_ERROR_RETURN(bool, "Expected identifier after ':'", SYNTAX_ERROR);
                    }
                    result(bool) m = _scanner_expand_range_macro(sc, 
                                                string_builder,
                                                ((token*)sc->tokens->list)[sc->tokens->len-1].string,
                                                to.result.string
                                                );
                    if(m.status == ERROR) {
                        PANIC("ERROR IN MACRO EXPANSION");
                        printf("%s\n", m.err.message);
                    }

                    break;
                case '.':
                    t.kind = PERIOD;
                    list_push(sc->tokens, &t);
                    break;
                case '*':
                    t.kind = STAR;
                    list_push(sc->tokens, &t);
                    break;
                case '+':
                    t.kind = PLUS;
                    list_push(sc->tokens, &t);
                    break;
                case '-':
                    t.kind = MINUS;
                    list_push(sc->tokens, &t);
                    break;
                case '/':
                    t.kind = SLASH;
                    list_push(sc->tokens, &t);
                    break;
                case '=':
                    t.kind = EQUALS;
                    list_push(sc->tokens, &t);
                    break;
                case '(':
                    t.kind = OPEN_PAREN;
                    list_push(sc->tokens, &t);
                    break;
                case ')':
                    t.kind = CLOSE_PAREN;
                    list_push(sc->tokens, &t);
                    break;
                case '\\':
                    ignore_next_comma = !ignore_next_comma;
                    break;
                case ' ':
                case '\t':
                    break;
                default:
                    result(token) result;
                    if(isalpha(t.character)) {
                        result = _scanner_identifier_token(sc, string_builder, t.character);
                        list_push(sc->tokens, &result.result);
                    }else if(isdigit(t.character)) {
                        result = _scanner_number_token(sc, string_builder, t.character);
                        list_push(sc->tokens, &result.result);
                    }else {
                        LOG("Unknown token: %c, at %zu:%zu", t.character, t.line, t.column);
                        delete_list(string_builder);
                        RESULT_ERROR_RETURN(bool, "Uknown token", UNKNOWN_TOKEN);
                    }
            };
        }

    }

    // Validating proper separation of cells
    //    token *list = (token*)sc->tokens;
    //    for(size_t i = 0; i < sc->tokens->len; i++) {
    //        if(list[i].kind != COMMA && list[i].kind != NEWLINE && i + 1 < sc->tokens->len) {
    //            if(list[i+1].kind == COMMA || list[i].kind == NEWLINE) {
    //                RESULT_ERROR_RETURN(bool, "Cannot contain more than one component per cell", UNEXPECTED_TOKEN);
    //            }
    //        }
    //    }

    RESULT_RETURN(bool, true);
}

void _scanner_tokenize_string(scanner *sc) {

    size_t start_line = 0, start_col = 0, start_index = 0;

    start_line = sc->br->line; start_col = sc->br->column; start_index = sc->br->index;
    string_view sv = (string_view){.str = &sc->br->buffer[start_index], .len = 0};


    for(result(char) str_ch_result = bf_get_char(sc->br);
    str_ch_result.status == OK;
    str_ch_result = bf_get_char(sc->br)) {
        switch(str_ch_result.result) {
            case ',':
                list_push_rv(sc->tokens, token_new_sv(start_line, start_col, start_index, sv));
                list_push_rv(sc->tokens, token_new_ch(COMMA, sc->br->line, sc->br->column, sc->br->index, ','));
                return;
            case '\n':
                list_push_rv(sc->tokens, token_new_sv(start_line, start_col, start_index, sv));
                list_push_rv(sc->tokens, token_new_ch(NEWLINE, sc->br->line, sc->br->column, sc->br->index, '\n'));
                return;
            default:
                sv.len++;
        }
    }
}

result(bool) scanner_start_scan(scanner *sc) {
    NULL_ARGUMENT_ERROR_RETURN(bool, sc);

    list(char) string_builder = new_list(char);

    for(result(char) ch_result = bf_get_char(sc->br);
    ch_result.status == OK;
    ch_result = bf_get_char(sc->br)) 
    {
        if(isspace(ch_result.result)) {
            continue;
        }else if(ch_result.result == '=' || isdigit(ch_result.result)) {
            sc->br->index--;
            _scanner_tokenize_expression(sc, string_builder);
        }else {
            sc->br->index--;
            _scanner_tokenize_string(sc);
        }
    }

    delete_list(string_builder);
    RESULT_RETURN(bool, true);
}

bool _scanner_is_identifier_token(result(char) ch) {
    if(ch.status == ERROR) return false;
    return isalnum(ch.result);
}

result(token) _scanner_identifier_token(scanner *sc, list(char) string_builder, char first_ch) {
    list_clear(string_builder);
    list_push(string_builder, &first_ch);

    while(_scanner_is_identifier_token(bf_peek_char(sc->br))) {
        list_push_rv(string_builder, bf_get_char(sc->br).result);
    }

    list_push_rv(string_builder, '\0');

    char *str = strdup((char*)string_builder->list);

    RESULT_RETURN(token, token_new_id(sc->br->line, sc->br->column, sc->br->index, str));
}

bool _scanner_is_number_token(result(char) ch, bool found_dot) {
    switch(ch.status) {
        case ERROR:
            return false;
        case OK:
            return isdigit(ch.result) || (!found_dot && ch.result == '.');
    }
    return false; // Unreachable, all cases in switch are covered
}

result(token) _scanner_number_token(scanner *sc, list(char) number_builder, char first_ch) {
    list_clear(number_builder);
    list_push(number_builder, &first_ch);

    bool found_dot = false;
    while(_scanner_is_number_token(bf_peek_char(sc->br), found_dot) ) {

        result(char) ch = bf_get_char(sc->br);
        list_push_rv(number_builder, ch.result);

        if(ch.result == '.') found_dot = true;
    }

    list_push_rv(number_builder, '\0');

    double num = strtod((char*)number_builder->list, NULL);

    RESULT_RETURN(token, token_new_num(sc->br->line, sc->br->column, sc->br->index, num));
}

