/**
	Initialize and open a reference to a file that
ideally has been given on the command line.
*/
#include <stdio.h>
#include <stdlib.h>
#include "init.h"

/* how many command-line arguments are we
expecting and allowing */
#define ARGS_ALLOWED 1
/* The first element of the argument array */
#define FIRST 0

FILE* init(int argc, char** argv){
	/* a temporary file pointer to return later
		if opening is successful */
	FILE* spec_file;
	/* shouldn't have a problem here unless getopt parsing
		wasn't accurate */
	/* make sure file was opened, otherwise might as well
		split */
	if((spec_file = fopen(argv[FIRST],"r")) == NULL){
		perror("Error");
		exit(EXIT_FAILURE);
	}
	return spec_file;
}
