#include "my_stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Printing
#include <stdio.h>

#define BUFFER_SIZE 64

int refill_buffer(MY_FILE *f);
char* concat(char *s1, char *s2);

/*
Opens an access to a file, where name is the path of the file and mode is either ”r” for read or ”w” for write. 
Returns a pointer to a MY FILE structure upon success and NULL otherwise.
*/
MY_FILE *my_fopen(char *name, char *mode) {
	// Initialize a new MY_FILE and its buffer
	MY_FILE *new_file = (MY_FILE *)malloc(sizeof(MY_FILE));
	new_file->buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE + 1);
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
	}
	
	return new_file;
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
		int content_offset = 0;
		int found_eof = 0;
		char *content = (char *)p;
		
		// Proceed reading one by one
		while (no_elements > 0) {	

			// First time - check if no element has bean read and we encountered an end of file
			if (f->buff_offset == -1) {
				found_eof = refill_buffer(f);
				printf("found_eof: %d\n", found_eof);
				if (found_eof == 1) {
					return -1;
				}
			}			

			// Not enough data in the buffer. We must "reload" it
			if (BUFFER_SIZE - f->buff_offset < (int)size) {
				if (found_eof == 1) break;
				found_eof = refill_buffer(f);
			}

			char temp[(int)size + 1]; // TODO maybe use char* and free it each time
			
			// Custom strncpy(temp, f->buffer + f->buff_offset, (int)size);
			// Try to read from the buffer 1 element of size size
			int x = 0;
			for (int i = f->buff_offset; i < f->buff_offset + size; i++) {
				// End of file
				if (f->buffer[i] == '\0') {
					printf("END OF FILE\n");
					no_elements = 0;
					break;
				}
				temp[x] = f->buffer[i];
				x++;
			}
			temp[x] = '\0';

			// Update the buffer's and content's offset position
			// We need two variables because the buffer's offset is set to 0 when read is called
			f->buff_offset += (int)size;
			content_offset += (int)size;
			
			// Concatenate every temp element in content
			content = concat(content, temp);
			content[content_offset] = '\0';
		
			printf("content: %s\n", content);
			printf("buffer when exiting: %s\n\n", f->buffer);	

			no_elements--;					
		}

		// For single chars, assign to p only the char - without the end of line
		if (size == 1 && nbelem == 1) {
			memcpy(p, content, 1);
		} else {
			memcpy(p, content, size * nbelem + 1);
		}
		
		free(content);
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
	if (*f->mode == 'w') {		
		return write(f->fd, p, size * nbelem);
	}
	else {
		// Dennied access
		return -1;
	}
}

/*
Returns 1 if an end-of-file has been encountered during a previous read and 0 otherwise.
*/
int my_feof(MY_FILE *f) {
	return 0;
}


/*
Custom functions
*/


/*
Returns 1 if the end of file has been found, 0 otherwise.
*/
int refill_buffer(MY_FILE *f) {

	int offset = (f->buff_offset == -1) ? 0 : BUFFER_SIZE - f->buff_offset;
	int result;

	if (offset > 0) {
		// If there was something left, we need to put it in the beginning of the new buffer
		if (f->buff_offset > 0) {
			int x = 0;
			for (int i = f->buff_offset; i < BUFFER_SIZE; i++) {
				f->buffer[x] = f->buffer[i];
				x++;
			}
		}

		// Read the rest from the file
		char temp[BUFFER_SIZE - offset + 1];
		result = read(f->fd, temp, BUFFER_SIZE - offset);
		temp[BUFFER_SIZE - offset + 1] = '\0';

		// Append it to buffer
		int j = 0;
		for (int i = offset; i < BUFFER_SIZE; i++) {
			f->buffer[i] = temp[j];
			j++;
		}
		f->buffer[BUFFER_SIZE] = '\0';
	} else {
		result = read(f->fd, f->buffer, BUFFER_SIZE);		
	}

	f->buffer[BUFFER_SIZE] = '\0';
	f->buff_offset = 0;

	printf("Refill: %s\n", f->buffer);
	return (result + offset == BUFFER_SIZE) ? 0 : 1;
}

char* concat(char *s1, char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    // TODO check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
