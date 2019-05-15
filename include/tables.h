#ifndef TABLES_H
#define TABLES_H

#include <stddef.h>
#include <sys/types.h>

/* token table */
#define INIT_GR_TABLE_SIZE 10
#define INIT_GR_RULES_SIZE 20
#define INIT_TABLE_SIZE 20
#define TABLE_INC_SIZE 2*INIT_TABLE_SIZE
#define MAX_TOKS 20
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
};

#define attr_t struct attribute_s
#define tok_tbl_t struct attribute_s

struct symbol_s
{
	size_t num_attribs;
	attr_t * attribs; /* array of attribute structs */
	tok_tab_t * name;
	tok_tab_t val;
};

#define symb_t struct symbol_s

struct rule_s
{
	size_t num_symbs;
	size_t used;
	symb_t ** symbols;
};

#define rule_t struct rule_s


//#define gr_tbl gr_t *
struct table_s 
{
	size_t num_ruls;
	size_t used;
	size_t num_toks;
	size_t tokused;
	rule_t ** rules;
	tok_tbl_t* tokens;
};
#define gr_tbl_t struct table_s
#define INIT_GRTBL_SIZE 10
//#define create_grtbl(name,size) ((name) = malloc(sizeof(gr_t*)*size))
//#define create_dyn_grtbl(name,size) gr_tbl_t *(name)[INIT_GRTBL_SIZE];\
(name) = malloc(sizeof(gr_tbl_t))
#define add_rule_to_tbl(name,which,rll) (name)[(which)] = (rll)
#define create_rules_array(name,size) 	gr_t (name)[(size)]
#define create_attribute_array(name,size) 	attr_t (name)[(size)]
#define create_tok_tbl(name,size) 	tok_tbl_t (name)[(size)]
#define create_dyn_tok_tbl(name,size) 	tok_tbl_t * (name); \
										(name) = malloc(sizeof(tok_tbl_t)*(size))
#define res_dyn_tok_tbl(name,size) if(sizeof((name)) < (sizeof(tok_tbl_t)*(size))) \
											realloc((name),(size));
#define set_symb_name(ats,which,nm) ((ats)[(which)].symb_name = (nm))
#define attach_attrib_array(ats,which,ar) ((ats)[(which)].attribs = (ar))
#define set_attribute_name(ats,which,nm) (ats)[(which)].name = (nm)
#define set_tok_name(ats,which,nm) (ats)[(which)].name = (nm)
#define set_attribute_val(ats,which,nm) ((ats)[(which)].val = (nm))
#define set_tok_val(ats,which,nm) ((ats)[(which)].val = (nm))
#define set_tok_type(ats,which,nm) ((ats)[(which)].type = (nm))
#define get_rul_nam(ats,which) ((ats)[(which)].symb_name)
#define get_attribs(ats,which) ((ats)[(which)].attribs)
#define get_attr_nam(ats,which,b) ((ats)[(which)].attribs[(b)].name)
#define get_attr_val(ats,which,b) ((ats)[(which)].attribs[(b)].val)
#define get_tok_nam(ats,which) ((ats)[(which)].name)
#define get_tok_val(ats,which) ((ats)[(which)].val)
#define get_tok_type(ats,which) ((ats)[(which)].type)
//#define 

symb_t * create_symb(char* name, tok_tab_t val);
rule_t * create_grrul(char* name, size_t size);
gr_tbl_t * create_grtbl(char* name,size_t size);

symb_t* copy_symb(symb_t* in_symb);

void add_symb_to_rule(rule_t* rule,symb_t * symbol);
void add_rule_to_table(gr_tbl_t * table, rule_t* rule);

void print_tok(tok_tbl_t* intok);
void print_tok_array(tok_tbl_t* inarray,size_t used);
void print_rule(rule_t* rule);
void print_gr_table(gr_tbl_t * table);
//tok_tbl_t * create_tok_tbl(char * name,size_t size);
#endif