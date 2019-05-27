#ifndef TABLES_H
#define TABLES_H

#include <stddef.h>
#include <sys/types.h>

/* token table */
#define INIT_GR_TABLE_SIZE 10
#define INIT_GR_RULES_SIZE 20
#define INIT_TABLE_SIZE 20
#define TABLE_INC_SIZE 2*INIT_TABLE_SIZE
#define TOK_INC_SIZE 25
#define RUL_INC_SIZE 20
#define MAX_TOKS 1
#define MAX_RULS 20
#define MAX_RULS_SZ 20
#define tok_tab_t char
#define num_tok_attribs 4

#define TERMINAL 1
#define NONTERMINAL 2


struct attribute_s 
{
	tok_tab_t * name;
	tok_tab_t val;
	u_int8_t type;
    	size_t tval;
    	size_t termnum;
};

#define attr_t struct attribute_s
#define tok_tbl_t struct attribute_s

struct symbol_s
{
	size_t num_attribs;
	attr_t * attribs; /* array of attribute structs */
	tok_tab_t * name;
	tok_tab_t val;
    	size_t tval;
};

#define symb_t struct symbol_s

struct rule_s
{
	size_t num_symbs;
	size_t used;
	symb_t ** symbols;
};

#define rule_t struct rule_s


struct table_s
{
	size_t num_ruls;
	size_t used;
	size_t num_toks;
	size_t tokused;
    	size_t num_terms;
    	size_t num_nonterms;
	rule_t ** rules;
	tok_tbl_t* tokens;
};

#define gr_tbl_t struct table_s
#define INIT_GRTBL_SIZE 10

/* Token */
tok_tab_t* get_tok_nam(tok_tbl_t* token);
tok_tab_t get_tok_val(tok_tbl_t* token);
size_t get_tok_type(tok_tbl_t* token);
size_t get_tok_termnum(tok_tbl_t* token);
size_t get_tok_tval(tok_tbl_t* token);
void set_tok_tval(tok_tbl_t* in_token, size_t val);
void set_tok_nam(tok_tbl_t* token, tok_tab_t* name);
void set_tok_val(tok_tbl_t* token, tok_tab_t val);
void set_tok_type(tok_tbl_t* token, size_t type);
void set_tok_termnum(tok_tbl_t* token, size_t term);
void print_tok(tok_tbl_t* intok);
tok_tbl_t* get_tok_by_id(tok_tbl_t* inarray, size_t id);
void print_tok_array(tok_tbl_t* inarray,size_t used);
/* end token methods */

/* symbols */
attr_t* get_symb_attrib(symb_t* symbol);
tok_tab_t* get_symb_nam(symb_t* symbol);
tok_tab_t get_symb_val(symb_t* symbol);
size_t get_symb_tval(symb_t* in_symb);
symb_t * create_symb(char* name, tok_tab_t val);
size_t get_symb_num_attrib(symb_t* symbol);
void set_symb_attrib(symb_t* symbol, attr_t* attribs);
void set_symb_nam(symb_t* symbol, tok_tab_t* name);
void set_symb_val(symb_t* symbol, tok_tab_t val);
void set_symb_tval(symb_t* in_symb, size_t val);
symb_t* copy_symb(symb_t* in_symb);
void print_symbol(symb_t * symbol);
/* end symbol methods */

/* rule */
rule_t * create_grrul(char* name, size_t size);
void add_symb_to_rule(rule_t* rule,symb_t * symbol);
void print_rule(rule_t* rule);
symb_t* get_symb_by_pos(rule_t* rule, size_t pos);
/* end rule methods */
/* table */
gr_tbl_t * create_grtbl(char* name,size_t size);
void add_rule_to_table(gr_tbl_t * table, rule_t* rule);
void calculate_num_terms(gr_tbl_t* grammar_table);
void print_gr_table(gr_tbl_t * table);
rule_t* get_rul_by_pos(gr_tbl_t* table, size_t pos);
/* end table methods */


#endif
