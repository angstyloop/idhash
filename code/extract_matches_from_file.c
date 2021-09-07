/*
gcc extract_matches_from_file.c -o test-extract-matches-from-file -DTEST_EXTRACT_MATCHES_FROM_FILE -g -Wall
*/

#include "extract_match.c"

#ifndef SZ_PATH
#  define SZ_PATH 4096
#endif

void extract_matches_from_file(
  char* pattern,
  char* in_path, 
  char* out_path)
{
  FILE* in_fp=0, * out_fp=0;
  if(!(in_fp = fopen(in_path, "r"))){
    fprintf(stderr, "Failed to open input file %s.", in_path);
    exit(EXIT_FAILURE);
  } 
  if(!(out_fp = fopen(out_path, "w"))){
    fprintf(stderr, "Failed to open output file %s.", out_path);
    exit(EXIT_FAILURE);
  } 
  char* line=0, * match=0;
  size_t len=0;
  ssize_t nread=0;
  while (-1<(nread = getline(&line, &len, in_fp))){
    match = extract_match(line, pattern);
    if(match){
      fprintf(out_fp, "%s\n", match);
      free(match);
    }
  }
  fclose(in_fp);
  fclose(out_fp);
}

#ifdef TEST_EXTRACT_MATCHES_FROM_FILE
int main(int argc, char* argv[argc]){
  if(argc!=4){
    fprintf(stderr, "Usage: %s <PATTERN> <INPUT_FILE> <OUTPUT_FILE>\n", 
      argv[0]);
    exit(EXIT_FAILURE);
  } 
  extract_matches_from_file(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
#endif
