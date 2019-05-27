#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "parser.h"
#include "spcread.h"
#include "tables.h"
#include "functions.h"
#include "lr0b.h"
#define next_token() current_tok = tokenize(specfile)
#ifdef FLEX
extern char* yytext;
#define cur_text yytext
#else
extern char sctext[];
#define cur_text sctext
#endif

#define first_rule_length 2

#define ini_tok_name(name,message,...) \
 	slen = sizeof(char)*(strlen(name)__VA_ARGS__); \
	temp_tok_nam = malloc(slen); \
    	if(!temp_tok_nam){ \
		perror(message); \
		exit(EXIT_FAILURE); \
	} \
	memset(temp_tok_nam,0,slen); \
	strncpy(temp_tok_nam,name,slen)

#define process_n_set_tval(tokid,type,pos) \
	new_tok_val = process_token(tokid,temp_tok_nam,type,pos,grammar_table); \
	set_tok_tval(get_tok_by_id(grammar_table->tokens,new_tok_val),new_tok_val)

#define create_token(name,tokid,type,pos) 	\
	ini_tok_name(name,"create token",+1); \
	process_n_set_tval(tokid,type,pos)

#define create_token_minus_1(name,tokid,type,pos)  \
	ini_tok_name(name,"create token m1"); \
	temp_tok_nam[strlen(name)-1] ='\0'; \
	process_n_set_tval(tokid,type,pos)

#define create_code_token() \
			char* codenum; \
			codenum = malloc(sizeof(char)*(3)); \
			sprintf(codenum,"%zu",num_code_sects); \
			num_code_sects++; \
			temp_tok_nam = malloc(sizeof(char)*strlen(codenum)+3); \
			sprintf(temp_tok_nam,"cs%s",codenum); \
			free(codenum); \
			codenum=NULL; \
			process_n_set_tval(CODE,TERMINAL,1)

#define add_token_decls() 		tok_tbl_t * tokens; \
							char* temp_tok_nam; \
							tok_tab_t new_tok_val; \
							tokens = NULL; \
							temp_tok_nam = NULL; \
							tokens=grammar_table->tokens

#define add_rule_decls() 		rule_t* temp_rules; \
							temp_rules = NULL

#define gen_symb_to_rule \
							symb_t* temp_sym = create_symb(temp_tok_nam,new_tok_val);\
							set_symb_tval(temp_sym,new_tok_val); \
							add_symb_to_rule(temp_rules,temp_sym)


#define generate_rule(rnm,rln) \
							temp_rules = create_grrul(rnm,rln); \
							gen_symb_to_rule

#define run_g_rul_body() \
							generate_rule((char*)rl,INIT_GR_RULES_SIZE*2); \
							add_rule_to_table(grammar_table,temp_rules); \
							print_gr_table(grammar_table); \
							next_token(); \
							check_for_empty_rule(temp_rules);\
							rulebody(specfile,temp_rules); \
							optional_precision(specfile,temp_rules)
								

#define process_codeblock() \
							printf("found { start of code\n"); \
							while(current_tok != RCBRA && current_tok != MARK && \
								current_tok != EOF) \
										next_token(); \
							if(current_tok == RCBRA){ \
								printf("found } end of code\n"); \
								create_code_token(); \
							 	symb_t* temp_sym = create_symb(temp_tok_nam,new_tok_val);\
								set_symb_tval(temp_sym,new_tok_val); \
								add_symb_to_rule(temp_rules,temp_sym); \
								print_gr_table(grammar_table); \
							} \

const static char rs[] = "RealStart";
const static char rl[] = "rule";
gr_tbl_t *grammar_table;
size_t num_code_sects;
/* Function Definitions */
struct parser_tables_s* read_and_parse_specfile(FILE* specfile){
	struct lr0_array_set* canon;
    	struct parser_tables_s* ptables;
    	size_t slen;
	grammar_table = create_grtbl("",INIT_GR_TABLE_SIZE);
	add_token_decls();
	add_rule_decls();
	num_code_sects = 0;
	canon = NULL;

	printf("Parser starting..\n");
    	slen = sizeof(tok_tbl_t)*MAX_TOKS;

    	grammar_table->tokens = malloc(slen);
    	if(!grammar_table->tokens){
	   perror("in read and parse specfile creating tokens\n");
	   exit(EXIT_FAILURE);
    	}
    memset(grammar_table->tokens,0,slen);
    grammar_table->num_toks = MAX_TOKS;
    create_token("S'",REALSTART,NONTERMINAL,0);

#ifdef debug_print
    printf("newtokval:%d\n",new_tok_val);
#endif

