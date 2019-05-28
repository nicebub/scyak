#include "lr0b.h"
#include "lr0.h"
#include <stdio.h>
#include "bitset.c"
#include "functions.h"
#include "tokens.h"

struct lr0_set* universe_set;

#define add_to_universe(symbll)	\
	add_item_to_set(new_set,create_lr0_item(i,j,symbll))

#define add_to_goto(symbll)	\
	add_item_to_set(out_set,create_lr0_item(tem_rul,tem_pos+1,symbll))

#define add_to_items(rul,pss) \
	add_item_to_set(initial_set, \
	create_lr0_item(rul,pss,get_symb_by_pos(get_rul_by_pos(grammar_table,rul),pss)));

#define add_to_folls() \
	add_item_to_set(follows->array[0],create_lr0_item(DOLLAR,DOLLAR,NULL))

#define exists_or_add(item,set) \
	if(!item_is_in_set(item,set)) \
		add_item_to_set(set,item)


struct lr0_set* create_universe_lr0_set(gr_tbl_t* grammar_table){
	size_t num_of_symbs,rulusd;
	struct lr0_set * new_set;
	num_of_symbs = 0;
	if(grammar_table && grammar_table->rules){
		for(int i=0;i<grammar_table->used;i++){
			num_of_symbs += get_rul_by_pos(grammar_table,i)->used;
		}
		new_set = create_lr0_set(num_of_symbs);
		for(int i=0;i<grammar_table->used;i++){
		    	rule_t* brule;
			int j;
		    	brule = get_rul_by_pos(grammar_table,i);
		    	rulusd = brule->used;
			for(j=0;j<rulusd;j++){
			    add_to_universe(get_symb_by_pos(brule,j));
			}
#ifdef debug_print
		    printf("checking j %d and used %zu\n",j,rulusd);
#endif
		    add_to_universe(create_symb(get_tok_nam(get_tok_by_id(grammar_table->tokens,1)),1));
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
    	tok_tab_t atval;
	struct lr0_set* out_set;
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
		    struct lr0_item* titem;
		    titem = get_item_by_pos(out_set,i);
		    if(get_item_pos(titem)){
			    symb_t* tsymm;
			    tsymm = get_item_symb(titem);
			    atval = get_symb_val(tsymm);
#ifdef debug_print
				printf("dot before symbol: %d:%s\n",atval,get_symb_nam(tsymm));
#endif
			    for (int j=1;j<universe->used;j++){ //each production B -+ y of G
				   struct lr0_item* anitem;
				   symb_t* fsym;
				   anitem = get_item_by_pos(universe,j);
				   fsym = get_item_symb(anitem);
				   if(get_item_pos(anitem) == 0 && get_symb_val(fsym) == atval){
#ifdef debug_print
						printf("found a rule starting with: %s:\n",get_symb_nam(fsym));
#endif
					    j++;
					    anitem = get_item_by_pos(universe,j);
					    if(!item_is_in_set(anitem,out_set)){
#ifdef debug_print
						   printf("item is not in set,adding...\n");
#endif
						    	add_item_to_set(out_set,anitem);
							made_additions=1;
					    }
					}
				/*if ( B -+ .y is not in J )
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
    	tok_tab_t tmval;
	out_set = create_lr0_set(20);
	for(int i=0;i<inset->used;i++){
	    struct lr0_item* mitem;
	    uint8_t tem_pos;
	    mitem = get_item_by_pos(inset,i);
	    tmval = get_symb_val(get_item_symb(mitem));
#ifdef debug_print
		printf("inside GOTO loop\n");
		printf("testing values %d and %d\n",in_token,tmval);
#endif
	    tem_pos = get_item_pos(mitem);
	    if(tem_pos!=0&&tmval == in_token){
#ifdef debug_print
			printf("value equals token value and trying to add to set\n");
#endif
		   uint8_t tem_rul = get_item_rul(mitem);
		   rule_t* trul = get_rul_by_pos(grammar_table,tem_rul);
		   if(tem_pos+1 < trul->used){
			  add_to_goto(get_symb_by_pos(trul,tem_pos+1));
		   }
		   else if(tem_pos+1 == trul->used){
			  add_to_goto(create_symb(get_tok_nam(get_tok_by_id(grammar_table->tokens,1)),1));
		   }
#ifdef debug_print
		   print_lr0_set(out_set);
#endif

		}
	}
	out_set = CLOSURE(out_set,universe_set,grammar_table);
#ifdef debug_print
	printf("so far the set we created in GOTO function is:\n");
	print_lr0_set(out_set);
#endif
    return out_set;
}


struct lr0_array_set* items(gr_tbl_t* grammar_table) {
	struct lr0_array_set* C;
	struct lr0_set* initial_set;
	struct lr0_set* got_set;
    	int made_changes;
	C = create_lr0_array(20);
	made_changes = 1;
	universe_set = create_universe_lr0_set(grammar_table);
	initial_set = create_lr0_set(10);
    	add_to_items(0,0);
    	add_to_items(0,1);
	add_set_to_array(C,CLOSURE(initial_set,universe_set,grammar_table));
    /*	C = CLOSURE(of the set of only first rule, dot on far left of rhs); s' -> .s*/
#ifdef debug_print
	print_lr0_array(C);
	printf("tokens we are working with\n");
	print_tok_array(grammar_table->tokens,grammar_table->tokused);
#endif
	while(made_changes){
	    made_changes=0;
	    for (int i=0;i<C->used;i++){ //each set of items I in C )
		    for (int j=1;j<grammar_table->tokused;j++){ //each grammar symbol X
			   	got_set = GOTO(get_set_by_pos(C,i),j,universe_set,grammar_table);
				if (got_set->used != 0){ //GOTO(I,X ) is not empty and not in C
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
	return (get_symb_val(get_symb_by_pos(get_rul_by_pos(in_gr_table,rule),1))==EMPTY);
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
	    	struct lr0_item* out_item =create_lr0_item(get_tok_tval(token),get_tok_tval(token),NULL);
	    	set_item_tok(out_item,token);
		add_item_to_set(outset,out_item);
		return outset;
	}
	else{
		struct lr0_set* temp_set;
	    	struct lr0_item* etem;
	    	struct lr0_item* yitem;
	    	tok_tbl_t* ttok2;
	    	tok_tbl_t* ttok;
	    	tok_tab_t* tnam2;
	    	rule_t* trul2;
	    	int tok_ex;
	    	size_t tvl_tok;
	    	etem = create_lr0_item(EMPTY,EMPTY,NULL);
		outset = create_lr0_set(10);
		if(does_tok_start_rul(token,in_gr_table)){
			for(int i=1;i<in_gr_table->used;i++){
			    trul2 = get_rul_by_pos(in_gr_table,i);
			    tvl_tok = get_tok_tval(token);
			    if(get_symb_tval(get_symb_by_pos(trul2,0)) == tvl_tok){
				    #ifdef debug_print
				    printf("found rule %d starting with token %zu\n",i,tvl_tok);
				    #endif
				   if(rule_goes_to_empty(i,in_gr_table)){
						#ifdef debug_print
						printf("rule went to empty so adding to set if not already\n");
						#endif
					  exists_or_add(etem,outset);
				    }
				    for(int j=1;j<trul2->used;j++){
					   ttok = malloc(sizeof(tok_tbl_t));
					   tnam2 = get_symb_nam(get_symb_by_pos(trul2,j));
					   set_tok_nam(ttok,tnam2);
					   tok_ex=tok_exists(tnam2,in_gr_table);
					   if(tok_ex){
							#ifdef debug_print
							printf("found token\n");
						  	#endif
						    ttok2 = get_tok_by_id(in_gr_table->tokens,tok_ex);
						    if(get_tok_tval(ttok2) != tvl_tok){
								#ifdef debug_print
							   	printf("its ok, not an endless loop\n");
								#endif
							   	temp_set = FIRST(ttok2,universe,in_gr_table);
							   	for(int r=0;r<temp_set->used;r++){
								  yitem = get_item_by_pos(temp_set,r);
								  exists_or_add(yitem,outset);
							   	}
							   	if(item_is_in_set(etem,temp_set)){
								    if(j==get_rul_by_pos(in_gr_table,i)->used-1){
									   exists_or_add(etem,outset);
								    }
								}
								else{
								    break;
								}
						    }
						    else
							   	break;
					   }
				    }
			    }
			}
		}
		return outset;
	}
}

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
}

struct lr0_array_set* get_follow_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table){
    size_t symtvl,tsim_tvl,tr_tvl;
    struct lr0_set	*temp, *ftsim_set, *fset, *fset2;
    struct lr0_array_set* follows;
    struct lr0_item* citem, *witem, *mT;
    rule_t* temp_rul, *temprul;
    symb_t* tsim;
    tok_tbl_t* tem_tok;
    int first_had_empty,found_empty,should_still_look;
    follows = NULL;
    temp = ftsim_set = fset = fset2 = NULL;
    mT = create_lr0_item(EMPTY,EMPTY,NULL);
	#ifdef debug_print
    printf("initializing follow array\n");
	#endif
    follows = create_lr0_array(grammar_table->tokused);
    for(int a=0;a<grammar_table->tokused;a++)
	   set_array_set_by_pos(follows,a,create_lr0_set(1));
    follows->used = grammar_table->tokused;

	#ifdef debug_print
    printf("putting $ in follow of S'\n");
	#endif
    add_item_to_set(follows->array[0],create_lr0_item(DOLLAR,DOLLAR,NULL));

    for(int j=0;j<grammar_table->used;j++){
	   first_had_empty=0;
	   should_still_look=1;
	   found_empty=0;
	   #ifdef debug_print
	   printf("first loop rule %d\n",j);
	   #endif
	   temp_rul = get_rul_by_pos(grammar_table,j);
	   for(size_t i=1;i<temp_rul->used-1;i++){
		  #ifdef debug_print
		  printf("second loop symbol %zu\n",i);
		  #endif
		  tsim = get_symb_by_pos(temp_rul,i);
		  tsim_tvl = get_symb_tval(tsim);
		  if(get_tok_type(get_tok_by_id(grammar_table->tokens,tsim_tvl)) == TERMINAL){
			 #ifdef debug_print
			 printf("symbol to the left of this one is a terminal, move on\n");
			 #endif
			 continue;
		  }
		  first_had_empty = 0;
		  for(size_t g=i+1;g<=temp_rul->used-1;g++){
			temp = FIRST(get_tok_by_id(grammar_table->tokens,
				    get_symb_tval(get_symb_by_pos(temp_rul,g))),universe_set,grammar_table);
			#ifdef debug_print
			printf("here is the first set found and used for rule %d grammar symbol %zu\n",j,g);
		  	print_lr0_set(temp);
		  	printf("temp is now the FIRST set\n");
			#endif
			for(int v=0;v<temp->used;v++){
			    witem = get_item_by_pos(temp,v);
			    citem = copy_lr0_item(witem);
			    found_empty = items_are_equal(citem,mT);
			    ftsim_set = get_set_by_pos(follows,tsim_tvl);
			    if(!found_empty){
					#ifdef debug_print
				   printf("adding item %d to set which has %d\n",v,get_item_pos(witem));
					#endif
				   exists_or_add(citem,ftsim_set);
			    }
			    else
				   first_had_empty = 1;
			}
			if(!first_had_empty)
			    break;
		  }
	   }
    }
    for(int j=0;j<grammar_table->used;j++){
	   first_had_empty=1;
	   should_still_look=1;
	   found_empty=0;
	   #ifdef debug_print
	   printf("first loop rule %d\n",j);
	   #endif
	   temprul = get_rul_by_pos(grammar_table,j);
	   for(size_t i=temprul->used-1;i>=1;i--){
		  symtvl = get_symb_tval(get_symb_by_pos(temprul,i));
		  tem_tok = get_tok_by_id(grammar_table->tokens,symtvl);
		  if(get_tok_type(tem_tok) != TERMINAL){
				#ifdef debug_print
				printf("not a terminal in later for loop\n");
				#endif
			 	temp = FIRST(tem_tok,universe_set,grammar_table);
				#ifdef debug_print
				printf("here is the first set found and used for rule %d grammar symbol %zu\n",j,i);
				print_lr0_set(temp);
				#endif
			 	found_empty =item_is_in_set(mT,temp);
			 	tr_tvl = get_symb_tval(get_symb_by_pos(temprul,0));
			 	fset = get_set_by_pos(follows,tr_tvl);
			 	fset2 = get_set_by_pos(follows,symtvl);
			 	for(int v=0;v<fset->used;v++){
					#ifdef debug_print
				    printf("adding item %d to FOLLOW of head rule symbol which has %d\n",v,get_item_pos(get_item_by_pos(fset,v)));
					#endif
				    citem = get_item_by_pos(fset,v);
				    exists_or_add(citem,fset2);
				}
			 	if(!found_empty)
				    break;
			 	continue;
		  }
		  break;
	   }
    }
    return follows;
}
void fill_action_table(struct parser_tables_s* ptable,gr_tbl_t* grammar_table,struct lr0_array_set* canon)
{
    size_t tok_val,tok_type,ttnum,thetnum,tok_index,using_index,termval;
    struct lr0_array_set* follows;
    struct lr0_item* litem, *qitem;
    struct lr0_set* fset, *gt_set, *hset;
    symb_t* tsymt, *exsymb;
    rule_t* tem_rul;
    tok_tbl_t* tem_token, *rttok;
    int rule,pos,in_ra;

    follows = get_follow_set(canon,grammar_table);
    printf("working with this follow set\n");
    print_lr0_array(follows);
    printf("working with this canon\n");
    print_lr0_array(canon);
    printf("filling action table\n");
    for(int i=0;i<canon->used;i++){
	   printf("set %d out of %zu\n",i,canon->used);
	   fset = get_set_by_pos(canon,i);
	   for(int j=0;j<fset->used;j++){
		  printf("symbol %d out of %lu\n",j,fset->used);
		  litem = get_item_by_pos(fset,j);
		  rule = get_item_rul(litem);
		  pos = get_item_pos(litem);
		  tem_rul = get_rul_by_pos(grammar_table,rule);
		  if(rule == 0 && pos ==2){
			 printf("found End of Start rule\n");
			 ptable->ACTION[i][ptable->num_terms-1].action = ACCEPT;
		  }
		  if(get_symb_val(get_item_symb(litem)) != EMPTY){
			 printf("symbol is not empty\n");
			 tsymt = get_symb_by_pos(tem_rul,pos);
			 tok_val = get_symb_tval(tsymt);
			 tem_token = get_tok_by_id(grammar_table->tokens,tok_val);
			 tok_type = get_tok_type(tem_token);
			 if(tok_type == TERMINAL){
			 	printf("found terminal %s in state set\n",get_symb_nam(tsymt));
				gt_set = GOTO(fset,tok_val,universe_set, grammar_table);
				in_ra = set_is_in_array(gt_set,canon);
				ttnum = get_tok_termnum(tem_token);
				if(in_ra != -1){
				    if(ptable->ACTION[i][ttnum].action == AERROR){
						  	printf("setting ACTION table state %d, action %d\n",in_ra,SHIFT);
						  	ptable->ACTION[i][ttnum].state = in_ra;
						  	ptable->ACTION[i][ttnum].action = SHIFT;
					}
					else{
						  printf("possibly parsing confict, already at %d?\n",ptable->ACTION[i][ttnum].action);
					    switch(ptable->ACTION[i][ttnum].action){
						   case REDUCE:
							  printf("shift/reduce replacing reduce w/shift\n");
							  ptable->ACTION[i][ttnum].state = in_ra;
							  ptable->ACTION[i][ttnum].action = SHIFT;
							  break;
						   default:
							  printf("shift/shift no conflict\n");
							  break;
					    }
//						  exit(EXIT_FAILURE);
					}
				}
				else{
					   printf("error, found a goto set that we didn't find earlier??\n");
					   exit(EXIT_FAILURE);
				}
			 }
		  }
		  else{
			 struct lr0_set* fset;
	//		 exrul = get_rul_by_pos(grammar_table,rule);
			 exsymb = get_symb_by_pos(tem_rul,0);
			 
			 if(pos == tem_rul->used){
				printf("found the end of rule empty string\n");
				tok_index = get_symb_tval(exsymb);
				fset = get_set_by_pos(follows,tok_index);
				if(tok_index !=0){
				    for(int b=0;b<fset->used;b++){
					   	printf("going through follow array of %s, symbol %d\n",get_symb_nam(exsymb),b);
					   qitem = get_item_by_pos(fset,b);
					   thetnum = get_tok_termnum(get_item_tok(qitem));
					   	if(get_item_pos(qitem) != DOLLAR){
						    using_index = thetnum;
				    		}
				    		else{
						    printf("found DOLLAR SIGN\n");
						    using_index = ptable->num_terms-1;
				    		}
					   if(ptable->ACTION[i][using_index].action == AERROR){
						  printf("setting ACTION table to REDUCE %d by rule %d to state %zu num symbols to pop %lu\n",REDUCE,rule,tok_index,tem_rul->used-1);
						  ptable->ACTION[i][using_index].action = REDUCE;
						  ptable->ACTION[i][using_index].state = rule;
						  ptable->ACTION[i][using_index].rule = get_tok_termnum(get_tok_by_id(grammar_table->tokens,tok_index));
						  ptable->ACTION[i][using_index].numtoks = tem_rul->used-1;
					   }
					   else{
						  printf("possibly parsing confict?\n");
						  switch(ptable->ACTION[i][using_index].action){
							 case REDUCE:
								printf("reduce/reduce\n");
								if(ptable->ACTION[i][using_index].state > rule){
								    printf("replacing action from earlier rule\n");
								    ptable->ACTION[i][using_index].action = REDUCE;
								    ptable->ACTION[i][using_index].state = rule;
								    ptable->ACTION[i][using_index].rule = get_tok_termnum(get_tok_by_id(grammar_table->tokens,tok_index));
								    ptable->ACTION[i][using_index].numtoks = tem_rul->used-1;
								}
//								ptable->ACTION[i][using_index].state = in_ra;
//								ptable->ACTION[i][using_index].action = SHIFT;
								break;
							 default:
								printf("shift/reduce, keeping existing shift\n");
								break;
						  }
//						  exit(EXIT_FAILURE);
					   }

				    }
				}
			 }
		  }
	   }
    }
    for(int m=0;m<grammar_table->tokused-1;m++){
	   rttok = get_tok_by_id(grammar_table->tokens,m);
	   printf("GOTO table construction with symbol %d\n",m);
	   if(get_tok_type(rttok) == NONTERMINAL){
		  printf("symbol is a nonterminal, continuing\n");
		  for(int h=0;h<canon->used;h++){
			 hset = get_set_by_pos(canon,h);
			 printf("going through states, on %d\n",h);
			 gt_set = GOTO(hset,get_tok_tval(rttok),universe_set, grammar_table);
			 in_ra = set_is_in_array(gt_set,canon);
			 if(in_ra != -1){
				termval = get_tok_termnum(rttok);
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
