#ifndef TOKENS_H
#define TOKENS_H

#define REALSTART 1 	// NON-TERMINAL
#define C_IDENT 2 		// NON-TERMINAL
#define IDENT 3 		// TERMINAL
#define LCBRA 4			// TERMINAL
#define RCBRA 5 	 	// TERMINAL
#define COMMA 6 	 	// TERMINAL
#define LESS 7 		 	// TERMINAL
#define GREAT 8 	 	// TERMINAL
#define MARK 9 			// NON-TERMINAL
#define LMARK 10  		// NON-TERMINAL
#define RMARK 11	 	// NON-TERMINAL
#define COLON 12 	 	// TERMINAL
#define START 13	 	// NON-TERMINAL
#define UNION 14	 	// NON-TERMINAL
#define LEFT 15	 		// NON-TERMINAL
#define RIGHT 16	 	// NON-TERMINAL
#define NONASSOC 17	 	// NON-TERMINAL
#define TYPE 18	 		// NON-TERMINAL
#define NUMBER 19 	 	// TERMINAL
#define ORSYMB 20	 	// NON-TERMINAL
#define PREC 21	 		// NON-TERMINAL
#define SYMBOL 22	 	// NON-TERMINAL
#define INTEGER 23 	 	// TERMINAL
#define TOKEN 24	 	// NON-TERMINAL
#define CODE 25	 		// NON-TERMINAL
#define SEMICOLON 26 	// TERMINAL
#define EMPTY -1

#endif