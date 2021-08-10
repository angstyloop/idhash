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

#ifndef GUARD_UUID
#  define GUARD_UUID
#  include <uuid/uuid.h>
#endif


#include <sys/types.h>
#include <dirent.h>

#include "join_dir_to_name.c"
#include "copy.c"

#ifndef EXT_MAX 256
#define EXT_MAX 256
#endif

#ifndef PATH_MAX 4096
#define PATH_MAX 4096
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

  // generate a uuid in order to create a unique test source directory name 
  char source_dir_name[PATH_MAX]={0};
  {
    char out[UUID_STR_LEN]={0};
    snprintf(source_dir_name, PATH_MAX, "test-dir-%s", uuid(out));
  }

  // create test source directory (should not exist yet, thanks to uuid).
  if(mkdir(source_dir_name, S_IRWU)){
    fprintf(stderr, "Failed to create directory %s.", source_dir_name);
  }

  // create test files in source directory
  char file_name_1[PATH_MAX]={0}, file_name_2[PATH_MAX]={0};
  snprintf(file_name_1, PATH_MAX, "%s/test-file-1.txt", source_dir_name);
  snprintf(file_name_2, PATH_MAX, "%s/test-file-2.txt", source_dir_name);

  FILE* f1=NULL, * f2=NULL;
  if((f1 = fopen(fname1, "w") && (f2 = fopen(fname2, "w"){
    fprintf(f1, "Hello, world!");
    fprintf(f2, "Hello, place!");
  }

  if(f1){
    fclose(f1); 
    f1=NULL;
  }
   
  if(f2){
    fclose(f2);
    f2=NULL;
  }

  // create a unique test target directory name
  char target_dir_name[PATH_MAX]={0};
  {
    char out[UUID_STR_LEN];
    snprintf(target_dir_name, PATH_MAX, "test-dir-%s", uuid(out));
  }

  // create test target directory (should not exist yet, thanks to uuid).
  if(mkdir(target_dir_name, S_IRWU)){
    fprintf(stderr, "Failed to create directory %s.", target_dir_name);
  }

  // generate duplicates in target dir
  generate_duplicates(source_dir_name, target_dir_name);

  char exp_name_1_a[PATH_MAX]={0}, exp_name_1_b[PATH_MAX],
   exp_name_2_a[PATH_MAX], exp_name_2_b[PATH_MAX];

  snprintf(exp_name_1_a, PATH_MAX, "%s/test-file-1_a.txt",
    target_dir_name);

  snprintf(exp_name_1_b, PATH_MAX, "%s/test-file-1_b.txt",
    target_dir_name);

  snprintf(exp_name_2_a, PATH_MAX, "%s/test-file-2_a.txt",
    target_dir_name);

  snprintf(exp_name_2_b, PATH_MAX, "%s/test-file-2_b.txt",
    target_dir_name);

  assert((f1 = fopen(exp_name_1_a, "r")) && (f2 = fopen(exp_name_1_b, "r")));

  if(f1){
    fclose(f1); 
    f1=NULL;
  }
   
  if(f2){
    fclose(f2);
    f2=NULL;
  }

  assert((f2 = fopen(exp_name_2_a, "r")) && (f2 = fopen(exp_name_2_b, "r")));

  if(f1){
    fclose(f1); 
    f1=NULL;
  }
   
  if(f2){
    fclose(f2);
    f2=NULL;
  }

  return EXIT_SUCCESS;
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
