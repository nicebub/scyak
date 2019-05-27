/**
This is the main file of the parser generator. This wrapper will call 
	the entire stack of the program.  The main function here will later
	be called scyak as an executable.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "spcread.h"
#include "parser.h"
#include "run_parser.h"

void version(void);
void usage(void);
void cleanup(FILE*);
	
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
    run_parser(read_and_parse_specfile(spec_file));
    	printf("done running parser\n");
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
