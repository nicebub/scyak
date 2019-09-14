#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "spcread.h"
#include "tokens.h"
#define END -1
#ifndef FLEX
#define LEXER sclex(spec_file)
extern int sclex(FILE* spec_file);
#else
#define LEXER yylex()
extern char* yytext;
#define sclex_text yytext
#endif
//extern char sclex_text[];
/*
inline int tokenize(FILE* spec_file){
	int r;
	r = LEXER;
#ifdef debug_print
    printf("Lexer reporting token found: %d\n",r);
#endif
    return r;
}
*/
#define LEX_MX_STR_LEN 128
inline int tokenize(FILE* spec_file){
	   char * ident;
	   char * idtemp;
	   int rchar,pos;
	   idtemp = ident = NULL;
	   rchar = fgetc(spec_file);
	   //   printf("character %c\n",rchar);
    		pos = 0;
	   while(rchar != EOF){
		  switch(rchar){
			 case -1:
//				printf("found EOF or '$'\n");
				return EOF;
			 case '{':
//				printf("lcbra\n");
				return LCBRA;
			 case '}':
//				printf("rcbra\n");
				return RCBRA;
			 case ';':
//				printf("semicolon\n");
				return SEMICOLON;
			 case ',':
//				printf("comma\n");
				return COMMA;
			 case '<':
//				printf("less\n");
				return LESS;
			 case '>':
//				printf("great\n");
				return GREAT;
			 case ':':
//				printf("colon\n");
				return COLON;
			 case '|':
//				printf("orsymb\n");
				return ORSYMB;
			 case '%':
//				printf("found a%%\n");
				rchar = fgetc(spec_file);
				switch(rchar){
				    case '{':
//					   printf("lmark\n");
					   return LMARK;
				    case '}':
//					   printf("rmark\n");
					   return RMARK;
				    case '%':
//					   printf("mark\n");
					   return MARK;
					   
				}
				while(1){
				    switch(rchar){
					   case '\n':
					   case '\t':
					   case ' ' :
					   case '\f':
					   case '\r':
						  sclex_text[pos] = '\0';
						  goto out;
						  break;
					   default:
						  sclex_text[pos] = rchar;
						  pos++;
						  rchar = fgetc(spec_file);
						  break;
						  
				    }
				}
			 out:
				if(!strcmp(sclex_text,"start"))
				    return START;
				else	if(!strcmp(sclex_text,"union"))
				    return UNION;
			    	else if(!strcmp(sclex_text,"token"))
				   return TOKEN;
			   	else if(!strcmp(sclex_text,"left"))
				    return LEFT;
				else if(!strcmp(sclex_text,"right"))
				    return RIGHT;
				else if(!strcmp(sclex_text,"nonassoc"))
				    return NONASSOC;
				else	if(!strcmp(sclex_text,"type"))
				    return TYPE;
				else if(!strcmp(sclex_text,"prec"))
				    return PREC;
				else {
				    printf("invalid token %%%s\n",sclex_text);
				    exit(EXIT_FAILURE);
				}
				break;
			 case '\n':
			 case '\t':
			 case ' ' :
			 case '\r':
			 case '\f':
//				printf("found ws\n");
				break;
			 default:
				if(isalnum(rchar)|| rchar=='_' || rchar=='-'){
				    if(!isnumber(rchar) && rchar!='-'){
//					   printf("found id\n");
					   ungetc(rchar,spec_file);
					   idtemp = malloc(sizeof(char)*LEX_MX_STR_LEN);
					   memset(idtemp,0,sizeof(char)*LEX_MX_STR_LEN);
					   fscanf(spec_file,"%s",idtemp);
					   ident = malloc(sizeof(char)*strlen(idtemp)+1);
					   memset(ident,0,sizeof(char)*strlen(idtemp)+1);
					   strncpy(ident,idtemp,sizeof(char)*strlen(idtemp));
					   memset(sclex_text,0,sizeof(char)*LEX_MX_STR_LEN);
					   if(ident[strlen(idtemp)-1]==':'){
						  if(!strcmp("error:",ident)){
							 printf("Error: error token cannot start a grammar rule\n");
							 exit(EXIT_FAILURE);
						  }
//						  printf("found a C_IDENT %s\n",ident);
						  strncpy(sclex_text,ident,sizeof(char)*strlen(ident));
//						  printf("sc_text is %s\n",sclex_text);
						//  ungetc(':',spec_file);
						  return C_IDENT;
					   }
//					   printf("found an IDENT %s\n",sclex_text);
					   strncpy(sclex_text,ident,sizeof(char)*strlen(ident));
					   return IDENT;
				    }
				    else{
//					   printf("found a number\n");
					   ungetc(rchar,spec_file);
					   size_t int_hold = 0;
					   fscanf(spec_file,"%zu",&int_hold);
					   sprintf(sclex_text,"%zu",int_hold);
					   return INTEGER;
				    }
				}
				else{
				    printf("Character not allowed in the language %c\n",rchar);
				    exit(EXIT_FAILURE);
				}
		  }
		  rchar = fgetc(spec_file);
		  
	   }
	   return EOF;
}
