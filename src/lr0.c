#include <stdio.h>
#include "lr0.h"
#include "bitset.c"
struct lr0_item* create_lr0_item(uint8_t rule ,uint8_t pos,symb_t* symbol){
	struct lr0_item* it;
	it = malloc(sizeof(struct lr0_item));
    if(!it){
	   perror("in create lr0 item");
	   exit(EXIT_FAILURE);
    }
    memset(it,0,sizeof(struct lr0_item));
	it->rule = rule;
	it->pos = pos;
	it->symbol = symbol;
    	it->token = NULL;
	return it;
}

uint8_t get_item_rul(struct lr0_item* item){
    if(!item) return -1;
    return item->rule;
}
uint8_t get_item_pos(struct lr0_item* item){
    if(!item) return -1;
    return item->pos;
}
symb_t* get_item_symb(struct lr0_item* item){
    if(!item) return NULL;
    return item->symbol;
}
tok_tbl_t* get_item_tok(struct lr0_item* item){
    if(!item) return NULL;
    return item->token;
}

void set_item_rul(struct lr0_item* item,uint8_t rule){
    if(!item) return;
    item->rule = rule;
}
void set_item_pos(struct lr0_item* item, uint8_t pos){
    if(!item) return;
    item->pos = pos;
}
void set_item_symb(struct lr0_item* item,symb_t* symbol){
    if(!item) return;
    item->symbol = symbol;
}
void set_item_tok(struct lr0_item* item,tok_tbl_t* token){
    if(!item) return;
    item->token = token;
}
struct lr0_item* get_item_by_pos(struct lr0_set* set, size_t pos){
    if(!set) return NULL;
    if(!set->items || set->used <=0) return NULL;
    return set->items[pos];
}

struct lr0_item* copy_lr0_item(struct lr0_item* in_item){
	struct lr0_item* out_item;

	if(!in_item) return NULL;
	out_item = malloc(sizeof(struct lr0_item));
    if(!out_item){
	   perror("in copy lr0 item");
	   exit(EXIT_FAILURE);
    }
    	memset(out_item,0,sizeof(struct lr0_item));
	out_item->rule = in_item->rule;
	out_item->pos = in_item->pos;
	out_item->symbol = copy_symb(in_item->symbol);
    	out_item->token = in_item->token;
	return out_item;
}

int items_are_equal(struct lr0_item* item1, struct lr0_item* item2){
	if(!item1 || !item2) return 0;
	return (item1->pos!=item2->pos)?0:(item1->rule!=item2->rule)?0:1;
}

struct lr0_set* create_lr0_set(size_t size){
	struct lr0_set* iset;
	if (size == 0) return NULL;
	iset = malloc(sizeof(struct lr0_set));
    	if(!iset){
	   perror("in create lr0 set");
	   exit(EXIT_FAILURE);
    	}
    	memset(iset,0,sizeof(struct lr0_set));
	iset->items = malloc(sizeof(struct lr0_item*)*size);
    	if(!(iset->items)){
	   perror("in create lr0 set");
	   exit(EXIT_FAILURE);
    	}
    	memset(iset->items,0,sizeof(struct lr0_item*)*size);
	for(int i=0;i<size;i++)
		iset->items[i]=NULL;
	iset->num_items = size;
	iset->used = 0;
	return iset;
}

struct lr0_set* copy_lr0_set(struct lr0_set* in_set){
	struct lr0_set* out_set;
	if(!in_set) return NULL;
	out_set = malloc(sizeof(struct lr0_set));
    	if(!out_set){
	   perror("in copy lr0 set");
	   exit(EXIT_FAILURE);
    	}
    	memset(out_set,0,sizeof(struct lr0_set));
	out_set->items = malloc(sizeof(struct lr0_item*)*in_set->used);
    	if(!(out_set->items)){
	   perror("in copy lr0 set");
	   exit(EXIT_FAILURE);
    	}
    memset(out_set->items,0,sizeof(struct lr0_set*)*in_set->used);
	out_set->num_items = in_set->used;
	out_set->used = 0;
	for(int i=0;i<in_set->used;i++)
		add_item_to_set(out_set,copy_lr0_item(in_set->items[i]));
	return out_set;
}

