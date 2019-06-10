#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "tables.h"
int process_token(char tok_val,
				char* tok_name,u_int8_t type,int pos,gr_tbl_t *grammar_table);
int tok_exists(char* name,gr_tbl_t *grammar_table);
void finalize_rule_prec(rule_t* temp_rules, gr_tbl_t* in_gr_table);



#endif
