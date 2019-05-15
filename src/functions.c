#include "functions.h"
#include <stddef.h>

int process_token(tok_tbl_t **intable, char val, char* nam,u_int8_t type,int pos,gr_tbl_t *grammar_table){
	size_t *num_toks;
	num_toks = &(grammar_table->tokused);
	int temp_tok = tok_exists(*intable,nam,grammar_table);
	if(temp_tok == -1){
		set_tok_name(*intable,*num_toks,nam);
		set_tok_val(*intable,*num_toks,val);
		set_tok_type(*intable,*num_toks,type);
		*num_toks += 1;
		return *num_toks-1;
	}
	else if(!pos){
			set_tok_type(*intable,temp_tok,NONTERMINAL);
		}
	
	return temp_tok;
}

int tok_exists(tok_tbl_t *intable, char* nam,gr_tbl_t *grammar_table){
	size_t *num_toks;
	num_toks = &(grammar_table->tokused);
	for(int i=0;i<*num_toks;i++){
		if(!(strcmp(intable[i].name,nam))){
			printf("old token\n");
			return i;
		}
	}
	if(!(*num_toks))
		printf("first token\n");
	else
		printf("new token\n");
	return -1;
}