struct lr0_array_set* create_lr0_array(size_t size){
	struct lr0_array_set* aset;
	if(size == 0) return NULL;
	aset = malloc(sizeof(struct lr0_array_set));
    if(!aset){
	   perror("in create lr0 array");
	   exit(EXIT_FAILURE);
    }
    memset(aset,0,sizeof(struct lr0_array_set));
	aset->array = malloc(sizeof(struct lr0_set*)*size);
    	if(!(aset->array)){
	   perror("in create lr0 array");
	   exit(EXIT_FAILURE);
    	}
    memset(aset->array,0,sizeof(struct lr0_set*)*size);
	for(int i=0;i<size;i++)
		aset->array[i] = NULL;
	aset->num_sets = size;
	aset->used = 0;
	return aset;
}

void add_item_to_set(struct lr0_set* lrset, struct lr0_item* lritem){
    size_t slen,rlen;
	if(!lrset || !lritem){
		printf("one of these was NULL\n");
		return;
	}
	if(lrset->used == lrset->num_items){
	    rlen = lrset->used+SET_INC_SIZE;
	    slen = sizeof(struct lr0_item*)*rlen;
	    lrset->items = realloc(lrset->items,slen);
	    if(!lrset->items){
		   perror("Realloc in add item to set:");
		   exit(EXIT_FAILURE);
	    }
	    for(size_t i=lrset->used;i<rlen;i++)
		   lrset->items[i]=NULL;
		lrset->num_items += SET_INC_SIZE;
	}
	lrset->items[lrset->used] = lritem;
	lrset->used++;
}
int item_is_in_set(struct lr0_item* item, struct lr0_set* inset){
	if(!item || !inset) return 0;
	for(int i=0;i<inset->used;i++){
		if(items_are_equal(item,inset->items[i]))
			return 1;
	}
	return 0;
}
void add_set_to_array(struct lr0_array_set* array, struct lr0_set* lrset){
    size_t slen,rlen;
	if(!array || !lrset){
		printf("either array or set is NULL\n");
		return;
	}
	if(array->used == array->num_sets){
	    printf("need more memory for array using realloc\n");
	    rlen = array->used+ARR_INC_SIZE;
	    slen = sizeof(struct lr0_set*)*rlen;
	    array->array = realloc(array->array,slen);
	    if(!array->array){
		   perror("Realloc in add set to array:");
		   exit(EXIT_FAILURE);
	    }
	    for(size_t i=array->used;i<rlen;i++)
		   array->array[i]=NULL;
	    array->num_sets += ARR_INC_SIZE;
	}
	array->array[array->used] = lrset;
	array->used++;
}

int set_is_in_array(struct lr0_set* inset, struct lr0_array_set* inarray){
	for(int i=0;i<inarray->used;i++){
		if(sets_are_equal(inarray->array[i],inset))
			return i;
	}
	return -1;
}
int sets_are_equal(struct lr0_set* set1, struct lr0_set* set2){
	int found_item;
	if(!set1 || !set2) return 0;
	if(set1->used != set2->used) return 0;
	for(int i=0;i<set1->used;i++){
		found_item=0;
		for(int j=0;j<set1->used;j++){
			if(items_are_equal(set1->items[i],set2->items[j])){
				found_item=1;
				break;
			}
		}
		if(!found_item)
			return 0;
	}
	return 1;
}


void print_lr0_item(struct lr0_item * item){
	if(!item){
		printf("NULL Item\n");
		return;
	}
	if(item->symbol)printf("Item:--Rule #: %s",item->symbol->name);
	printf(", Grammar Position in Rule %d: %d\n",item->rule,item->pos);
	
}
void print_lr0_set(struct lr0_set* aset){
	if(!aset){
		printf("NULL set\n");
		return;
	}
	printf("LR(0) Set:--\n");
	for(int i=0;i<aset->used;i++){
		print_lr0_item(aset->items[i]);
	}
}
void print_lr0_array(struct lr0_array_set* array){
	if(!array){
		printf("NULL set array\n");
		return;
	}
		for(int i=0;i<array->used;i++){
			print_lr0_set(array->array[i]);
		}
}


