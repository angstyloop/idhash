/*
gcc collect_vips_load_errors_for_directory.c -o test-collect-vips-load-errors-for-directory -DTEST_COLLECT_VIPS_LOAD_ERRORS_FOR_DIRECTORY -g -Wall
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include "join_dir_to_name.c"

void collect_vips_load_errors_for_directory(char[SZ_PATH] dir){
  VipsImage* in;
  // redirect stderr to error file 
  // loop over files in dir
    // run a vips command
    char* error_fname = "error_file.txt";
    DIR* d; 
    if((d = opendir(dir))){
      fprintf(stderr, "Failed to open directory %s.", dir);
      exit(EXIT_FAILURE);
    }
    // Define an output error file.
    char* error_filep=0;
    if((error_filep = fopen(error_fname, "w"))){
      fprintf(stderr, "Failed to open error file %s.\n", error_file);
      exit(EXIT_FAILURE);
    } 
    // Use dup2 to redirect stdout to error file.
    dup2(fno(error_filep), STDERR_FILENO);
    struct dirent* e={0};
    struct stat stat={0};
    char path[2*SZ_PATH]={0};
    while((e = readdir(d))){
      // Form the full path to the directory entry.
      join_dir_to_name(path, dir, name);
      // Stat to check its type. 
      if(fstat(path, &stat)){
        fprintf(stderr, "Unable to stat %s to check its 
          type.\n", path);
        exit(EXIT_FAILURE);
      } 
      // Ignore entries that are not regular files.
      if(!S_ISREG(stat.st_mode))
        continue;
      // Regular file. Try to open and close with VIPS (writing errors to 
      // the error file).
      in = vips_image_new_from_file(path, NULL);
      g_object_unref(in);
    }
    // Clean up.
    fclose(error_filep);
    closedir(d);
}

#ifdef TEST_COLLECT_VIPS_LOAD_ERRORS_FOR_DIRECTORY
int main(int argc, char* argv[argc]){
  if(argc!=2){
    fprintf(stderr, "Usage: %s <DIRECTORY>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  if(VIPS_INIT(argv[0]))
    vips_error_exit(NULL);
  return EXIT_SUCCESS;
}
#endif
