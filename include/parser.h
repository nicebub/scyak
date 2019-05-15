#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "tables.h"
#define RETVAL 0
#define tok_type int
#define tok_name token

static tok_type tok_name;

#define current_tok token


/* Function Prototypes */
void read_and_parse_specfile(FILE* specfile);
int optional_definitions(FILE* specfile);
int optional_auxillary(FILE* specfile);
int definition(FILE* specfile);
int option_type(FILE* specfile);
int optional_tag(FILE* specfile);
int ident_list(FILE* specfile);
int identcombo(FILE* specfile);
int grammar_rules(FILE* specfile);
int rulebody(FILE* specfile,rule_t* rules);
int optional_precision(FILE* specfile,rule_t* rules);

#endif