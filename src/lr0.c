#include <stdio.h>
#include "lr0.h"
#include "bitset.c"
struct lr0_item* create_lr0_item(uint8_t rule ,uint8_t pos,symb_t* symbol){
	struct lr0_item* it;
	it = malloc(sizeof(struct lr0_item));
	it->rule = rule;
	it->pos = pos;
	it->symbol = symbol;
	return it;
}

struct lr0_item* copy_lr0_item(struct lr0_item* in_item){
	struct lr0_item* out_item;

	if(!in_item) return NULL;
	out_item = malloc(sizeof(struct lr0_item));
	out_item->rule = in_item->rule;
	out_item->pos = in_item->pos;
	out_item->symbol = copy_symb(in_item->symbol);
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
	iset->items = malloc(sizeof(struct lr0_item*)*size);
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
	out_set->items = malloc(sizeof(struct lr0_item*)*in_set->used);
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
	aset->array = malloc(sizeof(struct lr0_set*)*size);
	for(int i=0;i<size;i++)
		aset->array[i] = NULL;
	aset->num_sets = size;
	aset->used = 0;
	return aset;
}

void add_item_to_set(struct lr0_set* lrset, struct lr0_item* lritem){
	if(!lrset || !lritem){
		printf("one of these was NULL\n");
		return;
	}
	if(lrset->used >= lrset->num_items-1){
		lrset->items = realloc(lrset->items,sizeof(struct lr0_item*)*(lrset->num_items+10));
		for(int i=lrset->used;i<lrset->num_items+10;i++)
			lrset->items[i]=NULL;
		lrset->num_items += 10;
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
	if(!array || !lrset){
		printf("either array or set is NULL\n");
		return;
	}
	if(array->used >= array->num_sets-1){
		array->array = realloc(array->array,sizeof(struct lr0_set*)*(array->num_sets+10));
		for(int i=array->used;i<array->num_sets+10;i++)
			array->array[i]=NULL;
	}
	array->array[array->used] = lrset;
	array->used++;
}

int set_is_in_array(struct lr0_set* inset, struct lr0_array_set* inarray){
	for(int i=0;i<inarray->used;i++){
		printf("i:%d\n",i);
		if(sets_are_equal(inarray->array[i],inset))
			return 1;
	}
	return 0;
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


