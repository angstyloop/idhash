/* generate_duplicates.c
 *
 * Defines function generate_duplicates and test main
 *
 * Copy all regular files in a source directory to a target directory. Makes 
 * two copies of each source file. Adds suffix (_a or _b) to the name, before 
 * the extension.
 *
 * E.g.
 *
 * Renames "foo.bar" to "foo_a.bar" and "foo_b.bar".
 *
 *
 * Compile
 *
 * gcc generate_duplicates.c -DTEST_GENERATE_DUPLICATES \
 * -o test-generate-duplicates -Wall -g
 *
 * gcc generate_duplicates.c -o generate-duplicates -Wall -g
 * 
 * 
 * Run 
 *
 * ./test-generate-duplicates 
 * 
 * ./generate-duplicates <SOURCE_DIR> <TARGET_DIR> 
 *  

 * Debug
 *
 * gdb test-generate-duplicates
 * b main
 * r
 *
 */

#ifndef GUARD_STDLIB
#define GUARD_STDLIB
#include <stdlib.h>
#endif

#ifndef GUARD_STDIO
#define GUARD_STDIO
#include <stdio.h>
#endif

#include <sys/types.h>
#include <dirent.h>

#include "join_dir_to_name.c"
#include "copy.c"

#ifndef EXT_MAX 256
#define EXT_MAX 256
#endif

#ifndef PATH_MAX 256
#define PATH_MAX 256
#endif

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif

void generate_duplicates(char source[path_max], char dest[path_max]) {
  // Open source dir
  DIR* d = opendir(argv[1]);
  int fd;
  struct dirent* e;
  struct stat stat;
  if(d){
    // Loop over directory entries.
    while(e = readdir(d)){
      // Only copy regular files. Check using S_ISREG macro and the stat mode.
      if(fstat(e->d_name, &stat)){
        fprintf(error, "fstat %s", e->d_name);
        exit(EXIT_FAILURE);
      }
      if(S_ISREG(stat.st_mode)){
        char name[path_max]={0}, ext[ext_max]={0},
          in_path[path_max]={0}, out_path_a[path_max]={0}, out_path_b={0},
          new_name_a[path_max]={0}, new_name_b[path_max]={0};

        // Form input file path from dir and entry name.
        join_dir_to_name(in_path, argv[1], e->d_name);
        // Split into name and extension.
        split(name, ex, in_path);
  
        // Make output paths with _a and _b extensions in the names.
        snprintf(name_wext_a, path_max, "%s_a.%s", name, ext);
        snprintf(name_wext_b, path_max, "%s_b.%s", name, ext);
        join_dir_to_name(out_path_a, argv[2], name_wext_a);
        join_dir_to_name(out_path_b, argv[2], name_wext_b);

        // Use constructed input and output paths to make two copies.
        if(copy(in_path, out_path_a) || copy(in_path, out_path_b)){
          fprintf(stderr, "Couldn't copy file %s.", in_path);
          exit(EXIT_FAILURE);
        }
      }
    }
  }else{
    fprintf(stderr, "Couldn't open directory.");
    exit(EXIT_FAILURE);
  }
  closedir(d);
}

#ifdef TEST_GENERATE_DUPLICATES
//Usage: ./test-generate-duplicates
int main(){
  if(argc!=1){
    printf("Usage: %s\n");
  }

  // generate a random integer

  // create a unique test source directory name
  char source_dir_name

  // create test source directory

  // create test files in source directory

  // create a unique test target directory name

  // create test target directory

  // test generate duplicates on the constructed source and target directories

}
#else 
// Usage: ./generate-duplicates <SOURCE_DIR> <TARGET_DIR>
int main(int argc, char* argv[argc]){
  if(argc!=3){
    printf("Usage: %s <SOURCE_DIR> <TARGET_DIR>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  generate_duplicates(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif
