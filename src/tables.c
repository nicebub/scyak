#include <string.h>
#include "stdio.h"
#include "tables.h"
#include <stdlib.h>

symb_t * create_symb(char* name,tok_tab_t val){
	symb_t* temp;
	temp = malloc(sizeof(symb_t));
	temp->name = name;
	temp->val = val;
	temp->num_attribs = 0;
	temp->attribs = NULL;
	return temp;
}

symb_t* copy_symb(symb_t* in_symb){
	symb_t* out_symb;
	if(!in_symb) return NULL;
	out_symb = malloc(sizeof(symb_t));
	out_symb->name = malloc(sizeof(char)*strlen(in_symb->name)+1);
	strncpy(out_symb->name,in_symb->name,strlen(in_symb->name)+1);
	out_symb->val = in_symb->val;
	out_symb->num_attribs = in_symb->num_attribs;
//	out_symb->attribs = copy_attribs(in_symb->attribs);
	return out_symb;
}

rule_t * create_grrul(char* name, size_t size){
	rule_t* temp;
	temp = malloc(sizeof(rule_t));
	temp->symbols = malloc(sizeof(symb_t*)*size);
	for(int i=0;i<size;i++)
		temp->symbols[i] = NULL;
	temp->used = 0;
	temp->num_symbs = size;
	return temp;
	
}

void add_symb_to_rule(rule_t* rule,symb_t * symbol){
	if(rule && symbol){
		if(rule->used == rule->num_symbs-1){
			printf("need more memory for rules, extdending if possible\n");
			rule->symbols = realloc(rule->symbols,sizeof(symb_t*)
				*(rule->num_symbs+20));
			rule->num_symbs += 20;
			for(size_t i=rule->used;i<rule->num_symbs;i++)
				rule->symbols[i] = NULL;
		}
		rule->symbols[rule->used] = symbol;
		rule->used++;
	}
}
void print_tok(tok_tbl_t* intok){
	if(!intok){
		printf("NULL token\n");
		return;
	}
	printf("Token %s: val:%d type: %d\n",intok->name,intok->val,intok->type);
	return;
}
void print_tok_array(tok_tbl_t* inarray,size_t used){
	if(!inarray){
		printf("NULL array\n");
		return;
	}
	printf("Array:\n");
	for(int i=0;i<used;i++)
		print_tok(&(inarray[i]));
}


void print_symbol(symb_t * symbol){
	if(!symbol){
		printf("NULL SYMBOL\n");
		return;
	}
    printf("Symbol--Name:%s tok_val: %d attribs max: %zu \n",symbol->name,
				symbol->val,symbol->num_attribs);
	return;
}
void print_rule(rule_t* rule){
	if(!rule){
		printf("NULL RULE\n");
		return;
	}
    printf("Rule--#symbs used: %zu symb max: %zu\n",rule->used, rule->num_symbs);
	for(int i=0;i<rule->used;i++){
		printf("Symbol %d: ", i);
		print_symbol((rule->symbols[i]));
	}
}

void add_rule_to_table(gr_tbl_t * table, rule_t* rule){
	if(rule == NULL){ printf("rule is NULL\n");return; }
	if(table->used == table->num_ruls-1){
		printf("need more memory for rules, extdending if possible\n");
		table->rules = realloc(table->rules,sizeof(rule_t*)
			*(table->num_ruls+20));
		for(size_t i=table->used;i<table->num_ruls+20;i++)
			table->rules[i] = NULL;
		table->num_ruls += 20;
	}
	table->rules[table->used] = rule;
	table->used++;
}

gr_tbl_t * create_grtbl(char* name,size_t size){
	gr_tbl_t * table = malloc(sizeof(gr_tbl_t));
	table->rules = malloc(sizeof(rule_t*)*size);
	for(int i=0;i<size;i++)
		table->rules[i] = NULL;
	table->num_ruls = size;
	table->used = 0;
	table->tokused = 0;
	table->num_toks = 0;
	table->tokens = NULL;
	return table;
}
/*tok_tbl_t * create_tok_tbl(char * name,size_t size){
	
}*/
void print_gr_table(gr_tbl_t * table){
	if(table == NULL){ printf("Empty Table\n"); return;}
	for(int i=0;i<table->used;i++){
		printf("Rule %d: ", i);
		print_rule((table->rules[i]));
	}
}


#ifdef DEBUGA
int main(int argc, char const ** argv){
	create_dyn_tok_tbl(ttable,10);
	create_rules_array(mrules,30);
	create_attribute_array(mattribs,4);
//	create_tok_tbl(ttable,10);
	set_tok_name(ttable,0,"token");
	set_tok_val(ttable,0,'f');
	set_tok_name(ttable,1,"last");
	set_tok_val(ttable,1,'t');
	set_attribute_name(mattribs,0,"first");
	set_attribute_val(mattribs,0,'5');
	set_attribute_name(mattribs,1,"second");
	set_attribute_val(mattribs,1,'f');
	set_name(mrules,0,"hello");
	attach_attrib_array(mrules,0,mattribs);
	printf("mrules[0].name and attibs\n");
	printf("%s: [0]:%s, %c, [1]:%s, %c\n",get_rul_nam(mrules,0),
	get_attr_nam(mrules,0,0), get_attr_val(mrules,0,0),
	get_attr_nam(mrules,0,1), get_attr_val(mrules,0,1));
	attach_attrib_array(mrules,0,ttable);
	printf("mrules[0].name and attibs\n");
	printf("%s: [0]:%s, %c, [1]:%s, %c\n",get_rul_nam(mrules,0),
	get_attr_nam(mrules,0,0), get_attr_val(mrules,0,0),
	get_attr_nam(mrules,0,1), get_attr_val(mrules,0,1));
}
#endif
