#include "lr0b.h"
#include <stdio.h>
#include "bitset.c"
#include "functions.h"
#include "tokens.h"
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
			int j;
		    	rulusd = get_rul_by_pos(grammar_table,i)->used;
			for(j=0;j<rulusd;j++){
				add_item_to_set(new_set,create_lr0_item(i,j,
					   get_symb_by_pos(get_rul_by_pos(grammar_table,i),j)));
			    
			}
#ifdef debug_print
		    printf("checking j %d and used %zu\n",j,rulusd);
#endif
			add_item_to_set(new_set,create_lr0_item(i,j,
				create_symb(get_tok_nam(get_tok_by_id(grammar_table->tokens,1)),1)));
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
			if(get_item_pos(titem) == 0){
			    symb_t* tsymm;
			    tsymm = get_item_symb(titem);
				head_val = get_symb_val(tsymm);
#ifdef debug_print
			    printf("head value is: %d\n",head_val);
#endif
			}
			else{
			    symb_t* tsymm;
			    tok_tab_t atval;
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
					if(get_item_pos(anitem) == 0 &&
					   get_symb_val(fsym) == atval){
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
						else{
						    ;
#ifdef debug_print
						    printf("item already in set\n");
#endif
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
	    mitem = get_item_by_pos(inset,i);
	    tmval = get_symb_val(get_item_symb(mitem));
#ifdef debug_print
		printf("inside GOTO loop\n");
		printf("testing values %d and %d\n",in_token,tmval);
#endif
	    uint8_t tem_pos;
	    tem_pos = get_item_pos(mitem);
	    if(tem_pos!=0&&tmval == in_token){
#ifdef debug_print
			printf("value equals token value and trying to add to set\n");
#endif
		   uint8_t tem_rul = get_item_rul(mitem);
		   rule_t* trul = get_rul_by_pos(grammar_table,tem_rul);
		   if(tem_pos+1 < trul->used){
				add_item_to_set(out_set,create_lr0_item(tem_rul,tem_pos+1,
					get_symb_by_pos(trul,tem_pos+1)));
#ifdef debug_print
				print_lr0_set(out_set);
#endif
		   }
			else if(tem_pos+1 == trul->used){
				add_item_to_set(out_set,create_lr0_item(tem_rul,tem_pos+1,
					create_symb(get_tok_nam(get_tok_by_id(grammar_table->tokens,1)),1)));
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
	C = create_lr0_array(20);
	int made_changes;
	made_changes = 1;
	universe_set = create_universe_lr0_set(grammar_table);
	initial_set = create_lr0_set(10);
	add_item_to_set(initial_set,create_lr0_item(0,0,get_symb_by_pos(get_rul_by_pos(grammar_table,0),0)));
	add_item_to_set(initial_set,create_lr0_item(0,1,get_symb_by_pos(get_rul_by_pos(grammar_table,0),1)));
	add_set_to_array(C,CLOSURE(initial_set,universe_set,grammar_table));
#ifdef debug_print
	print_lr0_array(C);
#endif
/*	C = CLOSURE(of the set of only first rule, dot on far left of rhs); s' -> .s*/
#ifdef debug_print
	printf("tokens we are working with\n");
	print_tok_array(grammar_table->tokens,grammar_table->tokused);
#endif
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
		int tok_ex;
		outset = create_lr0_set(10);
		if(does_tok_start_rul(token,in_gr_table)){
			for(int i=1;i<in_gr_table->used;i++){
#ifdef debug_print
				printf("starting first loop\n");
#endif
			    if(get_symb_tval(get_symb_by_pos(get_rul_by_pos(in_gr_table,i),0))
				  == get_tok_tval(token)){
#ifdef debug_print
				    printf("found rule %d starting with token %zu\n",i,get_tok_tval(token));
#endif
				   if(rule_goes_to_empty(i,in_gr_table)){
#ifdef debug_print
						printf("rule went to empty so adding to set if not already\n");
#endif
					  if(!item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),outset))
							add_item_to_set(outset,create_lr0_item(EMPTY,EMPTY,NULL));
					}
				   rule_t* trul2;
				   trul2 = get_rul_by_pos(in_gr_table,i);
					for(int j=1;j<trul2->used;j++){
#ifdef debug_print
						printf("starting second loop\n");
#endif
					    tok_tbl_t* ttok = malloc(sizeof(tok_tbl_t));
					    tok_tab_t* tnam2 = get_symb_nam(get_symb_by_pos(trul2,j));
						set_tok_nam(ttok,tnam2);
						tok_ex=tok_exists(tnam2,in_gr_table);
						if(tok_ex){
#ifdef debug_print
							printf("found token\n");
#endif
						    tok_tbl_t* ttok2;
						    ttok2 = get_tok_by_id(in_gr_table->tokens,tok_ex);
						    if(get_tok_tval(ttok2) != get_tok_tval(token)){
#ifdef debug_print
							printf("its ok, not an endless loop\n");
#endif
							   temp_set = FIRST(ttok2,universe,in_gr_table);
							   size_t val_t;
							   val_t = get_tok_tval(get_tok_by_id(in_gr_table->tokens,1));
							if(item_is_in_set(create_lr0_item(val_t,val_t,NULL),temp_set)){
#ifdef debug_print
								printf("ugh\n");
#endif
							    struct lr0_item* yitem;
							    for(int r=0;r<temp_set->used;r++){
								   yitem = get_item_by_pos(temp_set,r);
									if(!item_is_in_set(yitem,outset))
										add_item_to_set(outset,yitem);
									}
							    if(j==get_rul_by_pos(in_gr_table,i)->used-1){
								   struct lr0_item* temit;
								   temit = create_lr0_item(EMPTY,EMPTY,NULL);
								   if(!item_is_in_set(temit,outset))
									  add_item_to_set(outset,temit);
							    }
							 }
							else{
#ifdef debug_print
									printf("not ugh\n");
#endif
								struct lr0_item* yitem;
								for(int r=0;r<temp_set->used;r++){
									   yitem = get_item_by_pos(temp_set,r);
										if(!item_is_in_set(yitem,outset))
										add_item_to_set(outset,yitem);
								}
								break;
							 }
						  }else break;
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
    struct lr0_set* temp = NULL;
    struct lr0_array_set* follows=NULL;
    int should_still_look;
    int first_had_empty;
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
	   struct lr0_item* citem;
	   rule_t* temp_rul;
	   temp_rul = get_rul_by_pos(grammar_table,j);
	   for(size_t i=1;i<temp_rul->used-1;i++){
#ifdef debug_print
		  printf("second loop symbol %zu\n",i);
#endif
		  if(get_tok_type(get_tok_by_id(grammar_table->tokens
				,get_symb_tval(get_symb_by_pos(temp_rul,i)))) == TERMINAL){
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
			    struct lr0_item* witem;
			    witem = get_item_by_pos(temp,v);
			    citem = copy_lr0_item(witem);
			    found_empty = items_are_equal(citem,create_lr0_item(EMPTY,EMPTY,NULL));
			    size_t atval;
			    atval = get_symb_tval(get_symb_by_pos(temp_rul,i));
			    if(!found_empty && !item_is_in_set(citem,follows->array[atval])){
#ifdef debug_print
				   printf("adding item %d to set which has %d\n",v,get_item_pos(witem));
#endif
				   add_item_to_set(follows->array[atval],citem);
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
    for(int j=0;j<grammar_table->used;j++){
	   first_had_empty=1;
	   should_still_look=1;
	   found_empty=0;
#ifdef debug_print
	   printf("first loop rule %d\n",j);
#endif
	   struct lr0_item* citem;
	   rule_t* temprul;
	   temprul = get_rul_by_pos(grammar_table,j);
	   for(size_t i=temprul->used-1;i>=1;i--){
		  tok_tbl_t* tem_tok;
		  size_t symtvl;
		  symtvl = get_symb_tval(get_symb_by_pos(temprul,i));
		  tem_tok = get_tok_by_id(grammar_table->tokens,symtvl);
		  if(get_tok_type(tem_tok) == TERMINAL){
#ifdef debug_print
			 printf("found a terminal, no more follow transfer from this rule\n");
#endif
			 break;
		  }
		  else{
#ifdef debug_print
				printf("not a terminal in later for loop\n");
#endif
			 temp = FIRST(tem_tok,universe_set,grammar_table);
#ifdef debug_print
				printf("here is the first set found and used for rule %d grammar symbol %zu\n",j,i);
				print_lr0_set(temp);
#endif
			 	found_empty =item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),temp);
				if(found_empty){
				    size_t tr_tvl;
				    tr_tvl = get_symb_tval(get_symb_by_pos(temprul,0));
				    for(int v=0;v<follows->array[tr_tvl]->used;v++){
#ifdef debug_print
					   	printf("adding item %d to FOLLOW of head rule symbol which has %d\n",v,get_item_pos(get_item_by_pos(follows->array[tr_tvl],v)));
#endif
					   	citem = get_item_by_pos(follows->array[tr_tvl],v);
					   if(!item_is_in_set(citem,follows->array[symtvl])){
#ifdef debug_print
					  printf("item wasn't in set, so really adding\n");
#endif
						  add_item_to_set(follows->array[symtvl],citem);
					   }
				    }
				}
				else{
#ifdef debug_print
					   	printf("not empty in set?\n");
#endif
				    size_t tr_tvl;
				    tr_tvl = get_symb_tval(get_symb_by_pos(temprul,0));
				    for(int v=0;v<follows->array[tr_tvl]->used;v++){
#ifdef debug_print
					    printf("adding item %d to FOLLOW of head rule symbol which has 	%d\n",v,get_item_pos(get_item_by_pos(follows->array[tr_tvl],v));
#endif
					   citem = get_item_by_pos(follows->array[tr_tvl],v);
					   	if(!item_is_in_set(citem,follows->array[symtvl]))
						  	add_item_to_set(follows->array[symtvl],citem);
				    	}
				    break;
				}
			 }
	   }

    }
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
		  struct lr0_item* litem;
		  litem = get_item_by_pos(canon->array[i],j);
		  int rule = get_item_rul(litem);
		  int pos = get_item_pos(litem);
		  if(rule == 0 && pos ==2){
			 printf("found End of Start rule\n");
			 ptable->ACTION[i][ptable->num_terms-1].action = ACCEPT;
		  }
		  if(get_symb_val(get_item_symb(litem)) != EMPTY){
			 printf("symbol is not empty\n");
			 symb_t* tsymt;
			 tsymt = get_symb_by_pos(get_rul_by_pos(grammar_table,rule),pos);
			 size_t tok_val = get_symb_tval(tsymt);
			 size_t tok_type = get_tok_type(get_tok_by_id(grammar_table->tokens,tok_val));
			 if(tok_type == TERMINAL){
				struct lr0_set* gt_set;
			 	printf("found terminal %s in state set\n",get_symb_nam(tsymt));
				gt_set = GOTO(canon->array[i],tok_val,universe_set, grammar_table);
				int in_ra = set_is_in_array(gt_set,canon);
				size_t ttnum;
				ttnum = get_tok_termnum(get_tok_by_id(grammar_table->tokens,tok_val));
				if(in_ra != -1){
				    if(ptable->ACTION[i][ttnum].action == AERROR){
						  	printf("setting ACTION table state %d, action %d\n",in_ra,SHIFT);
						  	ptable->ACTION[i][ttnum].state = in_ra;
						  	ptable->ACTION[i][ttnum].action = SHIFT;
					}
					else{
						  printf("possibly parsing confict, already at %d?\n",ptable->ACTION[i][ttnum].action);
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
			 rule_t* exrul;
			 symb_t* exsymb;
			 exrul = get_rul_by_pos(grammar_table,rule);
			 exsymb = get_symb_by_pos(exrul,0);
			 if(pos == exrul->used){
				printf("found the end of rule empty string\n");
				size_t tok_index = get_symb_tval(exsymb);
				if(tok_index !=0){
				    for(int b=0;b<follows->array[tok_index]->used;b++){
					   	printf("going through follow array of %s, symbol %d\n",get_symb_nam(exsymb),b);
					   struct lr0_item* qitem;
					   qitem = get_item_by_pos(follows->array[tok_index],b);
					   	if(get_item_pos(qitem) != DOLLAR){
						    size_t thetnum;
						    thetnum = get_tok_termnum(get_item_tok(qitem));
						    if(ptable->ACTION[i][thetnum].action == AERROR){
							   printf("setting ACTION table to REDUCE %d by rule %d to state %zu num symbols to pop %lu\n",REDUCE,rule,tok_index,exrul->used-1);
							   	ptable->ACTION[i][thetnum].action = REDUCE;
							   	ptable->ACTION[i][thetnum].state = rule;
							   	ptable->ACTION[i][thetnum].rule = get_tok_termnum(get_tok_by_id(grammar_table->tokens,tok_index));
							   ptable->ACTION[i][thetnum].numtoks = exrul->used-1;
						    }
						    else{
							   printf("possibly parsing confict?\n");
							   exit(EXIT_FAILURE);
						    }
				    		}
				    		else{
						    printf("found DOLLAR SIGN\n");
						    if(ptable->ACTION[i][ptable->num_terms-1].action == AERROR){
							   	printf("setting ACTION table to REDUCE %d by rule %d to state %zu num symbols to pop %lu\n",REDUCE,rule,tok_index,exrul->used-1);
							   	ptable->ACTION[i][ptable->num_terms-1].action = REDUCE;
							   	ptable->ACTION[i][ptable->num_terms-1].state = rule;
							   	ptable->ACTION[i][ptable->num_terms-1].rule = get_tok_termnum(get_tok_by_id(grammar_table->tokens,tok_index));
							   	ptable->ACTION[i][ptable->num_terms-1].numtoks = exrul->used-1;
						    }
						    else{
							   printf("possibly parsing confict?\n");
							   exit(EXIT_FAILURE);
						    }
				    		}
				    }
				}
			 }
		  }
	   }
    }
    struct lr0_set* gt_set;
    for(int m=0;m<grammar_table->tokused-1;m++){
	   tok_tbl_t* rttok;
	   rttok = get_tok_by_id(grammar_table->tokens,m);
	   printf("GOTO table construction with symbol %d\n",m);
	   if(get_tok_type(rttok) == NONTERMINAL){
		  printf("symbol is a nonterminal, continuing\n");
		  for(int h=0;h<canon->used;h++){
			 printf("going through states, on %d\n",h);
			 gt_set = GOTO(canon->array[h],get_tok_tval(rttok),universe_set, grammar_table);
			 int in_ra = set_is_in_array(gt_set,canon);
			 if(in_ra != -1){
				size_t termval = get_tok_termnum(rttok);
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
