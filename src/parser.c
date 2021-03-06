#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
 char sclex_text[];
#define cur_text sclex_text
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

#define add_ctok_decls() \
			char* codenum

#define create_code_token() \
			codenum = malloc(sizeof(char)*(3)); \
			sprintf(codenum,"%zu",num_code_sects); \
			num_code_sects++; \
			temp_tok_nam = malloc(sizeof(char)*strlen(codenum)+3); \
			sprintf(temp_tok_nam,"cs%s",codenum); \
			free(codenum); \
			codenum=NULL; \
			process_n_set_tval(CODE,NONTERMINAL,1)

#define add_token_decls() 		\
							char* temp_tok_nam; \
							size_t slen; \
							tok_tab_t new_tok_val; \
							temp_tok_nam = NULL; \
							symb_t* temp_sym; \
							temp_sym = NULL


#define add_rule_decls() 		rule_t* temp_rules; \
							temp_rules = NULL

#define gen_symb_to_rule \
							temp_sym = create_symb(temp_tok_nam,new_tok_val);\
							set_symb_tval(temp_sym,new_tok_val); \
							add_symb_to_rule(temp_rules,temp_sym)


#define generate_rule(rnm,rln) \
							temp_rules = create_grrul(rnm,rln); \
							gen_symb_to_rule

#define run_g_rul_body() \
							generate_rule((char*)rl,INIT_GR_RULES_SIZE*2); \
							add_rule_to_table(grammar_table,temp_rules); \
						/*	print_gr_table(grammar_table);*/ \
							next_token(); \
							check_for_empty_rule(temp_rules);\
							rulebody(specfile,temp_rules); \
							optional_precedence(specfile,temp_rules)

const static char rs[] = "RealStart";
const static char rl[] = "rule";
gr_tbl_t *grammar_table;
size_t num_code_sects;
extern  u_int8_t prec_count; /*  precedence counter defined in functions.c */
/* Function Definitions */
gr_tbl_t* read_and_parse_specfile(FILE* specfile){
	struct lr0_array_set* canon;
    	struct parser_tables_s* ptables;
  //  	size_t slen;
	grammar_table = create_grtbl("",INIT_GR_TABLE_SIZE);
	add_token_decls();
	add_rule_decls();
	num_code_sects = 0;
	canon = NULL;
    	prec_count = 1;

	printf("Parser starting..\n");
    	slen = sizeof(tok_tbl_t)*MAX_TOKS;
    	grammar_table->tokens = malloc(slen);
    	if(!grammar_table->tokens){
	   perror("in read and parse specfile creating tokens\n");
	   exit(EXIT_FAILURE);
    	}
    memset(grammar_table->tokens,0,slen);
    tok_tbl_t* btok;
    for(int a=0;a<MAX_TOKS;a++){
	   btok = get_tok_by_id(grammar_table->tokens,a);
	   set_tok_assoc(btok,-1);
	   set_tok_prec(btok,-1);
    }
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
	if(current_tok != MARK){
		printf("Expecting %%%% Marker\n");
		exit(EXIT_FAILURE);
	}
	next_token();
	grammar_rules(specfile);
    	canon = items(grammar_table);
    	ptables = create_action_n_goto_tbls(canon,grammar_table);
    	fill_action_table(ptables,grammar_table,canon);
	optional_auxillary(specfile);

    	printf("done reading specfile\n");
    grammar_table->ptables = ptables;
    	return grammar_table;
}

