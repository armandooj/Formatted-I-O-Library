#include "my_stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Printing
#include <stdio.h>

#define BUFFER_SIZE 64

void refill_buffer(MY_FILE *f);

/*
Opens an access to a file, where name is the path of the file and mode is either ”r” for read or ”w” for write. 
Returns a pointer to a MY FILE structure upon success and NULL otherwise.
*/
MY_FILE *my_fopen(char *name, char *mode) {
	// Here we need to initialize the buffer
	// Open + what is needed to initialize the structure
	MY_FILE *new_file = (MY_FILE *)malloc(sizeof(MY_FILE));
	new_file->buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	new_file->name = name;
	new_file->mode = mode;
	// An offset of 0 is going to mean that we haven't read from the file yet
	new_file->buff_offset = -1;

	// Open the file -> kernel mode
	int flag;
	if (*mode == 'r') {
		flag = O_RDONLY;
	} else if (*mode == 'w') {
		flag = O_WRONLY;
	} else {
		flag = 0;
	}

	int inf = open(name, flag);
	if (inf < 0) {
		printf("Error opening file %s.\n", name);
		return NULL;
	} else {
		// Success, save the file descriptor for future access to the file
		new_file->fd = inf;

		// Should this be here?
		// refill_buffer(new_file);
	}
	
	return new_file;
}

void refill_buffer(MY_FILE *f) {
	// TODO validate it's not < 0
	// TODO if there's something in the buffer, buffer offset is less than buffer size, then we have to keep that in the new buffer
	read(f->fd, f->buffer, BUFFER_SIZE);
	f->buff_offset = 0;
}

/*
Closes the access to a file associated to f. Returns 0 upon success and -1 otherwise.
*/
int my_fclose(MY_FILE *f) {
	// TODO group the reads and the reads in groups of 64 bytes 
	// every 64 bytes we'll have a system call
	return 0;
}

/*
Reads at most nbelem elements of size size from file access f, that has to have been opened with mode ”r”, and places them at the address pointed by p. 
Returns the number of elements actually read, 0 if an end-of-file has been encountered before any element has been read and -1 if an error occured.
*/
int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f) {
	
	if (*f->mode == 'r') {
		int no_elements = nbelem;
		char *content = (char *)malloc(sizeof(char));
		
		// Proceed reading one by one
		while (no_elements > 0) {
			// Not enough data in the buffer. We must "reload" it
			if (BUFFER_SIZE - f->buff_offset < (int)size || f->buff_offset == -1) {
				refill_buffer(f);	
			}

			// Try to read from the buffer 1 element of size size
			char temp[(int)size];
			strncpy(temp, f->buffer + f->buff_offset, (int)size);
			temp[(int)size] = '\0';

			// Update the offset position
			f->buff_offset += (int)size;

			// Concatenate every element in content
			strcat(content, temp);

			no_elements--;			
		}

		p = content;
		free(content);
		printf("Read: %s\n", p);
		return nbelem;		
	} else {
		// Dennied access
		return -1;
	}
}

/*
Writes at most nbelem elements of size size to file access f, that has to have been opened with mode ”w”, taken at the address pointed by p.
Returns the number of elements actually written and -1 if an error occured.
*/
int my_fwrite(void *p, size_t size, size_t nbelem, MY_FILE *f) {
	
	return write(f->fd, p, size * nbelem);
}

/*
Returns 1 if an end-of-file has been encountered during a previous read and 0 otherwise.
*/
int my_feof(MY_FILE *f) {
	return 0;
}
