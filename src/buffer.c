/**
	The buffer structure and functions supplied with this program
were created in reference to the algorithm for a double-sided buffer
in the famous dragon book. The buffer is a front for the input stream an
allows for forward and backwards movement up to half the buffers length,
ie, if the buffer is 8K then the backwards direction can be up to 4K of
characters.

*/

#include "buffer.h"

/* Function Prototype

	void refresh_upper_buffer(buffer* inbuf);

Functionality: flush the upper half of the buffer with more 
	input if there exists any

Parameters: An existing buffer by a buffer* pointer inbuf

Results: Either refreshes the upper buffer or leaves it as it
	if the buffer cannot be refreshed.
*/
void refresh_upper_buffer(buffer* inbuf);
/* Function Prototype

	void refresh_lower_buffer(buffer* inbuf);

Functionality: flush the lower half of the buffer with more 
	input if there exists any

Parameters: An existing buffer by a buffer* pointer inbuf

Results: Either refreshes the lower buffer or leaves it as it
	if the buffer cannot be refreshed.
*/
void refresh_lower_buffer(buffer* inbuf);
/* Function Prototype

	void refresh_buffer(buffer* inbuf, const int start);

Functionality: flush the the buffer with more input if there
	exists any, this could be either the upper or lower half
	depending on where the current character pointer points to

Parameters: An existing buffer by a buffer* pointer inbuf

Results: Either refreshes the buffer or leaves it as it
	if the buffer cannot be refreshed.

*/
void refresh_buffer(buffer* inbuf, const int start);


/* 

	int bgetchar(buffer* inbuf)

Functionality: Takes as input an existing, initialized buffer and returns
	the next chracter in the buffer as an integer, and increments the buffer
	to the next character or EOF.

Parameters: A pointer to a buffer buffer* inbuf, that has been initialized

Returns: a character as an integer or -1

Results: The current pointer to the buffer points to the next character in the
	input buffer and an integer of the current character is returned.
	
*/
int bgetchar(buffer* inbuf){
	/* Temporary used to return current character of buffer */
    int r;
/* Check buffer, either we have found EOF in which we may be at the end of half
	the buffer or we can increment the buffer 1 character instead
*/
    switch(*(inbuf->forward)){
	   case EOF:
/* 
	Check whether we found true EOF or just the token used to represent the end
	   of the buffer half.
*/
		  switch(inbuf->type){
			  /* Buffer Half has been found */
			 case 0:
			 /* either we found the upper buffer limit ... */
			 /* and if so we move the pointer to the proper start of next buffer */
					if(inbuf->forward == &inbuf->buf[HALF_BUFFER-1]){
					    refresh_lower_buffer(inbuf);
					    inbuf->forward = &inbuf->buf[HALF_BUFFER];
					}
			/* or we found the lower buffer limit */
					else if(inbuf->forward == &inbuf->buf[BUFFER_LENGTH-1]){
					    refresh_upper_buffer(inbuf);
					    inbuf->forward = &inbuf->buf[0];

					}
					else{
			/* True EOF has been found, so return it */
			case 1:
					    return EOF;
					}
				break;
		  }
/* we can increment the buffer and return the appropriate 
		  current character */
	   default:
		  r = *inbuf->forward;
		  inbuf->forward++;
		  return r;
		  break;
    }
	/* All else fails return the NULL character */
	return '\0';
}

/*

	int ungetchar(buffer* mbuf)

Functionality: return the current character but move the pointer
	backwards in the buffer instead of forwards

Parameters: An existing buffer through a pointer buffer* mbuf

Returns: A character as an integer or -1

Results: The buffer pointer is moved backwards by 1 character if possible
	and then the current character is returned

*/
int ungetchar(buffer* mbuf){
/* Check to see if we have found True EOF, the token EOF, or other */
    switch(mbuf->type){
	  /* Buffer Half has been found */
	   case 0:
		 /* either we found the upper buffer limit ... */
		 /* and if so we move the pointer to the proper start of next buffer */
		  if(mbuf->forward == &mbuf->buf[HALF_BUFFER] )
	    		mbuf->forward = &mbuf->buf[HALF_BUFFER-3];
		/* or we found the lower buffer limit */
		  else if(mbuf->forward == &mbuf->buf[0])
			 mbuf->forward = &mbuf->buf[BUFFER_LENGTH-3];
		  /* or we can move the buffer backwards 1 character and 
		  	return 
		  */
		  else
			 mbuf->forward--;
		  return *mbuf->forward;
		  break;
		/* True EOF has been found, so we check if we can move
		  	 the buffer by one or not */
	   case 1:
	   /* we are at the start so go to the end */
		  if(mbuf->forward == mbuf->buf )
			 mbuf->forward = &mbuf->buf[mbuf->len-3];
		  /* we can just move backwards by 1 character and 
		  	return the current character */
		  else
			 mbuf->forward--;
		  return *mbuf->forward;
		  break;
    }
	/* all else fails, return -1 */
    return -1;
}