inline int optional_definitions(FILE* specfile){
	while(current_tok != MARK ){
	    /* input != MARK '%%' */
		definition(specfile);
		switch(current_tok){
		    	case MARK:
//			   break;
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
 //   char* aux;
    printf("auxillary workingn with %d %s\n",current_tok,cur_text);
	switch(current_tok){
		   /* what is the input token value, switch on this */
		case EOF:
		   /* EOF  this is ok, we are done with the rules with no extra auxillary */
				break;
		case MARK:
		   printf("found %%%% marker, reading aux section if there\n");
		 set_tbl_aux_code(grammar_table,read_aux_code_sect(specfile));
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
//    char* defcode;
	add_token_decls();
//    	size_t slen;
	int typ_tok,tag_tok;

	switch(current_tok){
		   /* input token value will be here*/
		case START: /* %start */ 
			next_token();
			if(current_tok != IDENT){
				printf("expecting Identifer after %%start\n");
				exit(EXIT_FAILURE);
			}
//			printf("Definition: %%start %s\n",cur_text);
		   	create_token(cur_text,current_tok,NONTERMINAL,0);

			#ifdef debug_print
		   	printf("newtokval:%d\n",new_tok_val);
			#endif
			break;
		case UNION:
		   /* %union */
//		   	printf("Definition: %%union\n");
			break;
		case LMARK:
		   set_tbl_def_code(grammar_table,read_def_code_sect(specfile));
		   break;
		default:
		   /* we found a grammar definition, continue */
			typ_tok = option_type(specfile);
			next_token();
			tag_tok = optional_tag(specfile);
			ident_list(typ_tok,specfile);
		   	prec_count++;
			break;
	}
	return RETVAL;
}
int option_type(FILE* specfile){
	switch(current_tok){
		   /* input token value will be here */
		case TOKEN: /* %token TOKEN */ 
//			printf("Definition: %%token\n");
			break;
		case LEFT: /* %left LEFT token */ 
//			printf("Definition: %%left\n");
			break;
		case RIGHT: /* %right RIGHT token */
//			printf("Definition: %%right\n");
			 break;
		case NONASSOC: /* %nonassoc token */ 
//			printf("Definition: %%nonassoc\n");
			break;
		case TYPE: /* %type token */ 
//			printf("Definition: %%type\n");
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
int ident_list(int typ_tok, FILE* specfile){
	while(current_tok != MARK){
	    /* replace with input token != MARK '%% */
combo:		identcombo(typ_tok,specfile);
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
//				printf("found a MARK, finish loop\n");
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
int identcombo(int typ_tok,FILE* specfile){
	add_token_decls();
 //   size_t slen;
    size_t ndx;
    tok_tab_t tval;
    tok_tbl_t* ttok;

	switch(current_tok){
		   /* input token value goes here */
		case IDENT:
		   /* IDENTIFIER token */
		   create_token(cur_text,current_tok,TERMINAL,1);
		   if(typ_tok != TYPE)
			  set_tok_prec(get_tok_by_id(grammar_table->tokens,new_tok_val),prec_count);
		   switch(typ_tok){
			  case TYPE:
			  case TOKEN:
				 break;
			  case LEFT:
			  case RIGHT:
			  case NONASSOC:
				 set_tok_assoc(get_tok_by_id(grammar_table->tokens,new_tok_val),typ_tok);
				 break;
		   }
#ifdef debug_print
		   printf("newtokval:%d\n",new_tok_val);
		   printf("temp_tok_nam: %s\n",temp_tok_nam);
		   printf("cur_text: %s\n",cur_text);
#endif

		   next_token();

		   if(current_tok == INTEGER){
			  /* next input token is NUMBER then */
			  if(atoi(cur_text) < 0){
				 printf("Error: Token values must be an integer 0 or greater\n");
				 printf("%s %d\n",get_tok_by_id(grammar_table->tokens,new_tok_val)->name,atoi(cur_text));
				 exit(EXIT_FAILURE);
			  }
			  ndx = grammar_table->tokused-1;
			  ttok = get_tok_by_id(grammar_table->tokens,ndx);
			  tval = get_tok_val(ttok);

#ifdef debug_print
			  tok_tab_t* tnam;
			  tnam = get_tok_nam(ttok);
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

//				printf("found either a comma or another ident\n");
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
    //	size_t slen;
	int first_rule_seen;
	first_rule_seen=0;

	while(current_tok != MARK && current_tok != EOF){
	    /* input token value == C_IDENTIFIER, repeat reading the
					grammar rule as in the previous iteration above */
		if(current_tok != C_IDENT){
		    /* input token == C_IDENTIFER otherwise error */
			printf("expecting C_IDENT, we got %d, %s\n", current_tok,cur_text);
			exit(EXIT_FAILURE);
		}
//	    create_token_minus_1(cur_text,current_tok,NONTERMINAL,0);
	    slen = sizeof(char)*(strlen(cur_text));
	    temp_tok_nam = malloc(slen);
	    if(!temp_tok_nam){
		   perror("couldn't create string here\n");
		   exit(EXIT_FAILURE);
	    }
	    memset(temp_tok_nam,0,slen);
	    strncpy(temp_tok_nam,cur_text,slen);
	    temp_tok_nam[strlen(cur_text)-1] ='\0';
	    process_n_set_tval(current_tok,NONTERMINAL,0);

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
    change_tbl_for_code_sects(specfile,grammar_table);
    add_symb_to_rule(get_rul_by_pos(grammar_table,0),get_symb_by_pos(get_rul_by_pos(grammar_table,1),0));
    calculate_num_terms(grammar_table);
    /* here add calculate rule prec and assoc */
    for(int g=0;g<grammar_table->used;g++)
	   finalize_rule_prec(get_rul_by_pos(grammar_table,g),grammar_table);
	return RETVAL;
}
int rulebody(FILE* specfile, rule_t* temp_rules){
	add_token_decls();
    	add_ctok_decls();
  //  	size_t slen;
    char* rul_code;
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
//			   print_gr_table(grammar_table);
			   break;
			case LCBRA:
//			   printf("found code in rulebody\n");
			   create_code_token();
			   rul_code = process_codeblock(specfile,temp_rules);
//			   printf("token value before new val %d\n",current_tok);
			   next_token();
			   if(current_tok!=RCBRA){
				  printf("expecting a  { to end the code section\n");
				  exit(EXIT_FAILURE);
			   }
			   next_token();
//			   printf("after code section, found token %d\n",current_tok);
			   switch(current_tok){
				  case ORSYMB:
				  case C_IDENT:
				  case SEMICOLON:
				  case MARK:
				  case EOF:
					 set_rul_code(temp_rules,rul_code);
					 set_rul_code_tok(temp_rules,grammar_table->tokused-1);
//					 printf("setting code for rule to %s\n",
//						   get_tok_nam(get_tok_by_id(grammar_table->tokens,
//											    get_rul_code_tok(temp_rules))));
					 break;
				  default:
					 gen_symb_to_rule;
					 set_symb_code(temp_sym ,rul_code);
//					 printf("setting code for symbol to %s\n",
//						   get_tok_nam(get_tok_by_id(grammar_table->tokens,get_symb_tval(temp_sym))));
					 break;
			   }
//			   current_tok = RCBRA;
			   /*  a ';' will cause error with next statements */
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
//	    printf("outside switch above outside label\n");
	outside:
//	    printf("outside switch below outside label\n");
		if((current_tok != MARK) || (current_tok != EOF))
			next_token();
	}
	endrbody:
  //  printf("outside while loop of rulebody\n");
	return RETVAL;
}
int optional_precedence(FILE* specfile,rule_t* temp_rules){
	add_token_decls();
    	add_ctok_decls();
    char* rul_code;
    	int found_tok;
    	u_int8_t tprec,tassoc;
    tok_tbl_t* tb_ptr;

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
//					   printf("found semicolon\n");
					   break;
					case IDENT:
					   found_tok = tok_exists(cur_text,grammar_table);
					   if(found_tok){
						  tb_ptr = get_tok_by_id(grammar_table->tokens,found_tok);
						  tprec = get_tok_prec(tb_ptr);
						  tassoc = get_tok_prec(tb_ptr);
						  set_rul_prec(temp_rules,tprec);
						  set_rul_assoc(temp_rules,tassoc);
					   }
					/* read IDENTIFER token or error */
					   next_token();
					   if(current_tok == LCBRA){
//						  printf("found code in optional precision\n");
						  create_code_token();
						  rul_code = process_codeblock(specfile,temp_rules);
						  set_rul_code(temp_rules,rul_code);
						  set_rul_code_tok(temp_rules,grammar_table->tokused-1);
//						  printf("setting code tok to %s\n",
//							    get_tok_nam(get_tok_by_id(grammar_table->tokens,
//								    get_rul_code_tok(temp_rules))));
					   }
				    default:
					   break;
				}
						break;
			case SEMICOLON:
			   /* ';' semicolon */
//			   	printf("found semicolon\n");
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
  //  size_t slen;
    switch(current_tok){
	   case ORSYMB:
	   case LCBRA:
	   case SEMICOLON:
//		  printf("found an empty rule!! yay\n");
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

char* read_def_code_sect(FILE* specfile){
    int c;
    char* defcode;
    char* code_head;
    char* fresh_code;
    size_t def_sz,def_used;
    defcode = malloc(sizeof(char)*128);
    memset(defcode,0,sizeof(char)*128);
    def_sz=128;
    def_used=0;
    code_head=defcode;
    c=fgetc(specfile);
    while(1){
	   switch(c){
		  case EOF:
			 printf("unexpected early termination of specification file\n");
			 exit(EXIT_FAILURE);
			 break;
		  case '%':
			 c=fgetc(specfile);
			 if(c=='}'){
//				printf("found RMARK\n");
				*code_head = '\0';
				goto outie;
			 }
			 else if(c=='%'){
				printf("found %%%% MARKer, missing %%} RMARK?\n");
				exit(EXIT_FAILURE);
			 }
			 ungetc(c,specfile);
			 c = '%';
//			 ungetc('%',specfile);
		  default:
			 *code_head = c;
			 code_head++;
			 def_used++;
			 if(def_used==def_sz){
//				printf("need more memory in definitions, realloc\n");
				defcode = realloc(defcode,sizeof(char)*(2*def_sz));
				if(!defcode){
				    perror("realloc definition code");
				    exit(EXIT_FAILURE);
				}
				code_head = &defcode[def_used-1];
				def_sz*=2;
			 }
			 c=fgetc(specfile);
			 break;
	   }
    }
outie:
    fresh_code = malloc(sizeof(char)*(strlen(defcode)+1));
    memset(fresh_code,0,sizeof(char)*(strlen(defcode)+1));
    strncpy(fresh_code,defcode,strlen(defcode));
    free(defcode);
    defcode = NULL;
    return fresh_code;
}



char* process_codeblock(FILE* specfile,rule_t* temp_rules){
//    add_token_decls();
//    add_rule_decls();
    size_t def_sz,def_used;
    char* defcode;
    char* code_head;
    char* fresh_code;
    int c,r,in_sin_quotes,in_dbl_quotes;
    defcode = malloc(sizeof(char)*128);
    memset(defcode,0,sizeof(char)*128);
    def_sz=128;
    def_used=0;
    in_sin_quotes=0;
    in_dbl_quotes=0;
    code_head=defcode;
    c=fgetc(specfile);
//    printf("found { start of code\n");
    while(1){
//	   printf("character working with %c\n",c);
	   switch(c){
		  case '$':
			 r = fgetc(specfile);
			 if(r =='$'){
				strcat(defcode,"vtop[0]");
				code_head += strlen("vtop[0]");
			 }
			 else if(isdigit(r)){
				uint cdnum = '\0';
				char rstr[10] = { '\0' };
				ungetc(r,specfile);
				fscanf(specfile,"%u",&cdnum);
				sprintf(rstr,"vtop[%u]",(cdnum-1));
				strcat(defcode,rstr);
				code_head += strlen(rstr);
			 }
			 else{
				ungetc(r,specfile);
				goto con_proc;
			 }
			 break;
		  case '%':
			 r = fgetc(specfile);
			 if(r == '%'){
				printf("found %%%% MARKer during code section\n");
				exit(EXIT_FAILURE);
			 }
			 ungetc(r,specfile);
		  case '\\':
	  con_proc:
			 char_to_code(&c,&code_head,&def_used,&def_sz,&defcode);
			 c=fgetc(specfile);
			 goto addchar;
			 break;
		  case '\'':
			 in_sin_quotes = ~in_sin_quotes;
			 goto addchar;
			 break;
		  case '\"':
			 in_dbl_quotes = ~in_dbl_quotes;
			 goto addchar;
		  case '}':
			 if(!in_sin_quotes&& !in_dbl_quotes){
//				printf("found } end of code\n");
				*code_head = '\0';
				fresh_code = malloc(sizeof(char)*(strlen(defcode)+1));
				memset(fresh_code,0,sizeof(char)*(strlen(defcode)+1));
				strncpy(fresh_code,defcode,strlen(defcode));
				free(defcode);
				ungetc(c,specfile);
				defcode = NULL;
//				printf("code found as block: \n");
//				printf("%s\n",fresh_code);
				return fresh_code;
			 }
			 else
				goto addchar;
		  case EOF:
			 printf("found EOF during code section\n");
			 exit(EXIT_FAILURE);
			 break;
		  default:
		  addchar:
			 char_to_code(&c,&code_head,&def_used,&def_sz,&defcode);
			 break;
	   }
	   c = fgetc(specfile);
    }
}
void char_to_code(int *c,char** code_head,size_t* def_used,size_t* def_sz,char** defcode){
    **code_head = *c;
    (*code_head)+=1;
    *(def_used)+=1;
    if(*def_used== *def_sz){
	   *defcode = realloc(*defcode,sizeof(char)*(2*(*def_sz)));
	   if(!(*defcode)){
		  perror("realloc definition code");
		  exit(EXIT_FAILURE);
	   }
	   *code_head = defcode[(*def_used)-1];
	   (*def_sz) *=2;
    }
}

void change_tbl_for_code_sects(FILE* specfile,gr_tbl_t* grammar_table){
    add_token_decls();
    add_rule_decls();
    rule_t* rule;
    symb_t* symbol;
    tok_tbl_t* token;
    tok_tbl_t* e_tok;
    e_tok = get_tok_by_id(grammar_table->tokens,tok_exists("{e}",grammar_table));
    for(int q=0;q<grammar_table->used;q++){
	   rule = get_rul_by_pos(grammar_table,q);
//	   printf("this rule %d\n",q);
	   for(int w=1;w<rule->used;w++){
		  symbol = get_symb_by_pos(rule,w);
		  token = get_tok_by_id(grammar_table->tokens,get_symb_tval(symbol));
//		  printf("working with this symbol %s\n",get_symb_nam(symbol));
		  if(get_tok_val(token) == CODE){
//			 printf("found code to make rule of\n");
//			 new_tok_val = get_symb_tval(symbol);
//			 temp_tok_nam = get_symb_nam(symbol);
			 create_token(get_tok_nam(token),get_tok_val(token),TERMINAL,0);
			 generate_rule((char*)rl,INIT_GR_RULES_SIZE*2);
			 add_rule_to_table(grammar_table,temp_rules);
			 create_token("{e}",EMPTY,TERMINAL,1);
			 gen_symb_to_rule;
			 set_rul_code(temp_rules,get_symb_code(symbol));
			 set_rul_code_tok(temp_rules,get_symb_tval(symbol));
//			 print_gr_table(grammar_table);
		  }
//		  else
//			 printf("not code to make rule of\n");
	   }
    }
}

char* read_aux_code_sect(FILE* specfile){
    char* defcode;
  //  char* temp_code;
    char* fresh_code;
    size_t def_sz,def_used;
    size_t file_pos;
    size_t file_count;
    def_sz = 128;
    def_used = 0;

    file_pos = ftell(specfile);
    fseek(specfile,0,SEEK_END);
    file_count = ftell(specfile) - file_pos;
//    printf("file_pos %d file_count %d specfile ftell %d\n",file_pos,file_count,ftell(specfile));
    fseek(specfile,-file_count,SEEK_END);
    defcode = malloc(sizeof(char)*(file_count+1));
//    t_char = malloc(sizeof(char)*101);
//    memset(aux,0,sizeof(*aux));
  //  memset(t_char,0,101);
   // fgets(t_char,0,specfile);


//    defcode = malloc(sizeof(char)*256);
    memset(defcode,0,sizeof(char)*(file_count+1));
//    temp_code = malloc(sizeof(char)*file_count);
 //   memset(temp_code,0,file_count);
    fread(defcode,sizeof(char),file_count,specfile);
    printf("found defcode\n%s\n",defcode);
//    fgets(temp_code,(int)file_count,specfile);
//    strcat(defcode,temp_code);
 //   def_used += file_count-1;
/*    printf("found, so far:\n%s\n",temp_code);
    while(*temp_code != EOF){
	   printf("reading aux\n");
	   if(def_used==def_sz){
		  printf("need to realloc for aux code\n");
		  defcode = realloc(defcode,sizeof(char)*(2*def_sz));
		  if(!defcode){
			 perror("realloc definition code");
			 exit(EXIT_FAILURE);
		  }
		  def_sz*=2;
	   }
	   memset(temp_code,0,file_count);
	   fgets(temp_code,(int)(def_sz-def_used),specfile);
	   printf("temp_code again\n%s\n",temp_code);
	   strcat(defcode,temp_code);
	   def_used += def_sz-def_used-1;
    }
    printf("while loop done\n");*/
    fresh_code = malloc(sizeof(char)*(strlen(defcode)+1));
    memset(fresh_code,0,sizeof(char)*(strlen(defcode)+1));
    strncpy(fresh_code,defcode,strlen(defcode));
    free(defcode);
    defcode = NULL;
//    free(temp_code);
  //  temp_code=NULL;
    return fresh_code;
}
