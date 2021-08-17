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
 *  gcc generate_nonduplicates.c -o generate-nonduplicates -luuid
 *
 *
 *  Usage
 *
 *  ./generate_nonduplicates <SOURCE_DIR> <TARGET_DIR>
 *
 *
 *  Compile Test
 *
 *  gcc generate_nonduplicates.c -o test-generate-nonduplicates -luuid
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

typedef struct file_info file_info;
struct file_info {
  int fd;
  stat stat;
};

file_info* file_info_init(file_info* file_info, char path[SZ_PATH]){
  return open_and_stat(&file_info, path) ? file_info : 0;
}

file_info* file_info_zero(file_info* file_info){
  memset(file_info, 0, sizeof(file_info));
  return file_info;
}

void copy_file_from_to(char* source, char* target){
  if(copy(source, target)){
    fprintf(stderr, "Couldn't copy file %s to %s.", source, target);
    exit(EXIT_FAILURE);
  } 
}

typedef struct dirent_info dirent_info;
struct dirent_info {
  DIR* dirp,
  struct dirent* direntp,
};

void open_dir(DIR** dirp, char* dir_name);

dirent_info* dirent_info_init(dirent_info* dirent_info, char* dir_name){
  open_dir(&dirent_info->dirp, dir_name);
  return dirent_info;
}

dirent_info* dirent_info_zero(dirent_info* d){
  memset(d, 0, sizeof(dirent_info));
  return d;
}

int open_and_stat(file_info* infop, char* path) {
  if(0>(infop->fd = open(path, O_RDONLY)) || fstat(infop->fd, statp)){
    fprintf(stderr, "Failed to open and stat file %s\n", path);
    return 1;
  } else {
    return 0;
  }
}

typedef struct file_path; 
struct file_path {
  char path[SZ_PATH];
  char dir[SZ_PATH];
  char name[SZ_NAME];
  char ext[SZ_NAME];
  char name_wext[2*SZ_NAME];
};

file_path* file_path_init(file_path* p, char* dir, char* name_wext){
  p->dir = dir;
  p->name_wext = name_wext;
  join_dir_to_name(p->path, p->dir, p->name_wext);
  split(p->name, p->ext, p->name_wext);
  return p;
}

file_path* file_path_zero(file_path* p){
  memset(p, 0, sizeof(file_path));
  return p;
}

dirent_info* dirent_info_next_entry(dirent_info* dirent_infop){
  if(!(dirent_infop->direntp = readdir(dirent_infop->dirp))){
    closedir(dirent_infop->dirp);
    fprintf(stderr, "Directory empty %s.", source_dir);
    exit(EXIT_FAILURE);
  }
  return dirent_infop;
}

int next_regular_file(
  file_info* file_infop,
  dirent_info* dirent_infop, 
  file_path* file_pathp)
{
  // Skip non-regular files.
  while(!S_ISREG(file_infop->stat.st_mode)){
    // Free the previous directory entry's file descriptor.
    close(file_infop->fd);

    // Return 0 when there are no more directory entries (or an error).
    if(!(*dirent_infop->direntp = readdir(dirent_infop->dirp))) 
      return 0;

    // Form input path to the next file.
    join_dir_to_name(file_pathp->path, file_pathp->dir, file_pathp->name);

    // Open and stat the next file. Return 0 on failure to open/stat.
    if(open_and_stat(file_infop, file_path->path))
      return 0;
  }

  // Return 1 on successful iteration.
  return 1;
}

/* Open directory named dir_name. Save the DIR pointer at the address pointed
 * to by dirp.
 */
void open_dir(DIR** dirp, char* dir_name){
  if(!(*dirp = opendir(dir_name))){
    fprintf(stderr, "Couldn't open directory %s.", dir_name);
    exit(EXIT_FAILURE);
  }
}

/* Join @name to extension @ext, separated by separator @sep. Write the result
 * to name_wext.
 */
char* join_name_to_ext(
  char name_wext[3*SZ_NAME],
  char name[SZ_NAME], 
  char ext[SZ_NAME],
  char sep[SZ_NAME])
{
  snprintf(name_wext, 3*SZ_NAME, "%s%s%s", name, sep, ext);
}

