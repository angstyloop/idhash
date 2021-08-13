/* generate_numbered_files.c
 *
 * Copy all the files in a source directory to a target directory, renaming the
 * files using a counter {0, 1, 2, ...}, but preserving the extension of the
 * file, if it has one. The order of the files is the order they are listed
 * with readdir, and shouldn't be depended on for sorting.
 *
 * Compile
 *
 * gcc generate_numbered_files.c -o generate-numbered-files -luuid
 * 
 *
 * Usage
 *
 * ./generate_numbered_files <SOURCE_DIR> <TARGET_DIR> <START_INDEX>
 * 
 *
 * Compile test
 * 
 * gcc generate_numbered_files.c -DTEST_GENERATE_NUMBERED_FILES -luuid
 *   -o test-generate-numbered-files
 * 
 *
 * Test
 *
 * ./test-generate-numbered-files
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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <assert.h>
#include <fcntl.h>

#include "join_dir_to_name.c"
#include "copy.c"
#include "uuid.c"
#include "split.c"

#define SZ_NAME 256
#define SZ_PATH 4096

#ifndef PATH_SEP
#define PATH_SEP '/'
#endif

void generate_numbered_files(char source_dir[SZ_PATH], char target_dir[SZ_PATH],
  int start_index) {
  //Open source dir
  DIR* d = opendir(source_dir);
  int fd, i = start_index > 0 ? start_index : 0;
  struct dirent* e;
  struct stat stat;
  char old_name[SZ_NAME]={0}, ext[SZ_NAME]={0}, new_name[2*SZ_NAME], 
    in_path[SZ_PATH]={0}, out_path[SZ_PATH]={0};
  if(d){
    while((e = readdir(d))){
      join_dir_to_name(in_path, source_dir, e->d_name);
      if(0>(fd = open(in_path, O_RDONLY)) || fstat(fd, &stat)){
        fprintf(stderr, "Failed to open and/or stat file.");
        exit(EXIT_FAILURE);
      }
      if(S_ISREG(stat.st_mode)){
        split(old_name, ext, in_path);
        snprintf(new_name, 2*SZ_NAME, "%d%s%s", i++, *ext?".":"", ext);
        join_dir_to_name(out_path, target_dir, new_name);
        if(copy(in_path, out_path)){
          close(fd);
          closedir(d);
          fprintf(stderr, "Failed to copy %s to %s.\n", in_path, out_path);
          exit(EXIT_FAILURE);
        }
      }
      close(fd);
    }
    closedir(d);
  }else{
    close(fd);
    fprintf(stderr, "Couldn't open directory.");
    exit(EXIT_FAILURE);
  }
}

#ifdef TEST_GENERATE_NUMBERED_FILES
int main(){
  char source_dir_name[SZ_NAME];
  {
    char out[UUID_STR_LEN]={0};
    snprintf(source_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }
  if(mkdir(source_dir_name, 0777)){
    fprintf(stderr, "Failed to create directory %s.\n", source_dir_name);
    exit(EXIT_FAILURE);
  }
  char test_path_1[SZ_PATH]={0}, test_path_2[SZ_PATH]={0};
  join_dir_to_name(test_path_1, source_dir_name, "test-file-1.txt");
  join_dir_to_name(test_path_2, source_dir_name, "test-file-2.txt");
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
  char target_dir_name[SZ_NAME]={0};
  {
    char out[UUID_STR_LEN];
    snprintf(target_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }
  if(mkdir(target_dir_name, 0777)){
    fprintf(stderr, "Failed to create directory %s.\n", target_dir_name);
    exit(EXIT_FAILURE);
  }
  generate_numbered_files(source_dir_name, target_dir_name, 1);
  char exp_name_1[SZ_NAME], exp_name_2[SZ_NAME];
  join_dir_to_name(exp_name_1, target_dir_name, "1.txt");
  join_dir_to_name(exp_name_2, target_dir_name, "2.txt");
  assert((f1 = fopen(exp_name_1, "r")) && (f2 = fopen(exp_name_2, "r")));
  if(f1){
    fclose(f1);
    f1=NULL;
  }
  if(f2){
    fclose(f2);
    f2=NULL;
  }
  if(remove(exp_name_1) || remove(exp_name_2) || remove(target_dir_name)
    || remove(test_path_1) || remove(test_path_2) || remove(source_dir_name)){
    fprintf(stderr, "Failed to delete temporary files and directories.");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
#else
int main(int argc, char* argv[argc]) {
  if(argc!=3){
    fprintf(stderr, "Usage: %s <SOURCE_DIR> <TARGET_DIR> <START_INDEX>\n", 
      argv[0]);
    exit(EXIT_FAILURE);
  }
  generate_numbered_files(argv[1], argv[2], 1);
  return EXIT_SUCCESS;
}
#endif
