/*
gcc extract_path_from_vips_error.c -o test-extract-path-from-vips-error -DTEST_EXTRACT_PATH_FROM_VIPS_ERROR -g -Wall
*/

#include "extract_match.c"

char* extract_path_from_vips_error(char* error_str){
  char* pattern = "VipsForeignLoad: \"[^/]+[/[^/]+]*\" is not a known file format";
  return extract_match(error_str, pattern);
}

#ifdef TEST_EXTRACT_PATH_FROM_VIPS_ERROR
#include <assert.h>
int main(){
  char* error_str = "VipsForeignLoad: \"duplicates/119_a.jpg\" is not a known file format";
  char* path = extract_path_from_vips_error(error_str);
  puts(path);
  free(path);
  return EXIT_SUCCESS;
}
#endif
