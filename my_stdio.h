#include "stddef.h"
#include <fcntl.h>

typedef struct {
	char *buffer;
	char *name;
	char *mode;
	int fd;
	int buff_offset;
} MY_FILE;

MY_FILE *my_fopen(char *name, char *mode);
int my_fclose(MY_FILE *f);
int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f);
int my_fwrite(void *p, size_t taille, size_t nbelem, MY_FILE *f);
int my_feof(MY_FILE *f);