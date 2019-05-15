#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "tables.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
int process_token(tok_tbl_t **intable, char tok_val,
				char* tok_name,u_int8_t type,int pos,gr_tbl_t *grammar_table);
int tok_exists(tok_tbl_t *intable, char* name,gr_tbl_t *grammar_table);



#endif