    generate_rule((char*)rs,first_rule_length);
    add_rule_to_table(grammar_table,temp_rules);

#ifdef debug_print
	print_gr_table(grammar_table);
#endif
    create_token("{e}",EMPTY,TERMINAL,1);

#ifdef debug_print
    printf("newtokval:%d\n",new_tok_val);
#endif
	next_token();
	
	optional_definitions(specfile);
	/* get token MARK '%%' */
	if(current_tok == MARK)
		;
	else{
		printf("Expecting %%%% Marker\n");
		exit(EXIT_FAILURE);
	}
	next_token();
	grammar_rules(specfile);
    
	add_symb_to_rule(get_rul_by_pos(grammar_table,0),get_symb_by_pos(get_rul_by_pos(grammar_table,1),0));
    	calculate_num_terms(grammar_table);
    	canon = items(grammar_table);
    	ptables = create_action_n_goto_tbls(canon,grammar_table);
    	fill_action_table(ptables,grammar_table,canon);
	optional_auxillary(specfile);

    	printf("done reading specfile\n");
    	return ptables;
}

inline int optional_definitions(FILE* specfile){
	while(current_tok != MARK ){
	    /* input != MARK '%%' */
		definition(specfile);
		switch(current_tok){
		    case MARK:
			   break;
			case START:
			case TOKEN:
			case LEFT:
			case RIGHT:
			case TYPE:
			case NONASSOC:
				break;
			default:
				next_token();
				break;
		}
	}
	return RETVAL;
}
inline int optional_auxillary(FILE* specfile){
	switch(current_tok){
		   /* what is the input token value, switch on this */
		case EOF:
		   /* EOF  this is ok, we are done with the rules with no extra auxillary */
				break;
		case MARK:
		   /* MARK '%%' now read everything else as code to output
								to parser file to be included */
		   		break;
		default: 	printf("Expecting either EOF or optional %%%% Marker\n");
					/* error we aren't expecting anything besides the
							end of file or an auxillary unit */
				break;
	}
	return RETVAL;
}
int definition(FILE* specfile){
	add_token_decls();
    	size_t slen;
	int typ_tok,tag_tok;

	switch(current_tok){
		   /* input token value will be here*/
		case START: /* %start */ 
			next_token();
			if(current_tok != IDENT){
				printf("expecting Identifer after %%start\n");
				exit(EXIT_FAILURE);
			}
			printf("Definition: %%start %s\n",cur_text);
		   	create_token(cur_text,current_tok,NONTERMINAL,0);

#ifdef debug_print
		   	printf("newtokval:%d\n",new_tok_val);
#endif
			break;
		case UNION:
		   /* %union */
		   	printf("Definition: %%union\n");
			break;
		case LMARK:
		   /* '%{'  token, read all code looking for  token '%}'
					if find EOF or potentiall MARK '%%' then error occurs */
			next_token();
		   	printf("start of code\n");
		   	while(1){
			  switch(current_tok){
				 case RMARK:
				 case MARK:
				 case EOF:
					goto w1end;
					break;
				 default:
				    	next_token();
				    	break;
			  }
			 }
			 w1end:
#ifdef debug_print
			 switch(current_tok){
			  	case EOF:
				    printf("found EOF\n");
				    break;
			  	case MARK:
				    printf("found MARK\n");
				    break;
			  	default:
				    printf("end of code\n");
				    break;
			 }
#endif
			 break;
		default:
		   /* we found a grammar definition, continue */
			typ_tok = option_type(specfile);
			next_token();
			tag_tok = optional_tag(specfile);
			ident_list(specfile);
			break;
	}
	return RETVAL;
}
int option_type(FILE* specfile){
	switch(current_tok){
		   /* input token value will be here */
		case TOKEN: /* %token TOKEN */ 
			printf("Definition: %%token\n");
			break;
		case LEFT: /* %left LEFT token */ 
			printf("Definition: %%left\n");
			break;
		case RIGHT: /* %right RIGHT token */
			printf("Definition: %%right\n");
			 break;
		case NONASSOC: /* %nonassoc token */ 
			printf("Definition: %%nonassoc\n");
			break;
		case TYPE: /* %type token */ 
			printf("Definition: %%type\n");
			break;
		default:
		   	printf("not expecting this token %d\n", current_tok);
			exit(EXIT_FAILURE);/* error anything else */
			break;
	}
	return current_tok;
}
int optional_tag(FILE* specfile){
	if(current_tok == LESS){
	    /* if input token == '<' the start of an optional tag then we
				have presumably found something else */
		/* eat token '<' */
		/* read IDENTIFIER token or produce error */
		/* process .... */
		/* eat token '>' end of tag or error */
	    while(1){
		   switch(current_tok){
			  case GREAT:
			  case MARK:
			  case EOF:
				 goto tsend;
				 break;
			  default:
				 next_token();
				 break;
		   }
	    }
		tsend:
		if(current_tok == MARK){
			printf("found early %%%% Marker\n");
			exit(EXIT_FAILURE);
		}
		else if(current_tok == EOF){
			printf("found EOF, WHY?\n");
			exit(EXIT_FAILURE);
		}
	}
		return RETVAL;
}
int ident_list(FILE* specfile){
	while(current_tok != MARK){
	    /* replace with input token != MARK '%% */
combo:		identcombo(specfile);
		switch(current_tok){
			case UNION:
			case START:
			case TOKEN:
			case LEFT:
			case RIGHT:
			case TYPE:
			case NONASSOC:
			case IDENT:
			case MARK:
				break;
			default:
				next_token();
				break;
		}
		switch(current_tok){
			   /* input token value goes here */
			case COMMA: 
				break;
			   /* iput token == ',' comma , eat token
					 			continue loop */
				/* IDENTIFIER token, go back through loop */
			case MARK:
				printf("found a MARK, finish loop\n");
				continue;
			   break;
			   /* MARK '%%' token, finish loop */
			case UNION:
			case START:
			case TOKEN:
			case LEFT:
			case RIGHT:
			case TYPE:
			case NONASSOC:
				goto leaveloop;
				break;
			case IDENT:
				goto combo;
				break;
			default:
				break;
			   /* error otherwise */
		}
		if(current_tok != MARK) next_token();
	}
leaveloop:
	return RETVAL;
}
int identcombo(FILE* specfile){
	add_token_decls();
    size_t slen;

	switch(current_tok){
		   /* input token value goes here */
		case IDENT:
		   /* IDENTIFIER token */
		   create_token(cur_text,current_tok,TERMINAL,1);

#ifdef debug_print
		   printf("newtokval:%d\n",new_tok_val);
		   printf("temp_tok_nam: %s\n",temp_tok_nam);
		   printf("cur_text: %s\n",cur_text);
#endif

		   next_token();

		   if(current_tok == INTEGER){
			  /* next input token is NUMBER then */
			  size_t ndx = grammar_table->tokused-1;
			  tok_tbl_t* ttok = get_tok_by_id(grammar_table->tokens,ndx);
			  tok_tab_t tval = get_tok_val(ttok);

#ifdef debug_print
			  tok_tab_t* tnam = get_tok_nam(ttok);
			  printf("top token in tokens: %s\n",tnam);
			  printf("integer value of token given: %d\n",atoi(cur_text));
			  printf("last token before %s: %d\n",tnam,tval);
#endif
			  set_tok_val(ttok,atoi(cur_text));
			  tval = get_tok_val(ttok);
#ifdef debug_print
			  printf("last token after %s: %d\n",tnam,tval);
#endif
			}
			else if(current_tok == COMMA || current_tok == IDENT){
			    /* input token == ',' comma or == IDENTIFIER */

				printf("found either a comma or another ident\n");
			/* process as just IDENTIFIER and not IDENTIFER NUMBER */
			}
			else if(current_tok == MARK){ /* input token == MARK '%%' */
			;
			/* we only found a IDENTIFIER and not a IDENTIFIER NUMBER
				as above */
			}
			 /* error otherwise */
			break;
		default:
		   	printf("error found %d\n",current_tok);
			exit(EXIT_FAILURE);
			break; /* error, not expecting anthing else */
	}
	return RETVAL;
}
int grammar_rules(FILE* specfile){
/* to be changed later and semantically will be checked for first rule
	or not, this helps determine whether we accept things like '|'
	because we have an existing partial rule constructed. 
For now, the fun is done already outside the loop with no semantic check
POTIENTIAL - need for checking for the ';' semicolon token here but
	unsure until later
*/
	add_token_decls();
	add_rule_decls();
    	size_t slen;
	int first_rule_seen;
	first_rule_seen=0;

	while(current_tok != MARK && current_tok != EOF){
	    /* input token value == C_IDENTIFIER, repeat reading the
					grammar rule as in the previous iteration above */
		if(current_tok != C_IDENT){
		    /* input token == C_IDENTIFER otherwise error */
			printf("expecting C_IDENT, we got %d\n", current_tok);
			exit(EXIT_FAILURE);
		}
	    create_token_minus_1(cur_text,current_tok,NONTERMINAL,0);

#ifdef debug_print
	    printf("newtokval:%d\n",new_tok_val);
#endif
		run_g_rul_body();
		/* read the C_IDENTIFIER token and process */
		/* read ':' token or error , expecting nothing else */
		while(current_tok == ORSYMB){
		    /* input token == '|' then loop */
			/* read token '|' */
			run_g_rul_body();
		}
		first_rule_seen=1;
		
	}
	if(!first_rule_seen){
		printf("there has to be at least 1 rule\n");
		exit(EXIT_FAILURE);
	}
	return RETVAL;
}
int rulebody(FILE* specfile, rule_t* temp_rules){
	add_token_decls();
    	size_t slen;
	while(1){
	    /* input token != EOF, != MARK '%%', != PREC_TOKEN %prec, != ';' */
		switch(current_tok){
			   /* on input token value here */
			case EOF:
			case MARK:
			case PREC:
			case SEMICOLON:
			case C_IDENT:
			case ORSYMB:
				goto endrbody;
				break;
		    case RCBRA:
			   goto outside;
			   break;
			case IDENT:
			   /* IDENTIFIER */
			   create_token(cur_text,current_tok,TERMINAL,1);

#ifdef debug_print
			   printf("newtokval:%d\n",new_tok_val);
#endif
			   gen_symb_to_rule;
			   print_gr_table(grammar_table);
			   break;
			case LCBRA: 
			   process_codeblock()
			   /*  a ';' will cause error with next statements */
			   else if(current_tok == MARK)
			   /* attached to process_codeblock*/
				printf("found MARK\n");
			   else
				printf("found EOF\n");
				continue;
				break;
			   		/* '{' token then read and store all the next
							code found until matching '}', process any
							variables found in the code and change them
							accordingly. */
			default: 
				printf("unexpected token in rule body\n");
				printf("token: %d\n",current_tok);
				exit(EXIT_FAILURE);
				break; /* error, nothing else expected */
		}
	outside:
		if((current_tok != MARK) || (current_tok != EOF))
			next_token();
	}
	endrbody:
	return RETVAL;
}
int optional_precision(FILE* specfile,rule_t* temp_rules){
	add_token_decls();

	while(1){
	    /* input token != EOF, != MARK '%%' */
		switch(current_tok){
			   /* input token value replace here */
			case EOF:
			case MARK:
			case C_IDENT:
			case ORSYMB:
			case IDENT:
			case LCBRA:
				goto endop;
				break;
			case PREC:
				/* read prec token %prec */
				next_token();
				switch(current_tok){
					case SEMICOLON:
					   /* ';' semicolon */
					   printf("found semicolon\n");
					   break;
					case IDENT:
					/* read IDENTIFER token or error */
					   next_token();
					   if(current_tok == LCBRA){
							process_codeblock()
						  /*  a ';' will cause error with next statements */
							/* if input token == '{' we found code to process
							read code and output or store data */
							else if(current_tok == MARK)
							/* attached to process_codeblock*/
								;
							else
								;
						}
						default:
					   		break;
				}
						break;
			case SEMICOLON:
			   /* ';' semicolon */
			   	printf("found semicolon\n");
				break;
			default: 
				break; /* error? maybe, need to see later */
		}
		next_token();
	}
	endop:
	return RETVAL;
}

inline void check_for_empty_rule(rule_t* inrule){
    add_token_decls();
    add_rule_decls();
    size_t slen;
    switch(current_tok){
	   case ORSYMB:
	   case LCBRA:
	   case SEMICOLON:
		  printf("found an empty rule!! yay\n");
		  create_token("{e}",EMPTY,TERMINAL,1);

#ifdef debug_print
		  printf("newtokval:%d\n",new_tok_val);
#endif
		  gen_symb_to_rule;
		  break;
	   default:
		  break;
    }
}
