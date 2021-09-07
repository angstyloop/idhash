/*  generate_nonduplicates.c
 *
 *  Copy all the files from <SOURCE_DIR> to <TARGET_DIR>, making two copies of
 *  each file, with suffixes "_a" and "_b" (before prefix). The basename of the 
 *  second copy will be changed to match the previous element (wrapping at the
 *  end), so that a/b pairs with identical basenames are nonduplicates
 *  (assuming the source directory contains only unique files).
 *
 *  COMPILE
 * 
 *  gcc  generate_nonduplicates.c -o generate-nonduplicates -luuid \
 *  -DRUN_GENERATE_NONDUPLICATES
 *
 *
 *  USAGE
 *
 *  ./generate_nonduplicates <SOURCE_DIR> <TARGET_DIR>
 *
 *
 *  COMPILE TEST
 *
 *  gcc generate_nonduplicates.c -o test-generate-nonduplicates -luuid
 *    -DTEST_GENERATE_NONDUPLICATES
 * 
 * 
 *  TEST
 * 
 *  ./test-generate-nonduplicates
 * 
 */

#ifndef STDLIB_H
#  define GUARD_STDLIB
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define GUARD_STDIO
#  include <stdio.h>
#endif

#ifndef UUID_H
#  define GUARD_UUID
#  include <uuid/uuid.h>
#endif

#ifndef UNISTD_H
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

typedef struct file_info file_info;
struct file_info {
  int fd;
  struct stat stat;
};

int open_and_stat(file_info* f_info, char* path);

file_info* file_info_init(file_info* f_info, char path[SZ_PATH]){
  return open_and_stat(f_info, path) ? 0 : f_info;
}

file_info* file_info_zero(file_info* f_info){
  memset(f_info, 0, sizeof(file_info));
  return f_info;
}

void copy_file_from_to(char* source, char* target){
  if(copy(source, target)){
    fprintf(stderr, "Couldn't copy file %s to %s.", source, target);
    exit(EXIT_FAILURE);
  } 
}

typedef struct dirent_info dirent_info;
struct dirent_info {
  DIR* dirp;
  struct dirent* direntp;
};

void open_dir(DIR** dirp, char* dir_name);

dirent_info* dirent_info_init(dirent_info* d_info, char* dir_name){
  open_dir(&d_info->dirp, dir_name);
  return d_info;
}

dirent_info* dirent_info_zero(dirent_info* d_info){
  memset(d_info, 0, sizeof(dirent_info));
  return d_info;
}

int open_and_stat(file_info* f_info, char* path) {
  if(0>(f_info->fd = open(path, O_RDONLY)) 
    || fstat(f_info->fd, &f_info->stat)){
    fprintf(stderr, "Failed to open and stat file %s\n", path);
    return 1;
  } else return 0;
}

typedef struct file_path file_path; 
struct file_path {
  char path[SZ_PATH];
  char dir[SZ_PATH];
  char name[SZ_NAME];
  char ext[SZ_NAME];
  char name_wext[2*SZ_NAME];
};

file_path* file_path_init(
  file_path* f_path,
  char dir[SZ_PATH],
  char name_wext[2*SZ_NAME])
{
  strncpy(f_path->dir, dir, SZ_PATH);
  strncpy(f_path->name_wext, name_wext, 2*SZ_NAME);
  join_dir_to_name(f_path->path, f_path->dir, f_path->name_wext);
  split(f_path->name, f_path->ext, f_path->path);
  return f_path;
}

file_path* file_path_zero(file_path* f_info){
  memset(f_info, 0, sizeof(file_path));
  return f_info;
}

dirent_info* dirent_info_next_entry(dirent_info* d_infop){
  if(!(d_infop->direntp = readdir(d_infop->dirp))){
    fprintf(stderr, "All dirents have been read.");
    exit(EXIT_FAILURE);
  }
  return d_infop;
}

int next_regular_file(
  file_info* f_infop,
  dirent_info* d_infop, 
  file_path* f_pathp,
  int offset)
{
  // Return 0 if offset is negative, to avoid an infinite loop.
  if(0 > offset) return 0;

  // Skip the first @offset dirents, and then skip all non-regular files. If
  // the offset is 0, and the given dirent_info has a dirent that is already
  // a regular file, does nothing.
  struct dirent* direntp=0;
  for(;;){

    if(offset > 0){
      --offset;
    } else if(S_ISREG(f_infop->stat.st_mode)){
      break;
    }

    // Free the previous directory entry's file descriptor.
    close(f_infop->fd);

    // Return 0 when there are no more directory entries (or an error).
    if(!(direntp = readdir(d_infop->dirp)))
      return 0;
    d_infop->direntp = direntp;

    // Form input path to the next file.
    file_path_init(f_pathp, f_pathp->dir, d_infop->direntp->d_name);

    // Open and stat the next file. Return 0 on failure to open/stat. Save 
    // the fd and stat in the object pointed to by f_infop.
    if(!file_info_init(f_infop, f_pathp->path)) 
      return 0;
  }

  // Return 1 by default
  return 1;
}