/* 

	buffer * buffer_from_string(char* instring)

Functionality: Creates and returns a pointer to a buffer structure from
	a stream of characters or character array as the input
	This buffer is not double-sided

Parameters: char * string an array of characters or input stream

Returns: returns a initialized buffer from the character string array
	this buffer is not double-sided

Results: A buffer initialized with BUFFER_LENGTH of data and the buffer
	tokens are set at the end.

*/

buffer * buffer_from_string(char* instring){
/* create a buffer structure, and from the length of the
	input string create a buffer in memory the same size
	plus a few extra bytes for control characters. 
*/
	size_t in_len = strnlen(instring,128);
    buffer *mbuf = malloc(sizeof(*mbuf));
    mbuf->len = (int)in_len;
    mbuf->buf = malloc(sizeof(char)*(mbuf->len+2));
/* copy the input string into the buffer, setup the control
	characters, and return the buffer
*/	
    strncpy(mbuf->buf,instring,in_len);
    mbuf->buf[mbuf->len] = '\0';
    mbuf->buf[mbuf->len+1] = EOF;
    mbuf->forward = mbuf->buf;
    mbuf->back = mbuf->buf;
    mbuf->type = 1;
    return mbuf;
}

/* 

	buffer* buffer_from_file(FILE* infile)

Functionality: Creates and returns a pointer to a buffer structure
	from a FILE * pointer or exits with an error

Parameters: FILE * infile a pointer to an initialied FILE

Returns: returns an initialized buffer from the FILE *

Results: a buffer structure is created, initialzied, and returned

*/
buffer* buffer_from_file(FILE* infile){
/* buffer created and set to size of BUFFER_LENGTH */
    buffer *mbuf = malloc(sizeof(*mbuf));
    
    mbuf->buf = malloc(sizeof(char)*BUFFER_LENGTH);
    
	/* exit with error, nothing given to function as input */
    if(infile == NULL ){
	   printf("\033[0;31merror\033[0m");
	   exit(-1);
    }

	/* 
	set buffer to associate with the FILE * infile for later use,
	as well as initialize control characters
	*/
    mbuf->work = infile;
    mbuf->forward = mbuf->buf;
    mbuf->back = mbuf->buf;
    mbuf->buf[HALF_BUFFER-1] = EOF;
    mbuf->buf[HALF_BUFFER-2] = '\0';
    mbuf->buf[BUFFER_LENGTH-1] = EOF;
    mbuf->buf[BUFFER_LENGTH-2] = '\0';
    mbuf->type = 0;
	/* 
	Call refresh_buffer to initialize buffer for first time
	*/
    refresh_buffer(mbuf,0);
    
	/* clear 2nd half of buffer until later */
    for(int b=HALF_BUFFER;b<BUFFER_LENGTH-2;b++)
	   mbuf->buf[b]=' ';
    
    return mbuf;
}

