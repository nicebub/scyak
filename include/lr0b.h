#ifndef LR0B_H
#define LR0B_H
#include "lr0.h"

struct lr0_set* CLOSURE(struct lr0_set* in_set, struct lr0_set* universe,gr_tbl_t* in_gr_table);
struct lr0_set* create_universe_lr0_set(gr_tbl_t* grammar_table);
struct lr0_set* GOTO(struct lr0_set* inset, tok_tab_t in_token,struct lr0_set* universe_set,gr_tbl_t* grammar_table);
struct lr0_array_set* items(gr_tbl_t* grammar_table);
struct lr0_set* FIRST(tok_tbl_t* token, struct lr0_set* universe, gr_tbl_t* in_gr_table);
struct lr0_set* FOLLOW(struct lr0_set* inset, struct lr0_set* universe, gr_tbl_t* in_gr_table);
void get_first_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table);
void get_follow_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table);
#endif