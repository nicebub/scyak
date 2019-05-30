#include <string.h>
#include "stdio.h"
#include "tables.h"
#include <stdlib.h>

inline tok_tab_t* get_tok_nam(tok_tbl_t* token){
    if(!token) return NULL;
    return token->name;
}

inline tok_tab_t get_tok_val(tok_tbl_t* token){
    if(!token) return -1;
    return token->val;
}
inline size_t get_tok_type(tok_tbl_t* token){
    if(!token) return -1;
    return token->type;
}

inline void set_tok_val(tok_tbl_t* token, tok_tab_t val){
    if(!token) return;
    token->val = val;
}
inline size_t get_tok_tval(tok_tbl_t* token){
    if(!token) return -1;
    return token->tval;
}

inline void set_tok_type(tok_tbl_t* token, size_t type){
    if(!token) return;
    token->type = type;
}
inline size_t get_tok_termnum(tok_tbl_t* token){
    if(!token) return -1;
    return token->termnum;
}
inline void set_tok_termnum(tok_tbl_t* token, size_t term){
    if(!token) return;
    token->termnum = term;
}
inline void set_tok_nam(tok_tbl_t* token, tok_tab_t* name){
    if(!token) return;
    token->name = name;
}
tok_tbl_t* get_tok_by_id(tok_tbl_t* inarray, size_t id){
    if(!inarray) return NULL;
    return &(inarray[id]);
}
inline int8_t get_tok_assoc(tok_tbl_t* token){
    if(!token) return -1;
    return token->assoc;
}
inline int8_t get_tok_prec(tok_tbl_t* token){
    if(!token) return -1;
    return token->prec;
}
inline void set_tok_assoc(tok_tbl_t* token, int8_t ass){
    if(!token) return;
    token->assoc = ass;

}
inline void set_tok_prec(tok_tbl_t* token, int8_t prec){
    if(!token) return;
    token->prec = prec;
}

symb_t * create_symb(tok_tab_t* name,tok_tab_t val){
	symb_t* temp;
    size_t slen;
	temp = malloc(sizeof(symb_t));
    	if(!temp){
	   perror("in create symb");
	   exit(EXIT_FAILURE);
    	}
    memset(temp,0,sizeof(symb_t));
    slen = sizeof(char)*(strlen(name)+1);
	temp->name = malloc(slen);
    if(!(temp->name)){
	   perror("in create symb name");
	   exit(EXIT_FAILURE);
    }
    memset(temp->name,0,slen);
    strncpy(temp->name,name,slen-1);
	temp->val = val;
    	temp->tval = -1;
	temp->num_attribs = 0;
	temp->attribs = NULL;
	return temp;
}

symb_t* copy_symb(symb_t* in_symb){
	symb_t* out_symb;
    size_t slen;
	if(!in_symb) return NULL;
	out_symb = malloc(sizeof(symb_t));
    	if(!out_symb){
	   perror("in copy symb");
	   exit(EXIT_FAILURE);
    	}
    memset(out_symb,0,sizeof(symb_t));
    	slen = sizeof(char)*(strlen(in_symb->name)+1);
	out_symb->name = malloc(slen);
    	if(!(out_symb->name)){
	   perror("in copy symb name");
	   exit(EXIT_FAILURE);
    	}
    	memset(out_symb->name,0,slen);
	strncpy(out_symb->name,in_symb->name,slen-1);
	out_symb->val = in_symb->val;
	out_symb->num_attribs = in_symb->num_attribs;
    	out_symb->tval = in_symb->tval;
	return out_symb;
}
inline attr_t* get_symb_attrib(symb_t* symbol){
    if(!symbol) return NULL;
    return symbol->attribs;
}
inline tok_tab_t* get_symb_nam(symb_t* symbol){
    if(!symbol) return NULL;
    return symbol->name;
}
inline tok_tab_t get_symb_val(symb_t* symbol){
    if(!symbol) return -1;
    return symbol->val;
}
inline size_t get_symb_tval(symb_t* in_symb){
    if(!in_symb) return -1;
    return in_symb->tval;
}
inline size_t get_symb_num_attrib(symb_t* symbol){
    if(!symbol) return -1;
    return symbol->num_attribs;
}
inline void set_symb_attrib(symb_t* symbol, attr_t* attribs){
    if(!symbol) return;
    symbol->attribs = attribs;
}
inline void set_symb_nam(symb_t* symbol, tok_tab_t* name){
    if(!symbol) return;
    symbol->name = name;
}
inline void set_symb_val(symb_t* symbol, tok_tab_t val){
    if(!symbol) return;
    symbol->val = val;
}
int8_t get_rul_assoc(rule_t* rule){
    if(!rule) return -1;
    return rule->assoc;
}
int8_t get_rul_prec(rule_t* rule){
    if(!rule) return -1;
    return rule->prec;
}
void set_rul_assoc(rule_t* rule, int8_t ass){
    if(!rule) return;
    rule->assoc = ass;
}
void set_rul_prec(rule_t* rule, int8_t prec){
    if(!rule) return;
    rule->prec = prec;
}

void set_rul_code(rule_t* rule, char* code){
    if(!rule) return;
    rule->code = code;
}
char* get_rul_code(rule_t* rule){
    if(!rule) return NULL;
    return rule->code;
}
void set_rul_code_tok(rule_t* rule, size_t token){
    if(!rule) return;
    rule->code_tval = token;
}
size_t get_rul_code_tok(rule_t* rule){
    if(!rule) return -1;
    return rule->code_tval;
}