// Defined in file scope, so the at_exit handler function close_files_and_dirs
// can clean them up if they aren't zeroed.
file_info prev_file_info={0}, file_info={0};
dirent_info dirent_info={0};

// An at_exit handler responsible for closing any open file descriptors or DIR*.
// Assumes the fd and DIR* objects are non-zero/non-null if and only if they are
// open, and need to be closed.
static void close_files_and_dirs(){
  if(file_info.fd) close(file_info.fd);
  if(prev_file_info.fd) close(prev_file_info.fd)
  if(dirent_info.dirp) closedir(dirent_info.dirp); 
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
  file_path file_path={0}, prev_file_path={0}; 

  // Schedule cleanup for any open file descriptors or DIR pointers at exit.
  at_exit(close_files_and_dirs);

  // Init the dirent_info we'll use to walk through the dirent objects returned
  // by readdir in dirent_info_next_entry. 
  dirent_info_init(&dirent_info, source_dir);

  // Load the first dirent and its path, and open it.
  dirent_info_next_entry(&dirent_info);
  file_path_init(&prev_file_path, source_dir, dirent_info.direntp->d_name);
  if(!file_info_init(&prev_file_info, prev_file_path.path)){
    fprintf(stderr, "Failed to open first file %s.\n", prev_file_info.path);
    exit(EXIT_FAILURE);
  } 

  // Save the first dirent's path for later. It's used with the last dirent to 
  // form the last non-duplicate pair. 
  strncpy(first_in_path, SZ_PATH, prev_file_path.path);

  // Load the next dirent and its path, and open it.
  dirent_info_next_entry(&dirent_info);
  file_path_init(&file_path, source_dir, dirent_info.direntp->d_name);
  if(!file_info_init(&file_info, file_path.path)){
    fprintf(stderr, "Failed to open second file %s.\n", file_info.path);
    exit(EXIT_FAILURE);
  } 

  // With the previous and current indices of our "iterator" initialized, we 
  // can walk through the remaining dirents with a loop.
  while(next_regular_file(&file_info, &dirent_info, &file_path)){

    // Create output file b using the previous file's data and a modified 
    // version of the current file's name.
    join_name_to_ext(name_wext_b, file_path.name, file_path.ext, "_b.");
    join_dir_to_name(out_path_b, target_dir, name_wext_b);
    copy_file_from_to(prev_file_info.path, out_path_b);

    // Create output file a using the current file's data and a modified 
    // version of its name.
    join_name_to_ext(name_wext_a, file_path.name, file_path.ext, "_a.");
    join_dir_to_name(out_path_a, target_dir, name_wext_a);
    copy_file_from_to(file_path.path, out_path_a);

    // Copy current data to previous data, so the next iteration of the loop 
    // will see the right previous and current data. Free the old previous
    // file's descriptor.
    close(prev_file_info.fd);
    prev_file_info = file_info;
    prev_file_path = file_path;
  }

  // Treat the last pair separately. Form the last pair by wrapping around 
  // (so the first file is the one used for the output file base name and 
  // data). 
  copy_file_from_to(prev_file_path.path, first_out_path_b);
  copy_file_from_to(first_in_path, first_out_path_a);
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
  char exp_path_1_a[SZ_PATH], exp_path_1_b[SZ_PATH], 
    exp_path_2_a[SZ_PATH], exp_path_2_b[SZ_PATH];
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
  if(remove(exp_path_1_a) || remove(exp_path_1_b) || remove(exp_path_2_a)
    || remove(exp_path_2_b) || remove(target_dir_name) || remove(test_path_1)
    || remove(test_path_2) || remove(source_dir_name)){
    fprintf(stderr, "Failed to delete temporary files and directories.");
    exit(EXIT_FAILURE);
  } 
  return EXIT_SUCCESS;
}
#else
int main(int argc, char* argv[]){
  if(argc!=3 || *argv[1] || *argv[2]){
    fprintf(stderr, "Usage: %s <SOURCE_DIR> <TARGET_DIR>", argv[0]);
    exit(EXIT_FAILURE);
  } 
  generate_nonduplicates(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif
