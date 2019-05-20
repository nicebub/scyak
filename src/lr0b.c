#include "lr0b.h"
#include <stdio.h>
#include "bitset.c"
#include "functions.h"
#include "tokens.h"
struct lr0_set* create_universe_lr0_set(gr_tbl_t* grammar_table){
	size_t num_of_symbs;
	struct lr0_set * new_set;
	num_of_symbs = 0;
	if(grammar_table && grammar_table->rules){
		for(int i=0;i<grammar_table->used;i++){
			num_of_symbs += grammar_table->rules[i]->used;
		}
		new_set = create_lr0_set(num_of_symbs);
		for(int i=0;i<grammar_table->used;i++){
			int j;
			for(j=0;j<grammar_table->rules[i]->used;j++){
				add_item_to_set(new_set,create_lr0_item(i,j,grammar_table->rules[i]->symbols[j]));
			}
#ifdef debug_print
		    printf("checking j %d and used %zu\n",j,grammar_table->rules[i]->used);
#endif
			add_item_to_set(new_set,create_lr0_item(i,j,
				create_symb(grammar_table->tokens[1].name,1)));
		}
#ifdef debug_print
		print_lr0_set(new_set);
#endif
	    return new_set;
	}
	return NULL;
}

struct lr0_set* CLOSURE(struct lr0_set* in_set, struct lr0_set* universe, gr_tbl_t* in_gr_table){
	int made_additions;
	int head_val;
	struct lr0_set* out_set;
//	in_gr_table->tokens;
	bitset_t * added;
	size_t add_size;
	add_size = bits_take_up(in_gr_table->num_toks);
	added = malloc(sizeof(bitset_t)*add_size);
	init_set(added,BITS_TO_USE);
	made_additions = 1;
	out_set = copy_lr0_set(in_set); //make a copy J of incoming set I
	while(made_additions){
		made_additions = 0;
		for (int i=0;i<out_set->used;i++){ //each item A -+ a.BP in J 
			if(out_set->items[i]->pos == 0){
				head_val = out_set->items[i]->symbol->val;
#ifdef debug_print
			    printf("head value is: %d\n",head_val);
#endif
			}
			else{
#ifdef debug_print
				printf("dot before symbol: %d:%s\n",out_set->items[i]->symbol->val,
					  out_set->items[i]->symbol->name);
#endif
			    for (int j=1;j<universe->used;j++){ //each production B -+ y of G
					if(universe->items[j]->pos ==0 && 
						universe->items[j]->symbol->val==out_set->items[i]->symbol->val){
#ifdef debug_print
						printf("found a rule starting with: %s:\n",universe->items[j]->symbol->name);
#endif
					    j++;
						if(!item_is_in_set(universe->items[j],out_set)){
#ifdef debug_print
							printf("item is not in set,adding...\n");
#endif
						    add_item_to_set(out_set,universe->items[j]);
							made_additions=1;
						}
						else{
						    ;
#ifdef debug_print
						    printf("item already in set\n");
#endif
						}
					}
		/*		if(out_set->items[i]->rule)
				if ( B -+ .y is not in J )
					add B -+ .y to J;
				*/
				}
			}
		}
	}
	return out_set;
}

