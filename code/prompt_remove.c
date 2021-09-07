/*
gcc prompt_remove.c -o test-prompt-to-delete-file -DTEST_PROMPT_TO_DELETE_FILE -Wall -g
*/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

void prompt_remove(char* path){
  char* line=0;
  size_t n=0;
  ssize_t z=0;
  for(;;){
    fprintf(stdout, "remove %s?(y/n)", path);
    // Loop over lines. Check the first character for 'y' or 'n'.
    if(1>(z = getline(&line, &n, stdin)) || 'n' == *line || 'N' == *line){
      break;
    }else if('y' == *line || 'Y' == *line){
      if(remove(path)){
        fprintf(stderr, "Failed to remove %s\n.", path);
        exit(EXIT_FAILURE);
      } 
      break;
    }
  }
}

#ifdef TEST_PROMPT_TO_DELETE_FILE
int main(int argc, char* argv[argc]){
  if(argc!=2){
    fprintf(stderr, "Usage: %s <FILE>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  prompt_remove(argv[1]);
  return EXIT_SUCCESS;
}
#endif
