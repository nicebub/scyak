#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "tables.h"
#include "lr0b.h"
#include "lr0.h"
#define RETVAL 0
#define tok_type int
#define tok_name token

static tok_type tok_name;

#define current_tok token
/* Function Prototypes */
gr_tbl_t* read_and_parse_specfile(FILE* specfile);
int optional_definitions(FILE* specfile);
int optional_auxillary(FILE* specfile);
int definition(FILE* specfile);
int option_type(FILE* specfile);
int optional_tag(FILE* specfile);
int ident_list(int typ_tok,FILE* specfile);
int identcombo(int typ_tok,FILE* specfile);
int grammar_rules(FILE* specfile);
int rulebody(FILE* specfile,rule_t* rules);
int optional_precedence(FILE* specfile,rule_t* rules);
void check_for_empty_rule(rule_t* inrule);
char* read_def_code_sect(FILE* specfile);
char* read_aux_code_sect(FILE* specfile);
char* process_codeblock(FILE* specfile,rule_t* current_rule);
void char_to_code(int *c,char** code_head,size_t* def_used,size_t* def_sz,char** defcode);
void change_tbl_for_code_sects(FILE* specfile,gr_tbl_t* grammar_table);
#endif