struct lr0_set* GOTO(struct lr0_set* inset, tok_tab_t in_token,struct lr0_set* universe_set,gr_tbl_t* grammar_table){
	struct lr0_set* out_set;
	out_set = create_lr0_set(20);
	for(int i=0;i<inset->used;i++){
#ifdef debug_print
		printf("inside GOTO loop\n");
		printf("testing values %d and %d\n",in_token,inset->items[i]->symbol->val);
#endif
	    if(inset->items[i]->pos!=0&&inset->items[i]->symbol->val == in_token){
#ifdef debug_print
			printf("value equals token value and trying to add to set\n");
#endif
		   if(inset->items[i]->pos+1 < grammar_table->rules[inset->items[i]->rule]->used){
				add_item_to_set(out_set,create_lr0_item(inset->items[i]->rule,inset->items[i]->pos+1,
					grammar_table->rules[inset->items[i]->rule]->symbols[inset->items[i]->pos+1]));
#ifdef debug_print
				print_lr0_set(out_set);
#endif
		   }
			else if(inset->items[i]->pos+1 == grammar_table->rules[inset->items[i]->rule]->used){
				add_item_to_set(out_set,create_lr0_item(inset->items[i]->rule,inset->items[i]->pos+1,
					create_symb(grammar_table->tokens[1].name,1)));
#ifdef debug_print
					print_lr0_set(out_set);
#endif
			}
		
		}
	}
	out_set = CLOSURE(out_set,universe_set,grammar_table);
#ifdef debug_print
	printf("so far the set we created in GOTO function is:\n");
	print_lr0_set(out_set);
#endif
    return out_set;
}

struct lr0_set* universe_set;

struct lr0_array_set* items(gr_tbl_t* grammar_table) {
	struct lr0_array_set* C;
	struct lr0_set* initial_set;
	struct lr0_set* got_set;
//	struct lr0_set* clos_got_set;
	C = create_lr0_array(20);
	int made_changes;
	made_changes = 1;
	universe_set = create_universe_lr0_set(grammar_table);
	initial_set = create_lr0_set(10);
	add_item_to_set(initial_set,create_lr0_item(0,0,grammar_table->rules[0]->symbols[0]));
	add_item_to_set(initial_set,create_lr0_item(0,1,grammar_table->rules[0]->symbols[1]));
	add_set_to_array(C,CLOSURE(initial_set,universe_set,grammar_table));
#ifdef debug_print
	print_lr0_array(C);
#endif
    //	got_set = GOTO(C->array[0],2,grammar_table);
//	clos_got_set = CLOSURE(got_set,universe_set,grammar_table);
//	C = CLOSURE()
/*	C = CLOSURE(of the set of only first rule, dot on far left of rhs); s' -> .s*/
#ifdef debug_print
	printf("tokens we are working with\n");
	print_tok_array(grammar_table->tokens,grammar_table->tokused);
#endif
    /*	for(int r=0;r<grammar_table->tokused;r++){
		printf("name:%s, value:%d, token:%d\n",grammar_table->tokens[r].name,grammar_table->tokens[r].val,r);
	}
	*/
	while(made_changes){
#ifdef debug_print
		printf("in while loop\n");
#endif
	    made_changes=0;
		for (int i=0;i<C->used;i++){ //each set of items I in C )
#ifdef debug_print
			printf("in first loop\n");
#endif
		    for (int j=1;j<grammar_table->tokused;j++){ //each grammar symbol X
#ifdef debug_print
				printf("in second loop\n");
#endif
			   	got_set = GOTO(C->array[i],j,universe_set,grammar_table);
				if (got_set->used != 0){ //GOTO(I,X ) is not empty and not in C
#ifdef debug_print
					printf("goto(i,x) is not empty\n");
#endif
				    if(set_is_in_array(got_set,C)== -1){
						add_set_to_array(C,got_set);
						made_changes=1;
					}
				}
			}
		}
	}
#ifdef debug_print
	print_lr0_array(C);
#endif
    return C;
}


int rule_goes_to_empty(int rule, gr_tbl_t* in_gr_table){
	if(!in_gr_table|| in_gr_table->used==0) return 0;
	return (in_gr_table->rules[rule]->symbols[1]->val==EMPTY);
}

