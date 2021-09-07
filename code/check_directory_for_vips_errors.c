/*
gcc check_directory_for_vips_errors.c -o test-check-directory-for-vips-errors -DTEST_CHECK_DIRECTORY_FOR_VIPS_ERRORS -g -Wall `pkg-config vips --cflags --libs`

Try to load each regular file in @dir into VIPS. Write errors to an error file.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <vips/vips.h>
#include "join_dir_to_name.c"

void check_directory_for_vips_errors(char dir[SZ_PATH]){
  VipsImage* in;
  // redirect stderr to error file 
  // loop over files in dir
  // run a vips command
  char* error_fname = "error_file.txt";
  DIR* d; 
  if(!(d = opendir(dir))){
    fprintf(stderr, "Failed to open directory %s.", dir);
    exit(EXIT_FAILURE);
  }
  // Define an output error file.
  FILE* efp=0;
  if(!(efp = fopen(error_fname, "w"))){
    fprintf(stderr, "Failed to open error file %s.\n", error_fname);
    exit(EXIT_FAILURE);
  } 
  struct dirent* e={0};
  struct stat stat={0};
  char path[2*SZ_PATH]={0};
  int fd;
  while((e = readdir(d))){
    // Form the full path to the directory entry.
    join_dir_to_name(path, dir, e->d_name);
    // Stat to check its type. 
    if(-1 == (fd = open(path, O_RDONLY)) || fstat(fd, &stat)){
      fprintf(stderr, "Unable to stat %s to check its " 
        "type.\n", path);
      exit(EXIT_FAILURE);
    } 
    // Ignore entries that are not regular files.
    if(!S_ISREG(stat.st_mode))
      continue;
    // Regular file. Try to open and close with VIPS (writing errors to 
    // the error file). 
    if(!(in = vips_image_new_from_file(path, NULL))){
      fprintf(efp, "%s", vips_error_buffer());
      vips_error_clear();
    } else {
      g_object_unref(in);
    }
  }
  // Clean up.
  fclose(efp);
  closedir(d);
}

#ifdef TEST_CHECK_DIRECTORY_FOR_VIPS_ERRORS
int main(int argc, char* argv[argc]){
  if(argc!=2){
    fprintf(stderr, "Usage: %s <DIRECTORY>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  if(VIPS_INIT(argv[0]))
    vips_error_exit(NULL);
  check_directory_for_vips_errors(argv[1]);
  return EXIT_SUCCESS;
}
#endif
