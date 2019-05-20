#ifndef RUNPARSER_H
#define RUNPARSER_H
#include <stdio.h>

#include "lr0b.h"
void run_parser(struct parser_tables_s* ptable);
int mlexer(FILE* thefile);

#endif