struct lr0_set* FIRST(tok_tbl_t* token, struct lr0_set* universe, gr_tbl_t* in_gr_table){
	struct lr0_set* outset;
	if(!token || !universe || !in_gr_table || in_gr_table->tokused ==0){
		printf("problem\n");
		return NULL;
	}
	if(token->type == TERMINAL){
#ifdef debug_print
		printf("found terminal\n");
#endif
	    outset = create_lr0_set(1);
	    struct lr0_item* out_item =create_lr0_item(token->tval,token->tval,NULL);
	    	out_item->token = token;
		add_item_to_set(outset,out_item);
		return outset;
	}
	else{
	//	struct lr0_set* out_set;
		struct lr0_set* temp_set;
		int tok_ex;
		outset = create_lr0_set(10);
//		temp_set = create_lr0_set(10);
		if(does_tok_start_rul(token,in_gr_table)){
			for(int i=1;i<in_gr_table->used;i++){
#ifdef debug_print
				printf("starting first loop\n");
#endif
			    if(in_gr_table->rules[i]->symbols[0]->tval == token->tval){
#ifdef debug_print
				    printf("found rule %d starting with token %zu\n",i,token->tval);
#endif
				   if(rule_goes_to_empty(i,in_gr_table)){
#ifdef debug_print
						printf("rule went to empty so adding to set if not already\n");
#endif
					  if(!item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),outset))
							add_item_to_set(outset,create_lr0_item(EMPTY,EMPTY,NULL));
					}
					for(int j=1;j<in_gr_table->rules[i]->used;j++){
#ifdef debug_print
						printf("starting second loop\n");
#endif
					    tok_tbl_t* ttok = malloc(sizeof(tok_tbl_t));
						ttok->name = in_gr_table->rules[i]->symbols[j]->name;
						tok_ex=tok_exists(in_gr_table->tokens,ttok->name,in_gr_table);
						if(tok_ex){
#ifdef debug_print
							printf("found token\n");
#endif
						    if(in_gr_table->tokens[tok_ex].tval != token->tval){
#ifdef debug_print
							printf("its ok, not an endless loop\n");
#endif
							   temp_set = FIRST(&(in_gr_table->tokens[tok_ex]),universe,in_gr_table);
							if(item_is_in_set(create_lr0_item(in_gr_table->tokens[1].tval,
								in_gr_table->tokens[1].tval,NULL),temp_set)){
#ifdef debug_print
								printf("ugh\n");
#endif
							    for(int r=0;r<temp_set->used;r++){
									if(!item_is_in_set(temp_set->items[r],outset))
										add_item_to_set(outset,temp_set->items[r]);
									}
							    if(j==in_gr_table->rules[i]->used-1){
								   if(!item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),outset))
									  add_item_to_set(outset,create_lr0_item(EMPTY,EMPTY,NULL));
							    }
								}
								else{
#ifdef debug_print
									printf("not ugh\n");
#endif
								    for(int r=0;r<temp_set->used;r++){
										if(!item_is_in_set(temp_set->items[r],outset))
										add_item_to_set(outset,temp_set->items[r]);
									}
									break;
								}
							}else break;
						}
					}
				}
			}
		}
