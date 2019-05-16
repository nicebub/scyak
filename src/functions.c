#include "functions.h"
#include <stddef.h>
#include <stdlib.h>

int process_token(tok_tbl_t **intable, char val, char* nam,u_int8_t type,int pos,gr_tbl_t *grammar_table){
	size_t *num_toks;
	num_toks = &(grammar_table->tokused);
	int temp_tok = tok_exists(*intable,nam,grammar_table);
	if(temp_tok == -1){
	    if(*num_toks >= grammar_table->num_toks){
		   printf("need more memory for tokens, extdending if possible\n");
		   grammar_table->tokens = realloc(grammar_table->tokens,sizeof(tok_tbl_t)*(grammar_table->num_toks+25));
		   grammar_table->num_toks += 25;
		   *intable = grammar_table->tokens;
/*		   for(size_t i=grammar_table->tokused;i<grammar_table->num_toks;i++)
			  grammar_table->tokens[i];*/
	    }
	    (*intable)[(*num_toks)].name = nam;
	    (*intable)[(*num_toks)].val =val;
	    (*intable)[(*num_toks)].type =type;
/*		set_tok_name(*intable,*num_toks,nam);
		set_tok_val(*intable,*num_toks,val);
		set_tok_type(*intable,*num_toks,type);*/
		*num_toks += 1;
		return (int)*num_toks-1;
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