/* 

	buffer* buffer_from_filename(const char * name)

Functionality: Create and return a pointer to a buffer structure
	from a name of a file that is then tied to a FILE* pointer.
	Otherwise exit with an error.

Parameters: const char* name is the fully qualified name of a file
	in a character string

Returns: returns a pointer to a buffer that has been initialized from
	the data found in the file 'name'

Results: A buffer structure and FILE are both opened and created.
	The resulting buffer is initialized and returned.

*/
buffer* buffer_from_filename(const char * name){
	/* allocate memory for buffer */
    buffer *mbuf = malloc(sizeof(*mbuf));

    mbuf->buf = malloc(sizeof(char)*BUFFER_LENGTH);
	/* try to open file, if it exists continue otherwise
		exit with an error
	*/
    if((mbuf->work = fopen(name, "r")) == NULL ){
		perror("\033[0;31merror\033[0m");
		exit(-1);
	}
	/* initialize control characters and refresh buffer to initialize
		the buffer with data from the input file
	*/

    mbuf->forward = mbuf->buf;
    mbuf->back = mbuf->buf;
	mbuf->buf[HALF_BUFFER-1] = EOF;
	mbuf->buf[HALF_BUFFER-2] = '\0';
	mbuf->buf[BUFFER_LENGTH-1] = EOF;
	mbuf->buf[BUFFER_LENGTH-2] = '\0';
    mbuf->type = 0;
    refresh_buffer(mbuf,0);

	/* clear the lower half of the buffer */
    for(int b=HALF_BUFFER;b<BUFFER_LENGTH-2;b++)
	   mbuf->buf[b]=' ';

    return mbuf;
}

/* 

	inline void delete_buffer(buffer* mbuf)

Functionality: Delete Buffer and release memory back to Operating System

Parameters: An already existing buffer

Results: The buffer is deleted and memory is returned to OS

*/
inline void delete_buffer(buffer* mbuf){
	if(mbuf){
	    free(mbuf->buf);
	    mbuf->buf = NULL;
	    free(mbuf);
	    mbuf = NULL;
	}
}

/*  DEBUG FUNCTION

	inline void display_buffer(const buffer* inbuf)

Functionality: Prints the contents of the buffer and its structure
	to standard output

Parameters: An existing buffer initialized with data

Results: The buffer is printed to standard output

*/
inline void display_buffer(const buffer* inbuf){
    switch(inbuf->type){
	   case 0:
		  printf("--MARK UP END--\n%s\n--MARK MIDDLE--\n",(char*)inbuf->buf);
		  printf("%s\n--MARK LOW END--\n",&inbuf->buf[HALF_BUFFER]);
		  break;
	   case 1:
		  printf("--MARK UP END--\n%s\n--MARK LOW END--\n",(char*)inbuf->buf);
		  break;
    }
}


/* 

	inline void refresh_upper_buffer(buffer* inbuf)

Functionality: Call refresh_buffer for only the upper half

Parameters: An existing buffer initialized with data

Results: either the buffer is flushed with new data or nothing
	if there is no more new data

*/
inline void refresh_upper_buffer(buffer* inbuf){
    refresh_buffer(inbuf,0);
}

/* 

	inline void refresh_lower_buffer(buffer* inbuf)

Functionality: Call refresh_buffer for only the lower half

Parameters: An existing buffer initialized with data

Results: either the buffer is flushed with new data or nothing
	if there is no more data

*/
inline void refresh_lower_buffer(buffer* inbuf){
    refresh_buffer(inbuf,HALF_BUFFER);
}

/* 

inline void refresh_buffer(buffer* inbuf, const int start)

Functionality: Refresh a buffer with new data if any is in the pipeline
	if an error comes up then exit with an error

Parameters: buffer* inbuf points to an existing inititalized buffer
	with data
			const int start is where to start counting inside the buffer

Results: The buffer is refreshed with new data if any exists otherwise
	nothing is changed.

*/
inline void refresh_buffer(buffer* inbuf, const int start){
    static size_t amount;
	/* read more data into the buffer and store the amount read into 'amount' */
	if(inbuf->work == stdin){
		fgets(&inbuf->buf[start],(size_t)(HALF_BUFFER-2),inbuf->work);
		amount = strlen(&inbuf->buf[start]);
	}
	else
		amount=fread(&inbuf->buf[start],1,HALF_BUFFER-2,inbuf->work);	
    if( amount != HALF_BUFFER-2){
	   if(ferror(inbuf->work)!=0){
		  perror("\033[0;31merror\033[0m");
		  exit(-1);
	   }
	   /* if we found data then reset the control characters */
	   if(feof(inbuf->work)!=0){
		  if(amount >0){
			 	inbuf->buf[start+amount]='\0';
			 	inbuf->buf[start+amount+1]=EOF;
		 
		  }
		  else
			 ;
	   }
    }
  
}