//		print_lr0_set(outset);
		return outset;
	}
	
	
}
/*
struct lr0_set* FOLLOW(tok_tbl_t* token, struct lr0_set* universe, gr_tbl_t* in_gr_table){
    struct lr0_set* outset;
    if(!token || !universe || !in_gr_table || in_gr_table->tokused ==0){
	   printf("problem\n");
	   return NULL;
    }
    if(token->type == TERMINAL){
	   printf("found terminal in FOLLOW, move on\n");
	   return NULL;
    }
    else{
	   printf("not a token, lets rock the FOLLOW\n");
	   if(token->val == REALSTART){
		  printf("found start token S'");
		  outset = create_lr0_set(1);
		  printf("found the real start token, adding $ as follow\n");
		  struct lr0_item* out_item =create_lr0_item(DOLLAR,DOLLAR,NULL);
		  out_item->token = token;
		  add_item_to_set(outset,out_item);
		  return outset;
	   }
	   else{
		  for(int i=0;i<in_gr_table->used;i++){
			 if(in_gr_table->rules[i]->symbols[0]->tval == token->tval){
				struct lr0_set* temp;
					for(size_t j=in_gr_table->rules[i]->used-1;j>0;j--){
					    tok_tbl_t* temp_tok;
					    int num =tok_exists(in_gr_table->tokens,(in_gr_table->rules[i]->symbols[j]->name), in_gr_table);
					    printf("tokexists put back %d\n",num);
					    temp_tok = &(in_gr_table->tokens[num]);
					    temp = FIRST(temp_tok,universe,in_gr_table);
					    printf("just got FIRST of last token in a grammar\n");
					    print_lr0_set(temp);
					    if(item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),temp)){
						   printf("found empty in set!!\n");
					    }
					    else{
						   printf("didn't find EMPTY in set so can add to follow ");
						   printf(" of previous symbol to this one\n");
						   break;
					    }
					}
			 }
		  }
	   }
    }
    return NULL;
}
*/
/*
void get_first_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table){
	struct lr0_set* temp;
	for(int i=2;i<grammar_table->tokused;i++){
		printf("getting FIRST for token %d\n",i);
		temp = FIRST(&(grammar_table->tokens[i]),universe_set,grammar_table);
		printf("adding set to array\n");
	    print_lr0_set(temp);
		add_set_to_array(canon,temp);
		//print_lr0_set(temp);
	}
    print_lr0_array(canon);
}
*/
struct parser_tables_s* create_action_n_goto_tbls(struct lr0_array_set* canon, gr_tbl_t* grammar_table){
    struct action_s ** ACTION;
    int ** GTTBL;
    struct parser_tables_s* out_p_table;
    out_p_table = NULL;
    ACTION = NULL;
    GTTBL = NULL;
    ACTION = malloc(sizeof(struct action_s*)*canon->used);
    GTTBL = malloc(sizeof(int*)*canon->used);
    for(int i=0;i<canon->used;i++){
	   ACTION[i] = malloc(sizeof(struct action_s)*(grammar_table->num_terms+1));
	   GTTBL[i] = malloc(sizeof(int)*(grammar_table->num_nonterms));
	   for(int j=0;j<grammar_table->num_terms+1;j++){
		  ACTION[i][j].action = AERROR;
		  ACTION[i][j].state = 0;
		  ACTION[i][j].numtoks = 0;
		  ACTION[i][j].rule = 0;
	   }
	   for(int j=0;j<grammar_table->num_nonterms;j++)
		  GTTBL[i][j] = -1;
    }
    out_p_table = malloc(sizeof(struct parser_tables_s));
    out_p_table->ACTION = ACTION;
    out_p_table->GTTBL = GTTBL;
    out_p_table->num_terms = grammar_table->num_terms+1;
    out_p_table->num_nonterms = grammar_table->num_nonterms;
    out_p_table->num_states = canon->used;
    return out_p_table;
    //    out_p_table
}

struct lr0_array_set* get_follow_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table){
    struct lr0_set* temp = NULL;//, *temp2=NULL;
    struct lr0_array_set* follows=NULL;
    int should_still_look;
    int first_had_empty;
   // char y = '\0';
#ifdef debug_print
    printf("creating follow array\n");
#endif
    follows = create_lr0_array(grammar_table->tokused);
#ifdef debug_print
    printf("initializing follow array\n");
#endif
    for(int a=0;a<grammar_table->tokused;a++)
	   follows->array[a] = create_lr0_set(1);

    follows->used = grammar_table->tokused;

#ifdef debug_print
    printf("putting $ in follow of S'\n");
#endif
    add_item_to_set(follows->array[0],create_lr0_item(DOLLAR,DOLLAR,NULL));
    int found_empty;

