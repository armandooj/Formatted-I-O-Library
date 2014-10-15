#include "my_stdio.h"
#include <stdlib.h>

#define BUFFER_SIZE 64

MY_FILE *my_fopen(char *name, char *mode) {
	// Here we need to initialize the buffer
	// Open + what is needed to initialize the structure
	MY_FILE *new_file = (MY_FILE *)malloc(sizeof(MY_FILE));
	new_file->buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	new_file->name = name;
	new_file->mode = mode;
	return new_file;
}

// TODO group the reads and the reads in groups of 64 bytes 
// every 64 bytes we'll have a system call

int my_fclose(MY_FILE *f) {
	return 0;
}

int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f) {
	// If it's empty or there's not enought data we make the system call
	return 0;
}

int my_fwrite(void *p, size_t taille, size_t nbelem, MY_FILE *f) {
	return 0;
}

int my_feof(MY_FILE *f) {
	return 0;
}