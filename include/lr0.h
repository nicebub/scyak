#ifndef LR0_H
#define LR0_H

#include <stdlib.h> /* for uint8_t and size_t */
#include "tables.h"
/* lr(0) item consists of rule number and dot location in rule */
struct lr0_item
{
	uint8_t rule;
	uint8_t pos;
	symb_t* symbol;
};

/* set of above lr(0) items and how many allocated and used */
struct lr0_set
{
	struct lr0_item ** items; 
	size_t num_items;
	size_t used;
};


/* array or set of sets of lr(0) items and the allocated and number used */
struct lr0_array_set
{
	struct lr0_set ** array;
	size_t num_sets;
	size_t used;
};

struct lr0_item* create_lr0_item(uint8_t rule ,uint8_t pos,symb_t* symbol);
struct lr0_set* create_lr0_set(size_t size);
struct lr0_array_set* create_lr0_array(size_t size);

struct lr0_item* copy_lr0_item(struct lr0_item* in_item);
struct lr0_set* copy_lr0_set(struct lr0_set* in_set);

int items_are_equal(struct lr0_item* item1, struct lr0_item* item2);
int sets_are_equal(struct lr0_set* set1, struct lr0_set* set2);

void add_item_to_set(struct lr0_set* lrset, struct lr0_item* lritem);
void add_set_to_array(struct lr0_array_set* array, struct lr0_set* lrset);

int item_is_in_set(struct lr0_item* item, struct lr0_set* inset);
int set_is_in_array(struct lr0_set* inset, struct lr0_array_set* inarray);

void print_lr0_item(struct lr0_item * item);
void print_lr0_set(struct lr0_set* aset);
void print_lr0_array(struct lr0_array_set* array);

#endif