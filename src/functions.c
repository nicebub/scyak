#include <stddef.h>
#include <stdlib.h>
#include "functions.h"
#include "tokens.h"
u_int8_t prec_count;
int process_token(char val, char* nam,u_int8_t type,int pos,gr_tbl_t *grammar_table){
    size_t newsz;
    tok_tbl_t* temptok;
    tok_tbl_t* btok;
    int temp_tok;
    newsz = grammar_table->tokused+TOK_INC_SIZE;
    	temp_tok = tok_exists(nam,grammar_table);
	if(temp_tok == -1){
	    if(grammar_table->tokused == grammar_table->num_toks){
//		   printf("need more memory for tokens, extending if possible\n");
		   grammar_table->tokens = realloc(grammar_table->tokens,sizeof(tok_tbl_t)*newsz);
		   if(!grammar_table->tokens){
			  perror("Realloc in process token:");
			  exit(EXIT_FAILURE);
		   }
		   for(size_t z=grammar_table->tokused;z<newsz;z++){
			  btok = get_tok_by_id(grammar_table->tokens,z);
			  memset(btok,0,sizeof(tok_tbl_t));
			  set_tok_assoc(btok,-1);
			  set_tok_prec(btok,-1);
		   }

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
void finalize_rule_prec(rule_t* temp_rules, gr_tbl_t* grammar_table){
    if(!temp_rules){
	   printf("rule is null, can't finalize!\n");
	   exit(EXIT_FAILURE);
    }
    if(temp_rules->prec ==  -1 || temp_rules->assoc == -1){
	   for(size_t r=temp_rules->used-1;r>=1;r--){
		  symb_t* prsym;
		  tok_tbl_t* prtok;
		  tok_tab_t prval;
		  size_t prtval;
		  prsym = get_symb_by_pos(temp_rules,r);
		  prval = get_symb_val(prsym);
		  prtval = get_symb_tval(prsym);
		  prtok = get_tok_by_id(grammar_table->tokens,prtval);
		  if(prval != CODE && prval != EMPTY && get_tok_type(prtok) == TERMINAL){
//			 printf("found last terminal of rule, not code..it is: %s\n",get_tok_nam(prtok));
			 set_rul_prec(temp_rules,get_tok_prec(prtok));
			 set_rul_assoc(temp_rules,get_tok_assoc(prtok));
			 return;
		  }
//		  printf("no terminal yet, keep looking...\n");
	   }
//	   printf("didn't find any terminals in this rule\n");
    }
}
