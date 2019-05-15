#include <stdio.h>
#include "spcread.h"

#define END -1
//#define FLEX
#ifndef FLEX
#define LEXER sclex(spec_file)
extern int sclex(FILE* spec_file);
#else
#define LEXER yylex()
#endif


int tokenize(FILE* spec_file);

void parse_grammar(FILE* spec_file){
	int tok;
	#ifdef FLEX
	yyin = spec_file;
	#endif
	while((tok = tokenize(spec_file)) != END ){
		printf("token: %d\n",tok);
	}
}

inline int tokenize(FILE* spec_file){
	int r;
	r = LEXER;
	printf("Lexer reporting token found: %d\n",r);
	return r;
}