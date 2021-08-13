/*  generate_nonduplicates.c
 *
 *  Copy all the files from <SOURCE_DIR> to <TARGET_DIR>, making two copies of
 *  each file, with suffixes "_a" and "_b". The basename of the second copy
 *  will be changed to match the previous element (wrapping at the end), so
 *  that a/b pairs with identical basenames are nonduplicates (assuming the
 *  source directory contains only unique files).
 *
 *  Compile
 * 
 *  gcc generate_nonduplicates -o generate_nonduplicates -luuid
 *
 *
 *  Usage
 *
 *  ./generate_nonduplicates <SOURCE_DIR> <TARGET_DIR>
 *
 *
 *  Compile Test
 *
 *  gcc generate_nonduplicates -o test-generate-nonduplicates -luuid
 *    -DTEST_GENERATE_NONDUPLICATES
 * 
 * 
 *  Test
 * 
 *  ./test-generate-nonduplicates
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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <assert.h>
#include <fcntl.h>

#include "join_dir_to_name.c"
#include "copy.c"
#include "uuid.c"
#include "split.c"

void generate_nonduplicates(char source_dir[SZ_PATH], char target_dir[SZ_PATH]){
  DIR* d = opendir(source_dir);
  int fd;
  struct dirent* e;
  struct stat stat;
  char name[SZ_NAME], ext[SZ_NAME], in_path[SZ_PATH]={0}, out_path[SZ_PATH]={0},
    out_path_a[SIZE_PATH]={0}, out_path_b[SIZE_PATH]={0},
    name_wext_a[3*SZ_NAME]={0}, name_wext_b[3*SZ_NAME]={0};
  if(d){
    while((e = readdir(d))){
      join_dir_to_name(in_path, source_dir, e->d_name);
      if(0>(fd = open(in_path, O_RDONLY)) || fstat(fd, &stat)){
        fprintf(stderr, "");
        exit(EXIT_FAILURE);
      }
      if(S_ISREG(stat.st_mode)){
        split(name, ext, in_path);
        snprintf(name_wext_a, 3*SZ_NAME, "%s_a.%s", name, ext);
        snprintf(name_wext_a, 3*SZ_NAME, "%s_b.%s", name, ext);
        join_dir_to_name(out_path_a, target_dir, name_wext_a);
        join_dir_to_name(out_path_b, target_dir, name_wext_b);
        if(copy(in_path, out_path_a) || copy(in_path, out_path_b)){
          close(fd);
          closedir(d);
          fprintf(stderr, "Couldn't copy file %s", in_path);
          return(EXIT_FAILURE);
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

#ifdef TEST_GENERATE_NONDUPLICATES
int main(){
  char source_dir_name[SZ_NAME];
  {
    char out[uuid_str_len]={0};
    snprintf(source_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }
  if(mkdir(source_dir_name, 0777)){
    fprintf(stderr, "Failed to create directory %s", source_dir_name);
    exit(EXIT_FAILURE);
  } 
  char test_path_1[SZ_PATH], file_name_2[SZ_PATH];
}
#else
int main(int argc, char* argv[]){
  if(argc!=3 || *argv[1] || *argv[2]){
    fprintf(stderr, "Usage: %s <SOURCE_DIR> <TARGET_DIR>", argv[0]);
    return(EXIT_FAILURE);
  } 
  generate_nonduplicates(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif