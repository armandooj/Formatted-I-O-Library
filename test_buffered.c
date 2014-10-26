#include <stdlib.h>
#include "my_stdio.h"

// Printing
#include <stdio.h>

int main (int argc, char *argv[])
{
  MY_FILE *f1;
  MY_FILE *f2;
  // char c;
  int result;

  // for the sake of simplicity we don't
  // print any error messages
  if (argc != 3)
      exit (-1);

  f1 = my_fopen(argv[1], "r");
  if (f1 == NULL)
      exit (-2);

  f2 = my_fopen(argv[2], "w");
  if (f2 == NULL)
      exit (-3);
 
  
  /*
  // Custom Test
  // TODO French chars?
  char *c = (char *)malloc(sizeof(char) * 1024);
  result = my_fread(c, 50, 3, f1);
  printf("Result: %d\n", result);
  printf("Test: %s\n", c);
  result = my_fwrite(c, 50, 3, f2);
  free(c);
  */
  
  /*
  // Buffered inputs/outputs - Test one  
  char c;
  result = my_fread(&c, 1, 1, f1);

  while (result == 1) {
    result = my_fwrite(&c, 1, 1, f2);
    if (result == -1)
        exit(-4);
    result = my_fread(&c, 1, 1, f1);
  }

  if (result == -1)
    exit(-5);

  my_fclose(f1);
  my_fclose(f2);
  */


  /*
  // Formatted inputs/outputs custom test (fprintf)
  char c;
  my_fprintf(f2, "Input files: %s %d %c %d\n", argv[1], 52, 'x', 31);
  my_fprintf(f2, "H");
  my_fscanf(f1, "%c", &c);
  printf("*%c\n", c);
  */


  // Formatted inputs/outputs - Test one
  /*
  char c;
  my_fprintf(f2, "Input file: %s\n", argv[1]);
  my_fscanf(f1, "%c", &c);
  while (!my_feof(f1)) {
    my_fprintf(f2, "Character %c read, its ASCII code is %d\n", c, c);
    my_fscanf(f1, "%c", &c);
  }

  my_fclose(f1);
  my_fclose(f2);
  */

  
  // Formatted inputs/outputs custom test (fscanf)
  /*
  char c1, c2;
  char *s1 = (char*)malloc(sizeof(char) * 5);
  int d;

  my_fscanf(f1, "%c%c", &c1, &c2);
  printf("Test: %c %c", c1, c2);
  my_fprintf(f2, "%c %c", c1, c2);
  */

  /*
  my_fscanf(f1, "%c %c %d", &c1, &c2, &d);
  printf("Test: %c %c %d", c1, c2, d);
  my_fprintf(f2, "%c %c %d", c1, c2, d);
  */

  my_fclose(f1);
  my_fclose(f2);

  //printf("Sys calls: %d\n", get_system_calls_count());
  return 0;
}