/* Open directory named dir_name. Save the DIR pointer at the address 
 * pointed to by dirp.
 */
void open_dir(DIR** dirp, char* dir_name){
  if(!(*dirp = opendir(dir_name))){
    fprintf(stderr, "Couldn't open directory %s.", dir_name);
    exit(EXIT_FAILURE);
  }
}

/* Join @name to extension @ext, separated by separator @sep. Write the result
 * to output buffer @name_wext.
 */
char* join_name_to_ext(
  char name_wext[3*SZ_NAME],
  char name[SZ_NAME], 
  char ext[SZ_NAME],
  char sep[SZ_NAME])
{
  snprintf(name_wext, 3*SZ_NAME, "%s%s%s", name, sep, ext);
  return name_wext;
}

// Defined in file scope, so that atexit handler can clean up.
file_info prev_f_info={0}, f_info={0}, first_f_info={0}; 
dirent_info d_info={0};

// An atexit handler responsible for closing any open file descriptors or DIR*.
// Assumes the fd and DIR* objects are non-zero/non-null if and only if they are
// open, and need to be closed.
static void close_files_and_dirs(){
  if(f_info.fd) close(f_info.fd);
  if(prev_f_info.fd) close(prev_f_info.fd);
  if(d_info.dirp) closedir(d_info.dirp); 
};

/* Make two copies of each regular file in @source_dir. Put the copies in
 * @target_dir. Each pair of file copies will have a name based on the same
 * original filename, and each original filename gets exactly one pair of files.
 * The "a" copy is always a copy of the file its named after. The "b" copy is
 * always a copy of the file before that (using a circular version of the list
 * returned by readdir).
 */
static void generate_nonduplicates(
  char source_dir[SZ_PATH], 
  char target_dir[SZ_PATH])
{
  // Buffers for building the a and b output file names.
  char name_wext_a[3*SZ_NAME]={0}, name_wext_b[3*SZ_NAME]={0};

  // Buffers for building the a and b output paths.
  char out_path_a[SZ_PATH]={0}, out_path_b[SZ_PATH]={0};

  // file_path objects used as the previous and current indices in the loop
  // over dirents.
  file_path f_path={0}, prev_f_path={0}, first_f_path={0}; 

  // Schedule cleanup for any open file descriptors or DIR pointers at exit.
  atexit(close_files_and_dirs);

  // Init the dirent_info we'll use to walk through the dirent objects returned
  // by readdir in dirent_info_next_entry. 
  dirent_info_init(&d_info, source_dir);

  // Load the first dirent and its path, and open it. Save the file descriptor
  // and stat in a file_info object. Finally advance previous to the 
  // regular file. Save that first file's so we can wrap around at the end.
  dirent_info_next_entry(&d_info);
  file_path_init(&prev_f_path, source_dir, d_info.direntp->d_name);
  if(!file_info_init(&prev_f_info, prev_f_path.path)){
    fprintf(stderr, "Failed to open first file %s.\n", prev_f_path.path);
    exit(EXIT_FAILURE);
  } 

  next_regular_file(&prev_f_info, &d_info, &prev_f_path, 0);
  first_f_info = prev_f_info;
  first_f_path = prev_f_path;

  dirent_info_next_entry(&d_info);

  file_path_init(&f_path, source_dir, d_info.direntp->d_name);

  if(!file_info_init(&f_info, f_path.path)){
    fprintf(stderr, "Failed to open second file %s.\n", f_path.path);
    exit(EXIT_FAILURE);
  } 

  next_regular_file(&f_info, &d_info, &f_path, 0);

  join_name_to_ext(name_wext_b, f_path.name, f_path.ext, "_b.");
  join_dir_to_name(out_path_b, target_dir, name_wext_b);
  copy_file_from_to(prev_f_path.path, out_path_b);

  join_name_to_ext(name_wext_a, f_path.name, f_path.ext, "_a.");
  join_dir_to_name(out_path_a, target_dir, name_wext_a);
  copy_file_from_to(f_path.path, out_path_a);

  while(next_regular_file(&f_info, &d_info, &f_path, 1)){
    join_name_to_ext(name_wext_b, f_path.name, f_path.ext, "_b.");
    join_dir_to_name(out_path_b, target_dir, name_wext_b);
    copy_file_from_to(prev_f_path.path, out_path_b);

    join_name_to_ext(name_wext_a, f_path.name, f_path.ext, "_a.");
    join_dir_to_name(out_path_a, target_dir, name_wext_a);
    copy_file_from_to(f_path.path, out_path_a);

    close(prev_f_info.fd);
    prev_f_info = f_info;
    prev_f_path = f_path;
  }

  // last
  join_name_to_ext(name_wext_a, first_f_path.name, first_f_path.ext, "_a.");
  join_dir_to_name(out_path_a, target_dir, name_wext_a);
  copy_file_from_to(first_f_path.path, out_path_a);

  join_name_to_ext(name_wext_b, first_f_path.name, first_f_path.ext, "_b.");
  join_dir_to_name(out_path_b, target_dir, name_wext_b);
  copy_file_from_to(f_path.path, out_path_b);
}

