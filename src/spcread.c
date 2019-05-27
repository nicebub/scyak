#include <stdio.h>
#include "spcread.h"

#define END -1
#ifndef FLEX
#define LEXER sclex(spec_file)
extern int sclex(FILE* spec_file);
#else
#define LEXER yylex()
#endif

inline int tokenize(FILE* spec_file){
	int r;
	r = LEXER;
#ifdef debug_print
    printf("Lexer reporting token found: %d\n",r);
#endif
    return r;
}
