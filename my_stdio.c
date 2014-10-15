#include "my_stdio.h"
#include <stdlib.h>
#include <unistd.h>

// Printing
#include <stdio.h>

#define BUFFER_SIZE 64

MY_FILE *my_fopen(char *name, char *mode) {
			printf("OPEN ");
	// Here we need to initialize the buffer
	// Open + what is needed to initialize the structure
	MY_FILE *new_file = (MY_FILE *)malloc(sizeof(MY_FILE));
	new_file->buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
	new_file->name = name;
	new_file->mode = mode;
	// Read from the beginning
	new_file->offset = 0;
	new_file->end_buffer = 0;

	// Open the file -> kernel mode
	printf("* %s *\n", name);

	int flag;
	if (*mode == 'r') {
		flag = O_RDONLY;
	} else if (*mode == 'w') {
		flag = O_WRONLY;
	} else {
		flag = 0;
	}

	int inf = open(name, flag);
	if (inf == -1) {
		printf("Error opening file %s.\n", name);
		return NULL;
		}
	new_file->fd = inf;
		printf("OPEN OK\n");
	return new_file;
}

// TODO group the reads and the reads in groups of 64 bytes 
// every 64 bytes we'll have a system call

int my_fclose(MY_FILE *f) {
	return 0;
}

int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f) {
	
	return read(f->fd,p,size*nbelem);

}
/*
int doit(int fd, const void *buf, size_t count){
	write(fd,buf,count);
	return 0;
	}*/

int my_fwrite(void *p, size_t size, size_t nbelem, MY_FILE *f) {
	
	return write(f->fd,p,size*nbelem);

}

int my_feof(MY_FILE *f) {
	return 0;
}