#ifdef TEST_GENERATE_NONDUPLICATES
/* Check that generate_nonduplicates creates files in the target directory
 * with the expected names. Clean up temporary directories and files.
 */
int main(){
  char source_dir_name[SZ_NAME];
  {
    char out[UUID_STR_LEN]={0};
    snprintf(source_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }
  if(mkdir(source_dir_name, 0777)){
    fprintf(stderr, "Failed to create directory %s", source_dir_name);
    exit(EXIT_FAILURE);
  } 
  char test_path_1[SZ_PATH], test_path_2[SZ_PATH];
  join_dir_to_name(test_path_1, source_dir_name, "test-file-1.txt");
  join_dir_to_name(test_path_2, source_dir_name, "test-file-2.txt");

  FILE* f1=0, * f2=0;
  if((f1 = fopen(test_path_1, "w")) && (f2 = fopen(test_path_2, "w"))){
    fprintf(f1, "Hello, world!");
    fprintf(f2, "Hello, place!");
  } 
  if(f1){
    fclose(f1);
    f1=0;
  }
  if(f2){
    fclose(f2);
    f2=0;
  }
  char target_dir_name[SZ_NAME]={0};
  {
    char out[UUID_STR_LEN]={0};
    snprintf(target_dir_name, SZ_NAME, "test-dir-%s", gen_uuid_str(out));
  }
  if(mkdir(target_dir_name, 0777)){
    fprintf(stderr, "Failed to created directory %s.\n", target_dir_name);
    exit(EXIT_FAILURE);
  } 
  generate_nonduplicates(source_dir_name, target_dir_name);

  // Form the expected paths for the output files, and test they are correct.
  char exp_path_1_a[SZ_PATH]={0}, exp_path_1_b[SZ_PATH]={0}, 
    exp_path_2_a[SZ_PATH]={0}, exp_path_2_b[SZ_PATH]={0};
  join_dir_to_name(exp_path_1_a, target_dir_name, "test-file-1_a.txt");
  join_dir_to_name(exp_path_1_b, target_dir_name, "test-file-1_b.txt");
  join_dir_to_name(exp_path_2_a, target_dir_name, "test-file-2_a.txt");
  join_dir_to_name(exp_path_2_b, target_dir_name, "test-file-2_b.txt");

  assert((f1 = fopen(exp_path_1_a, "r")) && (f2 = fopen(exp_path_1_b, "r")));
  if(f1){
    fclose(f1);
    f1=0;
  }
  if(f2){
    fclose(f2);
    f2=0;
  }

  assert((f1 = fopen(exp_path_2_a, "r")) && (f2 = fopen(exp_path_2_b, "r")));
  if(f1){
    fclose(f1);
    f1=0;
  }
  if(f2){
    fclose(f2);
    f2=0;
  }

  // Clean up test files and dirs.
  if(  remove(exp_path_1_a) || remove(exp_path_1_b)    || remove(exp_path_2_a)
    || remove(exp_path_2_b) || remove(target_dir_name) || remove(test_path_1)
    || remove(test_path_2)  || remove(source_dir_name)){

    fprintf(stderr, "Failed to delete temporary files and directories.");
    exit(EXIT_FAILURE);
  } 
  return EXIT_SUCCESS;
}
#elif defined(RUN_GENERATE_NONDUPLICATES)
int main(int argc, char* argv[]){
  if(argc!=3 || !*argv[1] || !*argv[2]){
    fprintf(stderr, "Usage: %s <SOURCE_DIR> <TARGET_DIR>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  generate_nonduplicates(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif
