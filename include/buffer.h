#ifndef BUFFER_PAIR_H
#define BUFFER_PAIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Buffer is 8K Bytes */
#define BUFFER_LENGTH 8196
/* middle of buffer can be found here */
#define HALF_BUFFER BUFFER_LENGTH / 2



/* Macro Function Prototype 
	
	#define getchar(inf) bgetchar(inf)
Short-Hand for self-implemented getchar() function

*/
#define getchar(inf) bgetchar(inf)



/* 
		Buffer Structure
    char *buf - is a pointer to buffer
    char *forward - is a pointer to going ahead in the buffer
    char *back - a pointer to going backwards in the buffer
    FILE* work - a FILE pointer to what the buffer will be used
    int type - type of buffer which can be either end of buffer side, 0
		or found true EOF, 1
    int len - length of current buffer being used

*/
typedef struct _buffer{
    char *buf;
    char *forward;
    char *back;
    FILE* work;
    int type;
    int len;
} buffer;

/* Function Prototype

	int bgetchar(buffer* inf);

Functionality: Return a Character as an integer
	from an existing buffer.

Parameters: buffer* inf - a pointer to an already created
	buffer

Return: An integer representation of the current
	character in the buffer

Results: Either return an integer from the next
	character in the buffer or and error

*/
int bgetchar(buffer* inf);

/* Function Prototype

	int ungetchar(buffer* inf);

Functionality: Move the cursor backwards in the buffer and
	return the token where the cursor(pointer) lies.

Parameters: buffer* inf - a pointer to an already created
	buffer

Return: An integer representation of the current
	character in the buffer

Results: Either return an integer from the next
	character in the buffer or and error

*/
int ungetchar(buffer* inf);

/* Function Prototype

	buffer* buffer_from_file(FILE*);

Functionality: Takes in a FILE pointer that has been
	initialized and points to a specification file,
	and returns a double-sided buffer for that file

Parameters: FILE* pointer to an exisiting, initialized and
	opened file, the specification file ending in .l

Returns: buffer* pointer to the buffer created for the FILE
	pointer

Results: A buffer is now initialized with the first BUFFER_LENGTH
	worth of input and given as pointer as the return value
*/
buffer* buffer_from_file(FILE*);
/* Function Prototype

	buffer* buffer_from_filename(const char * name);

Functionality: Takes in a name of a specification file, checks
	for existense and opens the file, then returns a double-sided 
	buffer for that file

Parameters: const char* name is a filename and exists, the 
	specification file ending in .l

Returns: buffer* pointer to the buffer created for the file
	named in the input

Results: A buffer is now initialized with the first BUFFER_LENGTH
	worth of input and given as pointer as the return value

*/
buffer* buffer_from_filename(const char * name);
/* Function Prototype

	buffer * buffer_from_string(char* instring);

Functionality: Takes in a name of a specification file, checks
	for existense and opens the file, then returns a double-sided 
	buffer for that file

Parameters: char* name is a filename and exists, the 
	specification file ending in .l

Returns: buffer* pointer to the buffer created for the file
	named in the input

Results: A buffer is now initialized with the first BUFFER_LENGTH
	worth of input and given as pointer as the return value
*/
buffer * buffer_from_string(char* instring);
/* Function Prototype

	void delete_buffer(buffer*);

Functionality: A buffer is deleted and the memory is freed to the
	operating system.

Parameters: A buffer* pointer to an already created buffer object

Results: the supplied  buffer is deleted and returned to the OS.

*/
void delete_buffer(buffer*);
/* Function Prototype

	void display_buffer(const buffer*);

Functionality: A buffer's internal structure and data is displayed
	to standard out.

Parameters: an already created buffer* by pointer

Results: The buffer is printed to standard output.

*/
void display_buffer(const buffer*);



#endif
