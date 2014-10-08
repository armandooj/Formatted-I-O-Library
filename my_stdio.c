#include "my_stdio.h"
#include <stdlib.h>

#define BUFFER_SIZE 64

typedef struct my_file {
	char *buffer;
	char *name;
	char *mode;
} my_file_s, *my_file_t;

my_file_t *my_fopen(char *name, char *mode) {
	// Here we need to initialize the buffer
	// Open + what is needed to initialize the structure
	my_file_t new_file = (my_file_t)malloc(sizeof(my_file_s));
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