#include "my_stdio.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

#define BUFFER_SIZE 64

int refill_buffer(MY_FILE *f);
int kernel_calls = 0;
char* concat(char *s1, char *s2);
char *str_replace(char *orig, char *rep, char *with);

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
	new_file->found_eof = 0;
	new_file->found_eob = 0;

	// Open the file -> kernel mode
	int flag;
	if (*mode == 'r') {
		// An offset of 0 is going to mean that we haven't read from the file yet
		new_file->buff_offset = -1;
		flag = O_RDONLY;
	} else if (*mode == 'w') {
		new_file->buff_offset = 0;
		flag = O_WRONLY | O_CREAT;
	} else {
		flag = 0;
	}

	int inf = open(name, flag);
	kernel_calls++;

	if (inf < 0) {
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
	
	kernel_calls++;
	if (close(f->fd) == 0) {
		free(f->buffer);
		free(f);
	}

	return -1;
}

/*
Reads at most nbelem elements of size size from file access f, that has to have been opened with mode ”r”, and places them at the address pointed by p. 
Returns the number of elements actually read, 0 if an end-of-file has been encountered before any element has been read and -1 if an error occured.
*/
int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f) {

	if (*f->mode == 'r') {
		if (size < BUFFER_SIZE) {

			int no_elements = nbelem;
			int content_offset = 0;
			int found_eof = 0;
			char *content = (char *)p;
			content[0] = '\0';
			
			// Proceed reading one by one
			while (no_elements > 0) {

				// First time - check if no element has bean read and we encountered an end of file
				if (f->buff_offset == -1) {
					found_eof = refill_buffer(f);
					if (found_eof == 2) {
						return -1;
					}
				}

				// Not enough data in the buffer. We must "reload" it
				if (BUFFER_SIZE - f->buff_offset < (int)size) {
					if (found_eof == 1) break;
					found_eof = refill_buffer(f);
				}

				char temp[(int)size + 1];
				
				// Try to read from the buffer 1 element of size size
				int x = 0;
				for (int i = f->buff_offset; i < f->buff_offset + size; i++) {
					// End of file
					if (f->buffer[i] == '\0') {
						// Both end of buffer and end of file
						if (f->found_eof == 1) {
							f->found_eob = 1;
						}
					
						no_elements = 0;					
						// For single chars, we need to return when end of line is found (nothing else to read)
						if (size == 1 && nbelem == 1) {	
							content[content_offset] = f->buffer[i];		
							memcpy(p, content, 1);			
							return 0;
						}

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
				// content = concat(content, temp);
				if (size == 1 && nbelem == 1) {
					content = temp;
				} else {
					content = strcat(content, temp);
				}

				content[content_offset] = '\0';		

				no_elements--;		

				// For single chars, assign to p only the char - without the end of line
				if (size == 1 && nbelem == 1) {
					memcpy(p, content, 1);
				} else {
					memcpy(p, content, size * nbelem + 1);
				}
			}
		} else {
			// If the buffer is not big enough for the requested size, use read()
			read(f->fd, p, size * nbelem);
			kernel_calls++;
		}
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
		// We can't write to buffer, write to the file
		if (size > BUFFER_SIZE) {
			kernel_calls++;
			return write(f->fd, p, size * nbelem);
		} else {
			//return write(f->fd, p, size * nbelem);
			char *content = (char *)p;
			int chunk = 0;

			while ((int)nbelem > 0) {
				// Not enough space in buffer
				if (BUFFER_SIZE - f->buff_offset < size) {

					// Write to file what buffer had
					kernel_calls++;
					write(f->fd, f->buffer, BUFFER_SIZE - (BUFFER_SIZE - f->buff_offset));					
					f->buff_offset = 0;
					f->buffer[0] = '\0';
				} 

				// There's enough space in the buffer, write to it
				int i;
				for (i = 0; i < size; i++) {
					f->buffer[f->buff_offset] = content[chunk * size + i];
					f->buff_offset++;				
				}
				
				chunk++;
				nbelem--;						
			}
			
			// Write whatever is left
			if (f->buffer[0] != '\0') {
				kernel_calls++;
				write(f->fd, f->buffer, BUFFER_SIZE - (BUFFER_SIZE - f->buff_offset));
				f->buff_offset = 0;
				f->buffer[0] = '\0';
			}

			return nbelem * size;
		}
	}
	else {
		// Dennied access
		return -1;
	}
}

/*
Writes to f the arguments given after format using the format given in format. 
Returns the number of arguments successfully written or -1 upon error.
*/
int my_fprintf(MY_FILE *f, char *format, ...) {
	va_list ap;
	int d;
	char c, *s;

	char *parsed = format;
	int parsed_size = 0;

	va_start(ap, format);
	while (*format) {
		switch (*format++) {			
			case '%':
			switch (*format++) {
				case 's':		
				s = va_arg(ap, char *);
				parsed = str_replace(parsed, "%s", s);
				break;

				case 'd':
				d = va_arg(ap, int);	
				char num[5];
				parsed = str_replace(parsed, "%d", num);
				break;

				case 'c':
				c = (char)va_arg(ap, int);
				char *str = (char *)malloc(sizeof(char) * 2);
				str[0] = c;
				str[1] = '\0';
				parsed = str_replace(parsed, "%c", str);
				free(str);
				break;
			}
			break;

			default:
			break;
		}
	}

	va_end(ap);

	for (int i = 1; parsed[i] != '\0'; i++) {
		parsed_size++;
	}

	return my_fwrite(parsed, strlen(parsed), 1, f);
}

/*
Stores at the addresses given after format the values read from the access associated to f using the format given in format
as described in the following. Returns the number of arguments successfully read or the value EOF if an end-of-file is 
encountered before any value is read or upon error.
*/
int my_fscanf(MY_FILE *f, char *format, ...) {
	va_list ap;
	int *d;
	char *s, c;
	int pos;
	int read_elements = 0;

	va_start(ap, format);
	while (*format) {
		switch (*format++) {
			case '%':
			switch (*format++) {
				
				case 's':
				s = va_arg(ap, char *);
				read_elements += my_fread(s, 1, 1, f);
				break;

				case 'd':
				d = va_arg(ap, int *);				
				pos = 0;

				// Read the first one
				read_elements += my_fread(&c, 1, 1, f);
				char *str;

				// store it locally
				str[pos] = c;
				pos++;
				str[pos] = '\0';

				// Find the rest of the "number"		
				while (!my_feof(f) && c != ' ') {
					read_elements += my_fread(&c, 1, 1, f);
					str[pos] = c;
					pos++;
  				}

  				// Convert to int and we're done
				*d = atoi(str);
				break;

				case 'c':
				// Read only one char
				s = va_arg(ap, char *);
				read_elements += my_fread(&c, 1, 1, f);
				s[0] = c;
				break;
			}
			break;

			default:
			break;
		}	
	}

	va_end(ap);

	return read_elements;
}


/*
Custom functions
*/

/*
Returns 
1 if the end of file has been found after reading something
2 if the end of file has been found without reading anything
0 otherwise
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
		kernel_calls++;

		f->buffer[result + offset] = '\0';

		// Append it to buffer
		int j = 0;
		for (int i = offset; i < BUFFER_SIZE; i++) {
			f->buffer[i] = temp[j];
			j++;
		}		
	} else {
		// Actual kernel call
		result = read(f->fd, f->buffer, BUFFER_SIZE);
		kernel_calls++;		
		// The buffer is not full
		f->buffer[result] = '\0';
	}

	f->buff_offset = 0;
	
	if (result + offset != BUFFER_SIZE) {
		f->found_eof = 1;
		// End of file found
		if (result == 0) {
			return 2;
		} else {
			return 1;
		}
	} else {
		return 0;
	}
}

int get_system_calls_count() {
	return kernel_calls;
}

/*
Returns 1 if an end-of-file has been encountered during a previous read and 0 otherwise.
*/
int my_feof(MY_FILE *f) {
	// Internally, end of file happens when both buffer and file have been read.
	return (f->found_eof && f->found_eob);
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    count = 1;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}
