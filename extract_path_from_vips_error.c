/*
gcc extract_path_from_vips_error.c -o test-extract-path-from-vips-error -DTEST_EXTRACT_PATH_FROM_VIPS_ERROR -g -Wall
*/

#include "extract_match.c"

char* extract_path_from_vips_error(char* error_str){
  char* pattern = "\"[^/\" ]+(/[^/\" ]+)*\"";
  if(strlen(pattern) > strlen(error_str))
    return 0;
  // Set last quote to 0, and use memmove to remove first quote.
  char* match = extract_match(error_str, pattern);
  if(match){
    int n = strlen(match);
    // Zero last quote.
    match[n-1]=0;
    // Overwrite first quote.
    memmove(match, match+1, n); //don't forget newline
  }
  return match;
}

#ifdef TEST_EXTRACT_PATH_FROM_VIPS_ERROR
#include <assert.h>
int main(){
  char* error_str = "VipsForeignLoad: \"duplicates/119_a.jpg\" is not a known file format";
  char* path = extract_path_from_vips_error(error_str);
  if(path){
    puts(path);
    free(path);
  }
  return EXIT_SUCCESS;
}
#endif
