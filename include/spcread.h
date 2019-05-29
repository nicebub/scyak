#ifndef SPCREAD_H
#define SPCREAD_H

#include <stdio.h>

//#define FLEX
#ifdef FLEX
extern int yylex(void);
extern FILE* yyin;
#endif
extern char sclex_text[128];

void parse_grammar(FILE* spec_file);
int tokenize(FILE* spec_file);

#endif
