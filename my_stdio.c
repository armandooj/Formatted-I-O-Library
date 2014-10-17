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
	new_file->buff_offset = 0;

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
		refill_buffer(new_file);
	}
	
	return new_file;
}

void refill_buffer(MY_FILE *f) {
	// TODO validate it's not < 0
	read(f->fd, f->buffer, BUFFER_SIZE);
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
		char *content;
		// Proceed reading one by one
		while (nbelem > 0) {
			// Try to read from the buffer
			if (BUFFER_SIZE - f->buff_offset >= (int)size) {
				/*
				char temp[(int)size];
				memcpy(temp, f->buffer[f->buff_offset], (int)size);
				// TODO end of line?
				strcat(content, temp);
				*/
				// try +=
			} 
			// Not enough data in the buffer. We must "reload" it
			else {
				refill_buffer(f);	
			}
			p = content;
			nbelem--;			
		}
	} else {
		printf("asdfasfasfasdf\n");
		return -1;
	}
	
	return read(f->fd, p, size * nbelem);
}

/*
int doit(int fd, const void *buf, size_t count){
	write(fd,buf,count);
	return 0;
}*/

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
