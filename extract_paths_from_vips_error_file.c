/*
gcc extract_paths_from_vips_error_file.c -o test-extract-paths-from-vips-error-file -DTEST_EXTRACT_PATHS_FROM_VIPS_ERROR_FILE -Wall -g
*/

#ifndef EXTRACT_PATH_FROM_VIPS_ERROR_H
#  define EXTRACT_PATH_FROM_VIPS_ERROR_H
#  include "extract_path_from_vips_error.c"
#endif

#ifndef SZ_PATH
#  define SZ_PATH 4096
#endif

void extract_path_from_vips_error_file(char* in, char* out){
  FILE* fin=0, * fout=0;
  if(!((fin = fopen(in, "r")) && (fout = fopen(out, "w")))){
    fprintf(stderr, "fopen: %s %s\n", fin?in:"", fout?out:"");
    exit(EXIT_FAILURE);
  }
  char* line=0, * path=0;
  size_t n=0;
  ssize_t z=0;
  while(-1<(z = getline(&line, &n, fin))){
    if((path = extract_path_from_vips_error(line))){
      fprintf(fout, "%s\n", path);
      free(path);
    }
  }
  fclose(fin);
  fclose(fout);
}

#ifdef TEST_EXTRACT_PATHS_FROM_VIPS_ERROR_FILE
int main(int argc, char* argv[argc]){
  if(argc!=3){
    fprintf(stderr, "Usage: %s <INPUT_FILE> <OUTPUT_FILE>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  extract_path_from_vips_error_file(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif
