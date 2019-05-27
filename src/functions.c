#include "functions.h"
#include <stddef.h>
#include <stdlib.h>

int process_token(char val, char* nam,u_int8_t type,int pos,gr_tbl_t *grammar_table){
    	size_t newsz = grammar_table->tokused+TOK_INC_SIZE;
    tok_tbl_t* temptok;
	int temp_tok = tok_exists(nam,grammar_table);
	if(temp_tok == -1){
	    if(grammar_table->tokused == grammar_table->num_toks){
		   printf("need more memory for tokens, extending if possible\n");
		   grammar_table->tokens = realloc(grammar_table->tokens,sizeof(tok_tbl_t)*newsz);
		   if(!grammar_table->tokens){
			  perror("Realloc in process token:");
			  exit(EXIT_FAILURE);
		   }
		   for(size_t z=grammar_table->tokused;z<newsz;z++)
			  memset(get_tok_by_id(grammar_table->tokens,z),0,sizeof(tok_tbl_t));

		   grammar_table->num_toks += TOK_INC_SIZE;
	    	}
	    	temptok = get_tok_by_id(grammar_table->tokens,grammar_table->tokused);
	    	set_tok_nam(temptok,nam);
	    	set_tok_val(temptok,val);
	    	set_tok_type(temptok,type);
	    	set_tok_termnum(temptok,0);
		return (int)grammar_table->tokused++;
	}
	else{
	    temptok = get_tok_by_id(grammar_table->tokens,temp_tok);
	    /* !pos means position 0, head symbol, so if head symbol */
	    if(!pos|| does_tok_start_rul(temptok,grammar_table)){
		   set_tok_type(temptok,NONTERMINAL);
		}
	    
	}
	return temp_tok;
}

int does_tok_start_rul(tok_tbl_t* token, gr_tbl_t* in_gr_table){
    if(!token || !in_gr_table) return 0;
    for(int i=0;i<in_gr_table->used;i++){
	   if(get_symb_tval(get_symb_by_pos(get_rul_by_pos(in_gr_table,i),0)) == token->tval){
		  return 1;
	   }
    }
    return 0;
}

int tok_exists(char* nam,gr_tbl_t *grammar_table){
	for(int i=0;i<grammar_table->tokused;i++){
		if(!(strcmp(get_tok_nam(get_tok_by_id(grammar_table->tokens,i)),nam))){
#ifdef debug_print
			printf("old token\n");
#endif
		    return i;
		}
	}
#ifdef debug_print
	if(!grammar_table->tokused)
		printf("first token\n");
	else
		printf("new token\n");
#endif
    return -1;
}
