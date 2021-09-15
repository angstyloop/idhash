/*
Skip a line of a file.

gcc -g -Wall skip_line.c -o test-skip-line -DTEST_SKIP_LINE

*/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef STRING_H
#  define STRING_H
#  include <string.h>
#endif

#ifndef ASSERT_H
#  define ASSERT_H
#  include <assert.h>
#endif

void skip_line(FILE* fp){
  char* line=0;
  size_t n=0;
  getline(&line, &n, fp);
  free(line);
}

#ifdef TEST_SKIP_LINE 
int main(){
  // write  1, 2, 3 to a new file (3 lines)
  FILE* fp = fopen("foo.txt", "w");
  fprintf(fp, "1\n2\n3");
  fclose(fp);
  fp = fopen("foo.txt", "r");

  // skip the first line. 
  char* line=0;
  size_t n=0;
  skip_line(fp);
  // assert the remaining two lines are 2 and 3.
  getline(&line, &n, fp);
  assert(!strcmp(line, "2\n"));
  getline(&line, &n, fp);
  assert(!strcmp(line, "3"));

  // clean up
  free(line);
  fclose(fp);

  //success!
  puts("OK");
  return(EXIT_SUCCESS);
}
#endif
