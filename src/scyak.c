/**
This is the main file of the parser generator. This wrapper will call 
	the entire stack of the program.  The main function here will later
	be called scyak as an executable.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "lr0b.h"
#include "spcread.h"
#include "parser.h"
#include <string.h>
//#include "outfile.in"
//#include "run_parser.h"

void version(void);
void usage(void);
void cleanup(FILE*);
void create_parser_file(struct parser_tables_s* ptables);

int main(int argc, char ** argv){
	char option;
	FILE* spec_file;
	if(argc <2){
		usage();
		exit(EXIT_FAILURE);
	}
	else{
		while((option = getopt(argc,argv,"vuVh")) != -1){
			switch(option){
				case 'v':	version();
							break;
				case 'h':
				case 'u':	usage();
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
	spec_file = init(argc,argv);
	/*
		parse spec file
	*/
	#ifdef FLEX
	yyin = spec_file;
	#endif
    struct parser_tables_s* ptables;
    	ptables = read_and_parse_specfile(spec_file);
    create_parser_file(ptables);
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
	printf("	scyak [-vhuV] <parser specification file>\n\n");
	printf("General Options:\n");
	printf("-v			Print the version\n");
	printf("-h			Show this help info\n");
	printf("-u			Show this usage info\n");
	printf("-V			Verbose\n");
}

inline void version(void){
	printf("scyak version 0.000001\n");
}
void process_outfile(FILE* outfile,char* code);
char* put_out_until_token(FILE* outfile,char* new_code);
void push_goto_tbl(struct parser_tables_s* ptable,FILE* outfile);
void push_action_tbl(struct parser_tables_s* ptable,FILE* outfile);

void create_parser_file(struct parser_tables_s* ptables){
    FILE* parse_code;
    FILE* outfile;
    char* new_code;
    char* temp_code;
    size_t fl_sz;
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
    printf("----------code file found------------------\n");
    printf("%s",new_code);
    printf("----------code file found------------------\n");
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
    sscanf(num,"%d",ptables->num_states);
    fprintf(outfile,"%d",ptables->num_states);
    c_ptr += strlen(num);
    c_ptr = put_out_until_token(outfile,c_ptr);
    sscanf(num,"%d",ptables->num_nonterms);
    fprintf(outfile,"%d",ptables->num_nonterms);
    c_ptr += strlen(num);
    c_ptr = put_out_until_token(outfile,c_ptr);
    sscanf(num,"%d",ptables->num_terms);
    fprintf(outfile,"%d",ptables->num_terms);
    c_ptr += strlen(num);

    c_ptr = put_out_until_token(outfile,c_ptr);
    push_goto_tbl(ptables,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
    push_action_tbl(ptables,outfile);
    c_ptr = put_out_until_token(outfile,c_ptr);
//    if(c_ptr != NULL)printf("found a special token\n");
}

void process_outfile(FILE* outfile,char* new_code){
    char* c_ptr;
    c_ptr = new_code;
    c_ptr = put_out_until_token(outfile,c_ptr);
    
//    if(c_ptr != NULL)printf("found a special token\n");
    
}
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
    printf("constructed GOTO table:\n %s\n",new_str);
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
    printf("actual string length of temp needed: chars: %lu\n",strlen(temp));
    strncpy(new_str,temp,sizeof(char)*strlen(temp));
    free(temp);
    temp = NULL;
    printf("constructed ACTION table:\n %s\n",new_str);
    fputs(new_str,outfile);

}
