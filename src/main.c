
#include<stdio.h>

#include "buffered_reader.h"
#include "scanner.h"
#include "result.h"
#include "spreadsheet.h"
#include "error.h"

#ifdef TEST

#include "test/test_runner.h"

int main(void) {
    run_tests();
    return 0;
}

#else

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    init_error_logging();

    buffered_reader *br = NULL;
    {
        result(buffered_reader_ptr) br_result = new_buffered_reader(fopen("./test.csv", "rb+"));
        switch(br_result.status) {
            case OK:
                br = br_result.result;
                break;
            case ERROR:
                PANIC("Failed to create buffered reader: %s", br_result.err.message);
        }
    }

    scanner *sc; unwrap(sc, new_scanner(br));

    scanner_start_scan(sc);

    spreadsheet *sh = new_spreadsheet(sc->tokens);
    //spreadsheet_evaluate_expressions(sh);
    spreadsheet_print(sh, stdout);

    delete_spreadsheet(sh);
    delete_scanner(sc);
    delete_buffered_reader(br);

    cleanup_error_logging();

//    printf("Hello, World\n");
    return 0;
}
// size_t len = list_len(sc->tokens);
//    token *tokens = sc->tokens->list;
//    for(size_t i = 0; i < len; i++) {
//        switch(tokens[i].kind) {
//            case PLUS:
//                LOG("Got PLUS token");
//                break;
//            case COLON:
//                LOG("Got COLON token");
//                break;
//            case MINUS:
//                LOG("Got MINUS token");
//                break;
//            case COMMA:
//                LOG("Got COMMA token");
//                break;
//            case PERIOD:
//                LOG("Got PERIOD token");
//                break;
//            case STAR:
//                LOG("Got STAR token");
//                break;
//            case SLASH:
//                LOG("Got SLASH token");
//                break;
//            case OPEN_PAREN:
//                LOG("Got OPEN PAREN token");
//                break;
//            case CLOSE_PAREN:
//                LOG("Got CLOSE PAREN token");
//                break;
//            case EQUALS:
//                LOG("Got EQUALS token");
//                break;
//            case NEWLINE:
//                LOG("Got NEWLINE token");
//                break;
//            case NUMBER:
//                LOG("Got NUMBER token: %lf", tokens[i].number);
//                break;
//            case STRING:
//            case IDENTIFIER:
//                LOG("Got STRING/ID token: %s", tokens[i].string);
//                break;
//            case STRING_VIEW:
//                LOG("Got STRING_VIEW token: %.*s", (int)tokens[i].sv.len, tokens[i].sv.str);
//                break;
//            default:
//                break;
//        }
//    }

#endif /* TEST */
