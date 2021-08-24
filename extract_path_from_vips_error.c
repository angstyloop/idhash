/*
gcc extract_path_from_vips_error.c -o test-extract-path-from-vips-error -DTEST_EXTRACT_PATH_FROM_VIPS_ERROR -g -Wall
*/

#include "extract_match.c"

char* extract_path_from_vips_error(char* error_str){
  char* pattern = "\"[^/]+[/[^/]+]*\"";
  if(strlen(pattern)>strlen(error_str)){
    fprintf(stderr, "Pattern is longer than error string.\n    pattern=%s    " 
      "error_str=%s\n", pattern, error_str);
    exit(EXIT_FAILURE);
  }
  // Set last quote to 0, and use memmove to remove first quote.
  return extract_match(error_str, pattern);
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