rule_t * create_grrul(tok_tab_t* name, size_t size){
	rule_t* temp;
    size_t slen;
	temp = malloc(sizeof(rule_t));
    if(!temp){
	   perror("in create grrul");
	   exit(EXIT_FAILURE);
    }
    memset(temp,0,sizeof(rule_t));
    slen = sizeof(symb_t*)*size;
	temp->symbols = malloc(slen);
    	if(!(temp->symbols)){
	   perror("in create grrul symbols");
	   exit(EXIT_FAILURE);
    	}
    memset(temp->symbols,0,slen);
	for(int i=0;i<size;i++)
		temp->symbols[i] = NULL;
	temp->used = 0;
	temp->num_symbs = size;
    	temp->assoc = -1;
    	temp->prec = -1;
    	temp->code = NULL;
    	temp->code_tval = -1;
	return temp;
	
}
inline void set_tok_tval(tok_tbl_t* in_token, size_t val){
    if(!in_token) return;
    in_token->tval = val;
}
inline void set_symb_tval(symb_t* intable, size_t val){
    if(!intable) return;
    intable->tval = val;
}
void add_symb_to_rule(rule_t* rule,symb_t * symbol){
    size_t slen,rlen;
	if(rule && symbol){
		if(rule->used == rule->num_symbs){
			printf("need more memory for symbols, extending if possible\n");
		    rlen = rule->used+RUL_INC_SIZE;
		    slen = sizeof(symb_t*)*(rlen);
		    rule->symbols = realloc(rule->symbols,slen);
		    if(!rule->symbols){
			   perror("Realloc in add symb to rule:");
			   exit(EXIT_FAILURE);
		    }
		    for(size_t i=rule->used;i<rlen;i++)
			   rule->symbols[i] = NULL;
			rule->num_symbs += RUL_INC_SIZE;
		}
		rule->symbols[rule->used] = symbol;
		rule->used++;
	}
}
symb_t* get_symb_by_pos(rule_t* rule, size_t pos){
    if(!rule) return NULL;
    if(!rule->symbols || rule->used <=0)
	   return NULL;
    return rule->symbols[pos];
}

void print_tok(tok_tbl_t* intok){
	if(!intok){
		printf("NULL token\n");
		return;
	}
    printf("Token %s: val:%d type: %zu prec:%d assoc:%d tval:%zu termnum:%zu\n",
		 get_tok_nam(intok),get_tok_val(intok),get_tok_type(intok),get_tok_prec(intok),get_tok_assoc(intok),get_tok_tval(intok),get_tok_termnum(intok));
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
    printf("Symbol--Name:%s tok_val: %d attribs max: %zu tval: %zu\n",symbol->name,
				symbol->val,symbol->num_attribs,symbol->tval);
	return;
}
void print_rule(rule_t* rule){
	if(!rule){
		printf("NULL RULE\n");
		return;
	}
    printf("Rule--#symbs used: %zu symb max: %zu prec:%d assoc:%d\n",
		 rule->used, rule->num_symbs,rule->prec,rule->assoc);
	for(int i=0;i<rule->used;i++){
		printf("Symbol %d: ", i);
		print_symbol((rule->symbols[i]));
	}
}

void add_rule_to_table(gr_tbl_t * table, rule_t* rule){
//    rule_t** newrules;
    size_t slen,rlen;
	if(rule == NULL){ printf("rule is NULL\n");return; }
	if(table->used == table->num_ruls){
		printf("need more memory for rules, extending if possible\n");
	    rlen = table->used+RUL_INC_SIZE;
	    slen = sizeof(rule_t*)*(rlen);
	    table->rules = realloc(table->rules,slen);
	    if(!table->rules){
		   perror("Realloc in add rule to table:");
		   exit(EXIT_FAILURE);
	    }
		for(size_t i=table->used;i<rlen;i++)
			table->rules[i] = NULL;
		table->num_ruls += RUL_INC_SIZE;
	}
	table->rules[table->used] = rule;
	table->used++;
}

gr_tbl_t * create_grtbl(tok_tab_t* name,size_t size){
	gr_tbl_t * table = malloc(sizeof(gr_tbl_t));
    	if(!table){
	   perror("in create grtbl");
	   exit(EXIT_FAILURE);
    	}
    	memset(table,0,sizeof(gr_tbl_t));
	table->rules = malloc(sizeof(rule_t*)*size);
    	if(!(table->rules)){
	   perror("in create grtbl table rules");
	   exit(EXIT_FAILURE);
    	}
    memset(table->rules,0,sizeof(rule_t*)*size);
	for(int i=0;i<size;i++)
		table->rules[i] = NULL;
	table->num_ruls = size;
	table->used = 0;
	table->tokused = 0;
	table->num_toks = 0;
    	table->num_terms = 0;
    	table->num_nonterms = 0;
	table->tokens = NULL;
	return table;
}
rule_t* get_rul_by_pos(gr_tbl_t* table, size_t pos){
    if(!table) return NULL;
    if(!table->rules || table->used <=0)
	   return NULL;
    return table->rules[pos];
}

void calculate_num_terms(gr_tbl_t * grammar_table){
    if(!grammar_table) return;
    for(int i=2;i<grammar_table->tokused;i++){
	   if(grammar_table->tokens[i].type == TERMINAL){
		  grammar_table->tokens[i].termnum = grammar_table->num_terms++;
		  printf("set terminal %s to term num %zu\n",grammar_table->tokens[i].name,grammar_table->tokens[i].termnum);
	   }
	   else{
		  grammar_table->tokens[i].termnum = grammar_table->num_nonterms++;
		  printf("set nonterminal %s to nonterm num %zu\n",grammar_table->tokens[i].name,grammar_table->tokens[i].termnum);
	   }
    }
    printf("IN this table, found %zu terminals and %zu nonterminals\n",grammar_table->num_terms,grammar_table->num_nonterms);
}

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
