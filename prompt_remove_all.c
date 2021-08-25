/*
gcc prompt_remove_all.c -o test-prompt-remove-all -DTEST_PROMPT_REMOVE_ALL -Wall -g

Prompt to remove files listed in @source, a file where each line is the path to a file to be removed.
*/

#ifndef PROMPT_REMOVE_H
#  define PROMPT_REMOVE_H
#  include "prompt_remove.c"
#endif

/*
Loop over lines of @source file, where each line of the file is a filename, and ask the the user if they want to delete each file.
*/
void prompt_remove_all(char* source){
  FILE* f=0;
  if(!(f = fopen(source, "r"))){
    fprintf(stderr, "Failed to fopen:\n%s\n", source);
    exit(EXIT_FAILURE);
  }
  char* line=0;
  size_t n=0;
  ssize_t z=0;
  while(-1<(z = getline(&line, &n, f))){
    // Remove terminating newline character, if any. 
    if(line[z-1]=='\n')
      line[z-1]=0;
    // Prompt user to delete file.
    prompt_remove(line);
  }
  fclose(f);
}

#ifdef TEST_PROMPT_REMOVE_ALL
int main(int argc, char* argv[argc]){
  if(2!=argc){
    fprintf(stderr, "Usage %s <FILE>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  prompt_remove_all(argv[1]);
  return EXIT_SUCCESS;
}
#endif