    for(int j=0;j<grammar_table->used;j++){
	   first_had_empty=0;
	   should_still_look=1;
	   found_empty=0;
#ifdef debug_print
	   printf("first loop rule %d\n",j);
#endif
	   //	   temp = create_lr0_set(2);
	   struct lr0_item* citem;
	   for(size_t i=1;i<grammar_table->rules[j]->used-1;i++){
#ifdef debug_print
		  printf("second loop symbol %zu\n",i);
#endif
		  if(grammar_table->tokens[grammar_table->rules[j]->symbols[i]->tval].type == TERMINAL){
#ifdef debug_print
			 printf("symbol to the left of this one is a terminal, move on\n");
#endif
			 continue;
		  }
		  first_had_empty = 0;
		  for(size_t g=i+1;g<=grammar_table->rules[j]->used-1;g++){
			temp = FIRST(&grammar_table->tokens[grammar_table->rules[j]->symbols[g]->tval]
					,universe_set,grammar_table);
#ifdef debug_print
			printf("here is the first set found and used for rule %d grammar symbol %zu\n",j,g);
		  	print_lr0_set(temp);
			 //	scanf("%c",&y);
		  	printf("temp is now the FIRST set\n");
#endif
			 /* if(!item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),temp)){
			 printf("EMPTY is not in the first set temp\n");*/
			//printf("not the first symbol and not a terminal\n");
			//scanf("%c",&y);
			for(int v=0;v<temp->used;v++){
			    citem = copy_lr0_item(temp->items[v]);
			    found_empty = items_are_equal(citem,create_lr0_item(EMPTY,EMPTY,NULL));
			    if(!found_empty && !item_is_in_set(citem,follows->array[grammar_table->rules[j]->symbols[i]->tval])){
#ifdef debug_print
				   printf("adding item %d to set which has %d\n",v,temp->items[v]->pos);
#endif
				   add_item_to_set(follows->array[grammar_table->rules[j]->symbols[i]->tval]
						    ,citem);
			    }
			    else if(found_empty){
				   first_had_empty = 1;
			    }
			}
			if(!first_had_empty)
			    break;
		  }
	   }
    }
//	   printf("we just added items to a follow symbol, print follow array\n");
//	   print_lr0_array(follows);
	//   scanf("%c",&y);
    for(int j=0;j<grammar_table->used;j++){
	   first_had_empty=1;
	   should_still_look=1;
	   found_empty=0;
#ifdef debug_print
	   printf("first loop rule %d\n",j);
#endif
	   //	   temp = create_lr0_set(2);
	   struct lr0_item* citem;

	   for(size_t i=grammar_table->rules[j]->used-1;i>=1;i--){
		  if(grammar_table->tokens[grammar_table->rules[j]->symbols[i]->tval].type==TERMINAL){
#ifdef debug_print
			 printf("found a terminal, no more follow transfer from this rule\n");
#endif
			 break;
		  }
		  else{
#ifdef debug_print
				printf("not a terminal in later for loop\n");
#endif
			 temp = FIRST(&grammar_table->tokens[grammar_table->rules[j]->symbols[i]->tval]
						   ,universe_set,grammar_table);
#ifdef debug_print
				printf("here is the first set found and used for rule %d grammar symbol %zu\n",j,i);
				print_lr0_set(temp);
#endif
			 //	scanf("%c",&y);
			 	found_empty =item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),temp);
				if(found_empty){
				    for(int v=0;v<follows->array[grammar_table->rules[j]->symbols[0]->tval]->used;v++){
#ifdef debug_print
					   	printf("adding item %d to FOLLOW of head rule symbol which has %d\n",v,follows->array[grammar_table->rules[j]->symbols[0]->tval]->items[v]->pos);
#endif
					   	citem = follows->array[grammar_table->rules[j]->symbols[0]->tval]->items[v];
					   if(!item_is_in_set(citem,follows->array[grammar_table->rules[j]->symbols[i]->tval])){
#ifdef debug_print
					  printf("item wasn't in set, so really adding\n");
#endif
						  add_item_to_set(follows->array[grammar_table->rules[j]->symbols[i]->tval],citem);
						  
					   }
				    }
				}
				else{
//				    if(i == grammar_table->rules[j]->used-1){
#ifdef debug_print
					   	printf("not empty in set?\n");
#endif
				    for(int v=0;v<follows->array[grammar_table->rules[j]->symbols[0]->tval]->used;v++){
#ifdef debug_print
					    printf("adding item %d to FOLLOW of head rule symbol which has 	%d\n",v,follows->array[grammar_table->rules[j]->symbols[0]->tval]->items[v]->pos);
#endif
					   citem = follows->array[grammar_table->rules[j]->symbols[0]->tval]->items[v];
					   	if(!item_is_in_set(citem,follows->array[grammar_table->rules[j]->symbols[i]->tval]))
						  	add_item_to_set(follows->array[grammar_table->rules[j]->symbols[i]->tval],citem);
				    	}
//				    }
				    break;
				}
				
			 }
		  
	   }

    }
