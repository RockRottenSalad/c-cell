#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "expression.h"
#include "global_types.h"

spreadsheet* new_spreadsheet(list(token) tokens);
void delete_spreadsheet(spreadsheet *ssheet);

void spreadsheet_evaluate_expressions(spreadsheet *spreadsheet_ptr);
void spreadsheet_print(spreadsheet *spreadsheet_ptr, FILE *fd); 


#endif /* SPREADSHEET_H */
