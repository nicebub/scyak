#ifndef LR0B_H
#define LR0B_H
#include "lr0.h"

#define SHIFT 1
#define REDUCE 2
#define ACCEPT 4
#define AERROR 8
#define STALL 15

struct action_s
{
    u_int8_t state;
    u_int8_t action;
    u_int8_t rule;
    u_int8_t numtoks;
};

struct parser_tables_s
{
    size_t num_states;
    size_t num_terms;
    size_t num_nonterms;
    struct action_s ** ACTION;
    int ** GTTBL;
};



struct lr0_set* CLOSURE(struct lr0_set* in_set, struct lr0_set* universe,gr_tbl_t* in_gr_table);
struct lr0_set* create_universe_lr0_set(gr_tbl_t* grammar_table);
struct lr0_set* GOTO(struct lr0_set* inset, tok_tab_t in_token,struct lr0_set* universe_set,gr_tbl_t* grammar_table);
struct lr0_array_set* items(gr_tbl_t* grammar_table);
struct lr0_set* FIRST(tok_tbl_t* token, struct lr0_set* universe, gr_tbl_t* in_gr_table);
struct lr0_set* FOLLOW(tok_tbl_t* token, struct lr0_set* universe, gr_tbl_t* in_gr_table);
void get_first_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table);
struct lr0_array_set* get_follow_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table);
struct parser_tables_s* create_action_n_goto_tbls(struct lr0_array_set* canon, gr_tbl_t* grammar_table);
void fill_action_table(struct parser_tables_s* ptable,gr_tbl_t* grammar_table, struct lr0_array_set* canon );
void print_action_n_goto_tbls(struct parser_tables_s* ptable);
#endif
