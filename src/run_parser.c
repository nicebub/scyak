#include <ctype.h>
#include "run_parser.h"
#include "lr0b.h"
#include "stdio.h"
#include "string.h"
#include "tokens.h"
char lex_text[128];
void run_parser(struct parser_tables_s* ptable){
    u_int8_t stack[100];
    memset(stack,0,sizeof(u_int8_t)*100);
    u_int8_t* top = &stack[0];
    char* file_to_parse;
    FILE* thefile;
    int tokn;
    int ntindex;
    file_to_parse = malloc(sizeof(char)*128);
    memset(file_to_parse,0,sizeof(char)*128);
    
    printf("input filename to parse\n");
    scanf("%s",file_to_parse);
   // thefile = fopen(file_to_parse,"r");
    if((thefile = fopen(file_to_parse,"r")) == NULL){
	   perror("Error");
	   exit(EXIT_FAILURE);
    }
    tokn = mlexer(thefile);
    printf("token %d\n",tokn);
    while(1){
	   switch(ptable->ACTION[*top][tokn].action){
		  case SHIFT:
			    printf("shifting\n");
			    top++;
			    *top = ptable->ACTION[*(top-1)][tokn].state;
			    tokn = mlexer(thefile);
			    break;
		  case REDUCE:
			    printf("reducing\n");
			    ntindex = ptable->ACTION[*(top)][tokn].rule;
			    printf("reduced by some production %d and head production %d num tokens %d\n",ptable->ACTION[*(top)][tokn].state,
					 ptable->ACTION[*(top)][tokn].rule,ptable->ACTION[*(top)][tokn].numtoks);
			    top = top - (ptable->ACTION[*top][tokn].numtoks-1);
			    *top = ptable->GTTBL[*(top-1)][ntindex];
			    break;
		  case ACCEPT:
			    printf("accepting\n");
			    goto pender;
			    break;
		  default:
			    printf("parsing error: found error as value in table\n");
			    exit(EXIT_FAILURE);
	   }
    }
pender:
    printf("ending parser\n");
    fclose(thefile);
    free(file_to_parse);
    thefile = NULL;
    file_to_parse = NULL;
}

int mlexer(FILE* thefile){
    int rchar;
    char * ident;
    char * idtemp;
    idtemp = ident = NULL;
    rchar = fgetc(thefile);
 //   printf("character %c\n",rchar);
    while(rchar != EOF){
	   switch(rchar){
		  case -1:
			 printf("found EOF or '$'\n");
			 return 5;
		  case '+':
			 printf("found +\n");
			 return 0;
			 break;
		  case '*':
			 printf("found *\n");
			 return 1;
			 break;
		  case '(':
			 printf("found (\n");
			 return 2;
			 break;
		  case ')':
			 printf("found )\n");
			 return 3;
			 break;
		  case '\n':
		  case '\t':
		  case ' ':
		  case '\r':
		  case '\f':
			 printf("found ws\n");
			 break;
		  default:
			 if(isalpha(rchar)|| rchar=='_'){
				printf("found id\n");
				ungetc(rchar,thefile);
				idtemp = malloc(sizeof(char)*128);
				memset(idtemp,0,sizeof(char)*128);
				fscanf(thefile,"%s",idtemp);
				ident = malloc(sizeof(char)*strlen(idtemp)+1);
				memset(ident,0,sizeof(char)*strlen(idtemp)+1);
				strncpy(ident,idtemp,sizeof(char)*strlen(idtemp));
				memset(lex_text,0,sizeof(char)*128);
				strncpy(lex_text,ident,sizeof(char)*strlen(ident));
				return 4;
			 }
			 else{
				printf("Character not allowed in the language %c\n",rchar);
				exit(EXIT_FAILURE);
			 }
	   }
	   rchar = fgetc(thefile);

    }
    return 5;
}
