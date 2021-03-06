/**
This is the main file of the parser generator. This wrapper will call 
	the entire stack of the program.  The main function here will later
	be called scyak as an executable.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "lr0b.h"
#include "spcread.h"
#include "parser.h"
//#include "outfile.in"
//#include "run_parser.h"

void version(void);
void usage(void);
void cleanup(FILE*);
void create_parser_file(gr_tbl_t* grammar_table);
void output_header(gr_tbl_t* grammar_table);
void process_outfile(FILE* outfile,char* code);
char* put_out_until_token(FILE* outfile,char* new_code);
void push_goto_tbl(struct parser_tables_s* ptable,FILE* outfile);
void push_action_tbl(struct parser_tables_s* ptable,FILE* outfile);
void push_code_sections(gr_tbl_t* grammar_table,FILE* outfile);
void push_aux(gr_tbl_t* grammar_table,FILE* outfile);
void push_trans_tbl(gr_tbl_t* grammar_table,FILE* outfile);
void push_term_defs(gr_tbl_t* grammar_table,FILE* outfile);
void push_def_sect(gr_tbl_t* grammar_table, FILE* outfile);

static int outputh;
int main(int argc, char ** argv){
	int nousage;
	char option;
	FILE* spec_file;
    	outputh=0;
	nousage=0;
	if(argc <2){
		usage();
		exit(EXIT_FAILURE);
	}
	else{
		while((option = getopt(argc,argv,"dhuvV")) != -1){
			switch(option){
				case 'v':	version();
						nousage=1;
							break;
				case 'h':
				case 'u':	usage();
							break;
			    case 'd':
				   		printf("output scy.tab.h\n");
				   		outputh=1;
				   			break;
				case '?':
				default:
							usage();
							
			}
		}
		argc -= optind;
		argv += optind;
	}
	
	/*
		open spec file
	*/
	if(!argv[0]){
		if(nousage) exit(EXIT_SUCCESS);
		usage();
		exit(EXIT_FAILURE);
	}
	spec_file = init(argc,argv);
	/*
		parse spec file
	*/
	#ifdef FLEX
	yyin = spec_file;
	#endif
     gr_tbl_t* grammar_table;
    	grammar_table = read_and_parse_specfile(spec_file);
    if(outputh)
	   output_header(grammar_table);
    	create_parser_file(grammar_table);
//    	printf("done running parser\n");
	/*
		construct table from spec file
	
	*/
	
	/*
	
		 output parser from spec file
	*/
	
	/*
		 clean up and check everything went well
	*/
	cleanup(spec_file);
	exit(EXIT_SUCCESS);
}
inline void cleanup(FILE* spec_file){
	if(spec_file) fclose(spec_file);	
}
void usage(void){
	printf("Usage:\n");
	printf("	scyak [-dhuvV] <parser specification file>\n\n");
	printf("General Options:\n");
    	printf("-d			Output definitions .h file\n");
    	printf("-h			Show this help info\n");
    	printf("-u			Show this usage info\n");
	printf("-v			Print the version\n");
	printf("-V			Verbose\n");
}

inline void version(void){
	printf("scyak version 0.000001\n");
}

