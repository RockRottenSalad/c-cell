
#include "spreadsheet.h"
#include "expression.h"
#include "token.h"
#include "error.h"

#include<string.h>

spreadsheet* new_spreadsheet(list(token) tokens) {

    spreadsheet *spreadsheet_ptr = calloc(1, sizeof(struct spreadsheet));

    spreadsheet_ptr->rows = 10;
    spreadsheet_ptr->columns = 25;

    {
        // TODO: Determine rows/cols from tokens
        void *mem = calloc(spreadsheet_ptr->rows * spreadsheet_ptr->columns, sizeof(struct cell));
        spreadsheet_ptr->sheet = calloc(spreadsheet_ptr->rows, sizeof(struct cell));

        for(size_t r = 0; r < spreadsheet_ptr->rows; r++) {
            spreadsheet_ptr->sheet[r] = (struct cell*)( &(((char*)mem)[sizeof(struct cell) * spreadsheet_ptr->columns * r]));
        }
    }

    size_t len = tokens->len;
    size_t col = 0, row = 0;

    token *token_list = (token*)tokens->list;
    for(size_t i = 0; i < len; i++) {
        switch(token_list[i].kind) {
            case COMMA:
                col++;
                break;
            case NEWLINE:
                row++;
                col = 0;
                break;
            case EQUALS:
                spreadsheet_ptr->sheet[row][col].kind = EXPRESSION;
                result(expression_ptr) r = new_expression_from_tokens(tokens, i, COMMA);
                if(r.status == OK)
                    spreadsheet_ptr->sheet[row][col].expr = r.result;
                else {
                    while(has_had_error()) {
                        error_log e = get_error();
                        LOG("Error in cell: <%zu, %zu> | %s", row, col, e.message);
                    }
                    spreadsheet_ptr->sheet[row][col].expr = new_literal_expression(0.0);
                }
                break;
            case STRING_VIEW:
                spreadsheet_ptr->sheet[row][col].kind = TEXT;
                spreadsheet_ptr->sheet[row][col].text = (string_view){.str = token_list[i].sv.str, .len = token_list[i].sv.len};
                break;
            default:
        }
    }

    return spreadsheet_ptr;
}

void delete_spreadsheet(spreadsheet *spreadsheet_ptr) {
    if(spreadsheet_ptr == NULL) {
        PANIC("Attempted to free NULL pointer with delete_spreadsheet()");
    }

    struct cell *c_list = spreadsheet_ptr->sheet[0];

    for(size_t i = 0; i < spreadsheet_ptr->rows * spreadsheet_ptr->columns; i++) {
        switch(c_list[i].kind) {
            case EXPRESSION:
                delete_expression(c_list[i].expr);
                break;
            default:
                break;
        }
    }

    free(spreadsheet_ptr->sheet[0]);
    free(spreadsheet_ptr->sheet);
    free(spreadsheet_ptr);
}

void spreadsheet_evaluate_expressions(spreadsheet *spreadsheet_ptr) {

    struct cell *c_list = spreadsheet_ptr->sheet[0];

    for(size_t i = 0; i < spreadsheet_ptr->rows * spreadsheet_ptr->columns; i++) {
        switch(c_list[i].kind) {
            case EXPRESSION:
                printf("Eval: %lf\n", evaluate(c_list[i].expr, spreadsheet_ptr));
                break;
            default:
                break;
        }
    }
}

// TODO: String builder instead of separate prints you twat
void spreadsheet_print(spreadsheet *spreadsheet_ptr, FILE *fd) {
    for(int r = 0; r < (int)spreadsheet_ptr->rows; r++) {
        for(int c = 0; c < (int)spreadsheet_ptr->columns; c++) {
            switch(spreadsheet_ptr->sheet[r][c].kind) {
                case TEXT:
                    fprintf(fd, "[%d, %d]%.*s,",
                            r, c, (int)spreadsheet_ptr->sheet[r][c].text.len, spreadsheet_ptr->sheet[r][c].text.str);
                    break;
                case EXPRESSION:
                    fprintf(fd, "[%d, %d]%lf,", r, c, evaluate(spreadsheet_ptr->sheet[r][c].expr, spreadsheet_ptr));
                    break;
                case EMPTY:
                    fprintf(fd, ",");
                    break;
            }
        }
        fprintf(fd, "\n");
    }
}

