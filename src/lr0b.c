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
		    printf("checking j %d and used %zu\n",j,grammar_table->rules[i]->used);
			add_item_to_set(new_set,create_lr0_item(i,j,
				create_symb(grammar_table->tokens[1].name,1)));
		}
		print_lr0_set(new_set);
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
				printf("head value is: %d\n",head_val);
			}
			else{
				printf("dot before symbol: %d:%s\n",out_set->items[i]->symbol->val,
					out_set->items[i]->symbol->name);
				for (int j=1;j<universe->used;j++){ //each production B -+ y of G
					if(universe->items[j]->pos ==0 && 
						universe->items[j]->symbol->val==out_set->items[i]->symbol->val){
						printf("found a rule starting with: %s:\n",
						universe->items[j]->symbol->name);
						j++;
						if(!item_is_in_set(universe->items[j],out_set)){
							printf("item is not in set,adding...\n");
							add_item_to_set(out_set,universe->items[j]);
							made_additions=1;
						}
						else{
							printf("item already in set\n");
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
		printf("inside GOTO loop\n");
		printf("testing values %d and %d\n",in_token,inset->items[i]->symbol->val);
		if(inset->items[i]->pos!=0&&inset->items[i]->symbol->val == in_token){
			printf("value equals token value and trying to add to set\n");
			if(inset->items[i]->pos+1 < grammar_table->rules[inset->items[i]->rule]->used){
				add_item_to_set(out_set,create_lr0_item(inset->items[i]->rule,inset->items[i]->pos+1,
					grammar_table->rules[inset->items[i]->rule]->symbols[inset->items[i]->pos+1]));
				print_lr0_set(out_set);
			}
			else if(inset->items[i]->pos+1 == grammar_table->rules[inset->items[i]->rule]->used){
				add_item_to_set(out_set,create_lr0_item(inset->items[i]->rule,inset->items[i]->pos+1,
					create_symb(grammar_table->tokens[1].name,1)));
					print_lr0_set(out_set);
			}
		
		}
	}
	out_set = CLOSURE(out_set,universe_set,grammar_table);
	printf("so far the set we created is:\n");
	print_lr0_set(out_set);
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
	print_lr0_array(C);
//	got_set = GOTO(C->array[0],2,grammar_table);
//	clos_got_set = CLOSURE(got_set,universe_set,grammar_table);
//	C = CLOSURE()
/*	C = CLOSURE(of the set of only first rule, dot on far left of rhs); s' -> .s*/
	printf("tokens we are working with\n");
	print_tok_array(grammar_table->tokens,grammar_table->tokused);
/*	for(int r=0;r<grammar_table->tokused;r++){
		printf("name:%s, value:%d, token:%d\n",grammar_table->tokens[r].name,grammar_table->tokens[r].val,r);
	}
	*/
	while(made_changes){
		printf("in while loop\n");
		made_changes=0;
		for (int i=0;i<C->used;i++){ //each set of items I in C )
			printf("in first loop\n");
			for (int j=1;j<grammar_table->tokused;j++){ //each grammar symbol X
				printf("in second loop\n");
				got_set = GOTO(C->array[i],j,universe_set,grammar_table);
				if (got_set->used != 0){ //GOTO(I,X ) is not empty and not in C
					printf("goto(i,x) is not empty\n");
					if(!set_is_in_array(got_set,C)){
						add_set_to_array(C,got_set);
						made_changes=1;
					}
				}
			}
		}
	}
	print_lr0_array(C);
	return C;
}

int does_tok_start_rul(tok_tbl_t* token, gr_tbl_t* in_gr_table){
	if(!token || !in_gr_table) return 0;
	for(int i=0;i<in_gr_table->used;i++){
		if(in_gr_table->rules[i]->symbols[0]->val == token->val){
			return 1;
		}
	}
	return 0;
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
		printf("found terminal\n");
		outset = create_lr0_set(1);
		add_item_to_set(outset,create_lr0_item(token->val,token->val,NULL));
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
				printf("starting first loop\n");
				if(in_gr_table->rules[i]->symbols[0]->val == token->val){
					printf("found rule starting with token\n");
					if(rule_goes_to_empty(i,in_gr_table)){
						printf("rule went to empty so adding to set if not already\n");
						if(!item_is_in_set(create_lr0_item(EMPTY,EMPTY,NULL),outset))
							add_item_to_set(outset,create_lr0_item(EMPTY,EMPTY,NULL));
					}
					for(int j=1;j<in_gr_table->rules[i]->used;j++){
						printf("starting second loop\n");
						tok_tbl_t* ttok = malloc(sizeof(tok_tbl_t));
						ttok->name = in_gr_table->rules[i]->symbols[j]->name;
						tok_ex=tok_exists(in_gr_table->tokens,ttok->name,in_gr_table);
						if(tok_ex){
							printf("found token\n");
							if(in_gr_table->tokens[tok_ex].val != token->val){
								printf("its ok, not an endless loop\n");
								temp_set = FIRST(&(in_gr_table->tokens[tok_ex]),universe,in_gr_table);
							if(item_is_in_set(create_lr0_item(in_gr_table->tokens[1].val,
								in_gr_table->tokens[1].val,NULL),temp_set)){
									printf("ugh\n");
									for(int r=0;r<temp_set->used;r++){
									if(!item_is_in_set(temp_set->items[r],outset))
										add_item_to_set(outset,temp_set->items[r]);
									}
								}
								else{
									printf("not ugh\n");
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
		print_lr0_set(outset);
		return outset;
	}
	
	
}
struct lr0_set* FOLLOW(struct lr0_set* inset, struct lr0_set* universe, gr_tbl_t* in_gr_table){
    return NULL;
}

void get_first_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table){
	struct lr0_set* temp;
	for(int i=2;i<grammar_table->tokused;i++){
		printf("getting FIRST for token %d\n",i);
		temp = FIRST(&(grammar_table->tokens[i]),universe_set,grammar_table);
		printf("adding set to array\n");
		add_set_to_array(canon,temp);
		//print_lr0_set(temp);
	}
}
void get_follow_set(struct lr0_array_set* canon,gr_tbl_t* grammar_table){
	
}