void create_parser_file(gr_tbl_t* grammar_table){
    struct parser_tables_s* ptables;
    FILE* parse_code;
    FILE* outfile;
    char* new_code;
    char* temp_code;
    size_t fl_sz;
    ptables = grammar_table->ptables;
    if(!ptables){
	   printf("Unkown error: parse tables weren't created\n");
	   exit(EXIT_FAILURE);
    }
    parse_code = fopen("/Users/scotty/Programming/scyak/src/outfile.in","r");
    if(!parse_code){
	   perror("error finding outfile.in");
	   exit(EXIT_FAILURE);
    }
    fseek(parse_code,0,SEEK_END);
    fl_sz = ftell(parse_code);
    rewind(parse_code);
//    printf("file size %d\n",fl_sz);
    new_code = malloc(sizeof(char)*(fl_sz+1));
    temp_code = malloc(sizeof(char)*(201));
    memset(new_code,0,fl_sz+1);
    memset(temp_code,0,200);
    fgets(temp_code,200,parse_code);
    while(*temp_code != '\0'){
	   strcat(new_code,temp_code);
	   memset(temp_code,0,200);
	   fgets(temp_code,200,parse_code);
//	   printf("working with this temp_code %s\n",temp_code);
    }
//    printf("----------code file found------------------\n");
  //  printf("%s",new_code);
    //printf("----------code file found------------------\n");
    free(temp_code);
    temp_code = NULL;
    outfile = fopen("scy.tab.c","w");
    if(!outfile){
	   perror("couldn't open file for writing");
	   exit(EXIT_FAILURE);
    }
    //    process_outfile(outfile,new_code);
    char* c_ptr;
    char num[4];
    c_ptr = new_code;
    c_ptr = put_out_until_token(outfile,c_ptr);
    push_def_sect(grammar_table,outfile);
    if(outputh){
	   fprintf(outfile,"#include \"scy.tab.h\"\n");
    }
    else{
	   push_term_defs(grammar_table,outfile);
    }
    c_ptr = put_out_until_token(outfile,c_ptr);
    sscanf(num,"%zu",&ptables->num_states);
    fprintf(outfile,"%zu",ptables->num_states);
    c_ptr += strlen(num);
    c_ptr = put_out_until_token(outfile,c_ptr);
    sscanf(num,"%zu",&ptables->num_nonterms);
    fprintf(outfile,"%zu",ptables->num_nonterms);
    c_ptr += strlen(num);
    c_ptr = put_out_until_token(outfile,c_ptr);
    sscanf(num,"%zu",&ptables->num_terms);
    fprintf(outfile,"%zu",ptables->num_terms);
    c_ptr += strlen(num);

    c_ptr = put_out_until_token(outfile,c_ptr);
    push_trans_tbl(grammar_table,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
    push_goto_tbl(ptables,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
    push_action_tbl(ptables,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
    push_code_sections(grammar_table,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
    push_aux(grammar_table,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
//    if(c_ptr != NULL)printf("found a special token\n");
}
/*
void process_outfile(FILE* outfile,char* new_code){
    char* c_ptr;
    c_ptr = new_code;
    c_ptr = put_out_until_token(outfile,c_ptr);
    
//    if(c_ptr != NULL)printf("found a special token\n");
    
}*/
char* put_out_until_token(FILE* outfile,char* new_code){
    char* c_ptr;
    c_ptr = new_code;
    while(*c_ptr != '\0'){
	   switch(*c_ptr){
		  case '#':
			 c_ptr++;
			 if(*c_ptr == '0'){
				c_ptr++;
				if(*c_ptr == '#'){
				    c_ptr++;
				    return c_ptr;
				}
				else{
				    c_ptr -= 2;
				}
			 }
			 else{
				c_ptr--;
			 }
		  default:
			 fputc(*c_ptr,outfile);
			 c_ptr++;
			 break;
	   }
    }
    return NULL;

}
void push_def_sect(gr_tbl_t* grammar_table, FILE* outfile){
    if(grammar_table->def_code)
	   	fprintf(outfile,"%s",grammar_table->def_code);
}
void push_trans_tbl(gr_tbl_t* grammar_table,FILE* outfile){
    char* temp;
    char* t_ptr;
  //  int index;
    char num[4];
    temp = malloc(sizeof(char)*((grammar_table->num_terms+1)*3+3));
    memset(temp,0,sizeof(*temp));
    t_ptr = temp;
    *t_ptr = '{';
    t_ptr++;
    for(int y=0;y<grammar_table->num_terms;y++){
	   printf("checking term %d\n",y);
	   for(int i=2;i<grammar_table->tokused;i++){
		  printf("checking terminal against token %d\n",i);
		  if(get_tok_termnum(get_tok_by_id(grammar_table->tokens,i)) == y){
			 printf("termnum matches token\n");
			 sprintf(num,"%zu",get_tok_tval(get_tok_by_id(grammar_table->tokens,i)));
			 sprintf(t_ptr,"%zu",get_tok_tval(get_tok_by_id(grammar_table->tokens,i)));
			 t_ptr += strlen(num);
			 break;
		  }
		  printf("token doesn't match terminal, keep looking\n");
	   }
	   printf("adding comma or not\n");
	   if(y != grammar_table->num_terms){
		  printf("adding comma\n");
		  	*t_ptr = ',';
		  	t_ptr++;
	   }
    }
    sprintf(num,"%d",-1);
    sprintf(t_ptr,"%d",-1);
    t_ptr += strlen(num);

    *t_ptr = '}';
    t_ptr++;
    *t_ptr = '\0';
    char * new_str;
    new_str = malloc(sizeof(char)*(strlen(temp)+1));
    memset(new_str,0,strlen(temp)+1);
    strncpy(new_str,temp,sizeof(char)*strlen(temp));
    free(temp);
    temp = NULL;
    fputs(new_str,outfile);

}
void push_goto_tbl(struct parser_tables_s* ptable,FILE* outfile){
    char* temp;
    char* t_ptr;
    temp = malloc(sizeof(char)*(3+((ptable->num_states+1)*(ptable->num_nonterms+1)*4)));
    memset(temp,0,sizeof(*temp));
    t_ptr = temp;
    *t_ptr = '{';
    t_ptr++;
    for(int y=0;y<ptable->num_states;y++){
	   *t_ptr = '{';
	   t_ptr++;
	   for(int t=0;t<ptable->num_nonterms;t++){
		  char num[4];
		  sprintf(num,"%d",ptable->GTTBL[y][t]);
		  sprintf(t_ptr,"%d",ptable->GTTBL[y][t]);
		  t_ptr += strlen(num);
		  if(t != ptable->num_nonterms-1){
			 *t_ptr = ',';
			 t_ptr++;
		  }
	   }
	   *t_ptr = '}';
	   t_ptr++;
	   if(y != ptable->num_states-1){
		  *t_ptr = ',';
		  t_ptr++;
	   }
	   *t_ptr = '\n';
	   t_ptr++;
    }
    *t_ptr = '}';
    t_ptr++;
    *t_ptr = '\0';
    char * new_str;
//    printf("temp string so far\n%s\n",temp);
    new_str = malloc(sizeof(char)*(strlen(temp)+1));
    memset(new_str,0,strlen(temp)+1);
    strncpy(new_str,temp,sizeof(char)*strlen(temp));
    free(temp);
    temp = NULL;
//    printf("constructed GOTO table:\n %s\n",new_str);
    fputs(new_str,outfile);
}
void push_action_tbl(struct parser_tables_s* ptable,FILE* outfile){
    char* temp;
    char* t_ptr;
    temp = malloc(sizeof(char)*(3+((ptable->num_states+1)*(ptable->num_terms+1)*10)));
    memset(temp,0,sizeof(*temp));
    t_ptr = temp;
    *t_ptr = '{';
    t_ptr++;
    for(int y=0;y<ptable->num_states;y++){
	   *t_ptr = '{';
	   t_ptr++;
	   for(int t=0;t<ptable->num_terms;t++){
		  char num[4];
		  *t_ptr = '{';
		  t_ptr++;

		  sprintf(num,"%d",ptable->ACTION[y][t].state);
		  sprintf(t_ptr,"%d",ptable->ACTION[y][t].state);
		  t_ptr += strlen(num);
		  *t_ptr = ',';
		  t_ptr++;
		  sprintf(num,"%d",ptable->ACTION[y][t].action);
		  sprintf(t_ptr,"%d",ptable->ACTION[y][t].action);
		  t_ptr += strlen(num);
		  *t_ptr = ',';
		  t_ptr++;
		  sprintf(num,"%d",ptable->ACTION[y][t].rule);
		  sprintf(t_ptr,"%d",ptable->ACTION[y][t].rule);
		  t_ptr += strlen(num);
		  *t_ptr = ',';
		  t_ptr++;
		  sprintf(num,"%d",ptable->ACTION[y][t].numtoks);
		  sprintf(t_ptr,"%d",ptable->ACTION[y][t].numtoks);
		  t_ptr += strlen(num);

		  *t_ptr = '}';
		  t_ptr++;
		  if(t != ptable->num_terms-1){
			 *t_ptr = ',';
			 t_ptr++;
		  }

	   }
	   *t_ptr = '}';
	   t_ptr++;
	   if(y != ptable->num_states-1){
		  *t_ptr = ',';
		  t_ptr++;
	   }
	   *t_ptr = '\n';
	   t_ptr++;
    }
    *t_ptr = '}';
    t_ptr++;
    *t_ptr = '\0';
    char * new_str;
    //    printf("temp string so far\n%s\n",temp);
    new_str = malloc(sizeof(char)*(strlen(temp)+1));
    memset(new_str,0,strlen(temp)+1);
 //   printf("actual string length of temp needed: chars: %lu\n",strlen(temp));
    strncpy(new_str,temp,sizeof(char)*strlen(temp));
    free(temp);
    temp = NULL;
//    printf("constructed ACTION table:\n %s\n",new_str);
    fputs(new_str,outfile);

}
void push_code_sections(gr_tbl_t* grammar_table, FILE* outfile){
    char* code;
    code = NULL;
    for(int e=1;e<grammar_table->used;e++){
	   code = get_rul_code(get_rul_by_pos(grammar_table,e));
	   if(code == NULL) continue;
	   fprintf(outfile,"case %d:\n%s\nbreak;\n",e,code);
    }
}
void push_aux(gr_tbl_t* grammar_table,FILE* outfile){
    fprintf(outfile,"%s",grammar_table->aux_code);
}
void output_header(gr_tbl_t* grammar_table){
    FILE* outfile;
    tok_tbl_t* tokn;
    outfile = fopen("scy.tab.h","w");
    if(!outfile){
	   perror("error opening scy.tab.h for writing");
	   exit(EXIT_FAILURE);
    }
    fprintf(outfile,"#ifndef SCYTAB_H\n#define SCYTAB_H\n");
    for(size_t i=2;i<grammar_table->tokused;i++){
	   tokn = get_tok_by_id(grammar_table->tokens,i);
	   if(get_tok_type(tokn) == TERMINAL)
		  	fprintf(outfile,"#define %s %zu\n",get_tok_nam(tokn),get_tok_termnum(tokn));
    }
    fprintf(outfile, "#endif\n");
    fclose(outfile);
    outfile = NULL;
}
void push_term_defs(gr_tbl_t* grammar_table,FILE* outfile){
    tok_tbl_t* tokn;
    for(size_t i=2;i<grammar_table->tokused;i++){
	   tokn = get_tok_by_id(grammar_table->tokens,i);
	   if(get_tok_type(tokn) == TERMINAL)
		  fprintf(outfile,"#define %s %zu\n",get_tok_nam(tokn),get_tok_termnum(tokn));
    }
}
