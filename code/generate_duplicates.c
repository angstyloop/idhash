/* generate_duplicates.c
 *
 * Copy all regular files in a source directory to a target directory. Make
 * two copies of each source file. Add suffix to each name ( _a or _b) to the 
 * name, before the extension.
 *
 * E.g.
 *
 * Renames "foo.bar" to "foo_a.bar" and "foo_b.bar".
 *
 *
 * Compile
 *
gcc generate_duplicates.c -DTEST_GENERATE_DUPLICATES -luuid -o test-generate-duplicates -Wall -g
 *
gcc generate_duplicates.c -o generate-duplicates -Wall -g -luuid
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
#  define GUARD_STDLIB
#  include <stdlib.h>
#endif

#ifndef GUARD_STDIO
#  define GUARD_STDIO
#  include <stdio.h>
#endif

#ifndef GUARD_UUID
#  define GUARD_UUID
#  include <uuid/uuid.h>
#endif

#ifndef GUARD_UNISTD
#  define GUARD_UNISTD
#  include <unistd.h>
#endif

#ifndef SZ_NAME
#  define SZ_NAME 256
#endif

#ifndef SZ_PATH
#  define SZ_PATH 4096
#endif

#ifndef PATH_SEP
#  define PATH_SEP '/'
#endif

#ifndef TYPES_H
#  define TYPES_H
#  include <sys/types.h>
#endif

#ifndef STAT_H
#  define STAT_H
#  include <sys/stat.h>
#endif

#ifndef DIRENT_H
#  define DIRENT_H
#  include <dirent.h>
#endif

#ifndef ASSERT_H
#  define ASSERT_H
#  include <assert.h>
#endif

#ifndef FCNTL_H
#  define FCNTL_H
#  include <fcntl.h>
#endif

#ifndef JOIN_DIR_TO_NAME_H
#  define JOIN_DIR_TO_NAME_H
#  include "join_dir_to_name.c"
#endif

#ifndef COPY_H
#  define COPY_H
#  include "copy.c"
#endif

#ifndef UUID_H
#  define UUID_H
#  include "uuid.c"
#endif

#ifndef SPLIT_H
#  define SPLIT_H
#  include "split.c"
#endif

void generate_duplicates(
  char source_dir[SZ_PATH],
  char target_dir[SZ_PATH]) 
{
  // Open source dir
  DIR* d = opendir(source_dir);
  int fd;
  struct dirent* e;
  struct stat stat;
  char name[SZ_NAME]={0}, ext[SZ_NAME]={0}, in_path[SZ_PATH]={0},
    out_path_a[SZ_PATH]={0}, out_path_b[SZ_PATH]={0}, 
    name_wext_a[3*SZ_NAME]={0}, name_wext_b[3*SZ_NAME]={0};
  if(d){
    // Loop over directory entries.
    while((e = readdir(d))){
      // Form input file path from dir and entry name.
      join_dir_to_name(in_path, source_dir, e->d_name);
      // Stat the entry, so we can get the type from the stat mode.
      if(-1 == (fd = open(in_path, O_RDONLY)) || fstat(fd, &stat)){
        fprintf(stderr, "Failed to inspect %s\n", e->d_name);
        exit(EXIT_FAILURE);
      }
      // Only copy regular files. Check using S_ISREG macro and the stat mode.
      if(S_ISREG(stat.st_mode)){
        // Split into name and extension. e.g. "/foo/bar.baz" -> "bar" and "baz"
        split(name, ext, in_path);
        // Make output names with _a and _b extensions in the names.
        snprintf(name_wext_a, 3*SZ_NAME, "%s_a.%s", name, ext);
        snprintf(name_wext_b, 3*SZ_NAME, "%s_b.%s", name, ext);
        join_dir_to_name(out_path_a, target_dir, name_wext_a);
        join_dir_to_name(out_path_b, target_dir, name_wext_b);
        // Use constructed input and output paths to make two copies.
        if(copy(in_path, out_path_a) || copy(in_path, out_path_b)){
          close(fd);
          closedir(d);
          fprintf(stderr, "Couldn't copy file %s.", in_path);
          exit(EXIT_FAILURE);
        }
      }
      close(fd);
    }
    closedir(d);
  }else{
    fprintf(stderr, "Couldn't open directory.");
    exit(EXIT_FAILURE);
  }
}

#ifdef TEST_GENERATE_DUPLICATES
//Usage: ./test-generate-duplicates
int main(){
  // generate a uuid in order to create a unique test source directory name 
  char source_dir_name[SZ_NAME]={0};
  {
    char out[UUID_STR_LEN]={0};
    snprintf(source_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }

  // create test source directory (should not exist yet, thanks to uuid).
  if(mkdir(source_dir_name, 0777)){
    fprintf(stderr, "Failed to create directory %s.", source_dir_name);
    exit(EXIT_FAILURE);
  }

  // create test files in source directory
  char test_path_1[SZ_PATH]={0}, test_path_2[SZ_PATH]={0};
  join_dir_to_name(test_path_1, source_dir_name, "test-file-1.txt");
  join_dir_to_name(test_path_2, source_dir_name, "test-file-2.txt");

  // Print some text to the test source files, then close them.
  FILE* f1=NULL, * f2=NULL;
  if((f1 = fopen(test_path_1, "w")) && (f2 = fopen(test_path_2, "w"))){
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

  // Create a unique test target directory name. this is just the name of the
  // dir, not the full path to the dir.
  char target_dir_name[SZ_NAME]={0};
  {
    char out[UUID_STR_LEN];
    snprintf(target_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }

  // Create test target directory (should not exist yet, thanks to uuid).
  if(mkdir(target_dir_name, 0777)){
    fprintf(stderr, "Failed to create directory %s.\n", target_dir_name);
    exit(EXIT_FAILURE);
  }

  // Generate duplicates in target dir.
  generate_duplicates(source_dir_name, target_dir_name);

  // Form the expected names.
  char exp_path_1_a[SZ_PATH]={0}, exp_path_1_b[SZ_PATH]={0},
   exp_path_2_a[SZ_PATH]={0}, exp_path_2_b[SZ_PATH]={0};
  join_dir_to_name(exp_path_1_a, target_dir_name, "test-file-1_a.txt");
  join_dir_to_name(exp_path_1_b, target_dir_name, "test-file-1_b.txt");
  join_dir_to_name(exp_path_2_a, target_dir_name, "test-file-2_a.txt");
  join_dir_to_name(exp_path_2_b, target_dir_name, "test-file-2_b.txt");

  // Assert the files in target dir were created with the expected names.
  assert((f1 = fopen(exp_path_1_a, "r")) && (f2 = fopen(exp_path_1_b, "r")));
  if(f1){
    fclose(f1); 
    f1=NULL;
  }
  if(f2){
    fclose(f2);
    f2=NULL;
  }
  assert((f2 = fopen(exp_path_2_a, "r")) && (f2 = fopen(exp_path_2_b, "r")));
  if(f1){
    fclose(f1); 
    f1=NULL;
  }
  if(f2){
    fclose(f2);
    f2=NULL;
  }

  // Clean up.
  if(remove(exp_path_1_a) || remove(exp_path_1_b) || remove(exp_path_2_a)
    || remove(exp_path_2_b) || remove(target_dir_name) || remove(test_path_1)
    || remove(test_path_2) || remove(source_dir_name)){
    fprintf(stderr, "Failed to delete temporary files and directories.");
    exit(EXIT_FAILURE);
  }
  puts("OK");
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