//    printf("printing follows array now\n");
 //   print_lr0_array(follows);
    return follows;
}
void fill_action_table(struct parser_tables_s* ptable,gr_tbl_t* grammar_table, struct lr0_array_set* canon ){
    struct lr0_array_set* follows;
    
    follows = get_follow_set(canon,grammar_table);
    printf("working with this follow set\n");
	 print_lr0_array(follows);
    printf("working with this canon\n");
    print_lr0_array(canon);
    printf("filling action table\n");
    for(int i=0;i<canon->used;i++){
	   printf("set %d out of %zu\n",i,canon->used);
	   for(int j=0;j<canon->array[i]->used;j++){
		  printf("symbol %d out of %lu\n",j,canon->array[i]->used);
		  int rule = canon->array[i]->items[j]->rule;
		  int pos = canon->array[i]->items[j]->pos;
		  if(rule == 0 && pos ==2){
			 printf("found End of Start rule\n");
			 ptable->ACTION[i][ptable->num_terms-1].action = ACCEPT;
		  }
		  if(canon->array[i]->items[j]->symbol->val != EMPTY){
			 printf("symbol is not empty\n");
/*		  if(!(canon->array[i]->items[j]->symbol->val == EMPTY
			&& canon->array[i]->items[j]->pos >= grammar_table->rules[rule]->used) )*/
			 	size_t tok_val = grammar_table->rules[rule]->symbols[pos]->tval;
			 	int tok_type = grammar_table->tokens[tok_val].type;
			 	if(tok_type == TERMINAL){
				    struct lr0_set* gt_set;
			 		printf("found terminal %s in state set\n",grammar_table->rules[rule]->symbols[pos]->name);
				    gt_set = GOTO(canon->array[i],tok_val,universe_set, grammar_table);
				    int in_ra = set_is_in_array(gt_set,canon);
				    if(in_ra != -1){
					   if(ptable->ACTION[i][grammar_table->tokens[tok_val].termnum].action == AERROR){
						  	printf("setting ACTION table state %d, action %d\n",in_ra,SHIFT);
						  	ptable->ACTION[i][grammar_table->tokens[tok_val].termnum].state = in_ra;
						  	ptable->ACTION[i][grammar_table->tokens[tok_val].termnum].action = SHIFT;
					   }
					   else{
						  printf("possibly parsing confict, already at %d?\n",ptable->ACTION[i][grammar_table->tokens[tok_val].termnum].action);
						  exit(EXIT_FAILURE);
					   }
					   
				    }
				    else{
					   printf("error, found a goto set that we didn't find earlier??\n");
					   exit(EXIT_FAILURE);
				    }
		  		}
		  }
		  else{
			 if(pos == grammar_table->rules[rule]->used){
				printf("found the end of rule empty string\n");
				size_t tok_index = grammar_table->rules[rule]->symbols[0]->tval;
				if(grammar_table->rules[rule]->symbols[0]->tval !=0){
				    for(int b=0;b<follows->array[tok_index]->used;b++){
					   	printf("going through follow array of %s, symbol %d\n",grammar_table->rules[rule]->symbols[0]->name,b);
					   	if(follows->array[tok_index]->items[b]->pos != DOLLAR){
						    if(ptable->ACTION[i][follows->array[tok_index]->items[b]->token->termnum].action == AERROR){
//							   char y;
//							   scanf("%d",&y);
//							   int token = tok_exists(grammar_table->tokens,"",grammar_table);
							   printf("setting ACTION table to REDUCE %d by rule %d to state %zu num symbols to pop %lu\n",REDUCE,rule,grammar_table->rules[rule]->symbols[0]->tval,grammar_table->rules[rule]->used-1 );
							   	ptable->ACTION[i][follows->array[tok_index]->items[b]->token->termnum].action = REDUCE;
//							   	ptable->ACTION[i][follows->array[tok_index]->items[b]->token->termnum].state = grammar_table->rules[rule]->symbols[0]->tval;
							   ptable->ACTION[i][follows->array[tok_index]->items[b]->token->termnum].state = rule;
							   ptable->ACTION[i][follows->array[tok_index]->items[b]->token->termnum].rule = rule;
							   ptable->ACTION[i][follows->array[tok_index]->items[b]->token->termnum].numtoks = grammar_table->rules[rule]->used-1;
						    }
						    else{
							   printf("possibly parsing confict?\n");
							   exit(EXIT_FAILURE);
						    }
				    		}
				    		else{
						    printf("found DOLLAR SIGN\n");
						    if(ptable->ACTION[i][ptable->num_terms-1].action == AERROR){
							   	printf("setting ACTION table to REDUCE %d by rule %d to state %zu num symbols to pop %lu\n",REDUCE,rule,grammar_table->rules[rule]->symbols[0]->tval,grammar_table->rules[rule]->used-1 );
							   	ptable->ACTION[i][ptable->num_terms-1].action = REDUCE;
//							   	ptable->ACTION[i][ptable->num_terms-1].state = grammar_table->rules[rule]->symbols[0]->tval;
							   ptable->ACTION[i][ptable->num_terms-1].state = rule;
							   	ptable->ACTION[i][ptable->num_terms-1].rule = rule;
							   	ptable->ACTION[i][ptable->num_terms-1].numtoks = grammar_table->rules[rule]->used-1;
						    }
						    else{
							   printf("possibly parsing confict?\n");
							   exit(EXIT_FAILURE);
						    }
				    		}
				    }
				}
//				ptable->ACTION[i][tok_val].state = -1;
//				ptable->ACTION[i][tok_val].
			 }
		  }
	   }
    }
    struct lr0_set* gt_set;
    for(int m=0;m<grammar_table->tokused-1;m++){
	   printf("GOTO table construction with symbol %d\n",m);
	   if(grammar_table->tokens[m].type == NONTERMINAL){
		  printf("symbol is a nonterminal, continuing\n");
		  for(int h=0;h<canon->used;h++){
			 printf("going through states, on %d\n",h);
			 gt_set = GOTO(canon->array[h],grammar_table->tokens[m].tval,universe_set, grammar_table);
			 int in_ra = set_is_in_array(gt_set,canon);
			 if(in_ra != -1){
				size_t termval = grammar_table->tokens[m].termnum;
				printf("found GOTO before, which is good, so setting in GOTO table %d %zu to %d\n",h,termval,in_ra);
				ptable->GTTBL[h][termval] = in_ra;
			 }
		  }
	   }
    }
    printf("done with action and goto set, printing them now\n");
    print_action_n_goto_tbls(ptable);
}
void print_action_n_goto_tbls(struct parser_tables_s* ptable){
    if(!ptable || !ptable->ACTION || !ptable->GTTBL) return;
    if(ptable->num_terms == 0) return;
    for(int i=0;i<ptable->num_states;i++){
	   printf("%d|",i);
	   for(int j=0;j<ptable->num_terms;j++){
		  printf("%d%d ",ptable->ACTION[i][j].action,ptable->ACTION[i][j].state);
	   }
	   printf("|");
	   if(ptable->num_nonterms !=0){
		  for(int j=0;j<ptable->num_nonterms;j++){
			 printf("%d| ",ptable->GTTBL[i][j]);
		  }
	   }
	   printf("\n");
    }
}
