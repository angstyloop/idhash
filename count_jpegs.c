/******************************************************************************* 
gcc -g -Wall count_jpegs.c -o test-count-jpegs -DTEST_COUNT_JPEGS

Count the jpegs in a directory.
*******************************************************************************/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef TYPES_H
#  define TYPES_H
#  include <sys/types.h>
#endif

#ifndef DIRENT_H
#  define DIRENT_H
#  include <dirent.h>
#endif

#ifndef STAT_H
#  define STAT_H
#  include <sys/stat.h>
#endif

#ifndef UNISTD_H
#  define UNISTD_H
#  include <unistd.h>
#endif

#ifndef FCNTL_H
#  define FCNTL_H
#  include <fcntl.h>
#endif

#ifndef ASSERT_H
#  define ASSERT_H
#  include <assert.h>
#endif

#ifndef PATH_H
#  define PATH_H
#  include "path.c"
#endif

// return 1 if @substr matches the end of @str (and is no bigger than @str),
// else return 0.
int ends_with(char* s, char* ss){
  size_t ns = strlen(s), nss = strlen(ss);
  return nss > ns ? 0 : !strncmp(s + ns - nss, ss, nss);
}

// 0-terminated list of accepted jpeg extensions
static char* jpeg_exts[] = {"jpg", "JPG", "jpeg", "JPEG", 0};

// return a newly allocated string just big enough for "${s1}${s2}".
// the returned string must be freed.
char* concat(char* s1, char* s2){
  size_t n = strlen(s1) + strlen(s2) + 1/*nullbyte*/;
  char* s = calloc(n, 1);
  snprintf(s, n, "%s%s", s1, s2);
  return s;
}

// return 1 if @path has an extension from @jpeg_extensions
int is_jpeg(char* path){
  for(char** pext=jpeg_exts; *pext; ++pext){
    char* dot_ext = concat(".", *pext);
    if(ends_with(path, dot_ext)) return 1;
    free(dot_ext);
  }
  return 0;
}

// count file names with extension from @jpeg_exts
void count_jpegs(unsigned* count, char* dname){
  // iterate through each regular file in directory, counting the paths with 
  // a jpeg extension from @jpeg_exts
  DIR* pdir = opendir(dname);
  int fd=-1;
  struct dirent* pdirent=0;
  struct stat st={0};
  while((pdirent = readdir(pdir))){
    path* ppath = path_new(dname, pdirent->d_name, "");
    char* fullpath = path_full(ppath);

    // open and stat file
    if(-1 == (fd = open(fullpath, O_RDONLY)) || fstat(fd, &st)){
      fprintf(stderr, "Failed to open/stat file %s.\n", fullpath);
      exit(EXIT_FAILURE);
    }

    // is it a regular file? does it have a jpeg extension?
    if(S_ISREG(st.st_mode) && is_jpeg(fullpath))
      ++(*count);

    //clean up
    free(fullpath);
    path_free(ppath);
    close(fd);
  }
  closedir(pdir);
}

#ifdef TEST_COUNT_JPEGS
int main(){
  char dname[] =  "/tmp/test_count_jpegs_XXXXXX";
  //make tempdir
  if(!(mkdtemp(dname))){
    fprintf(stderr, "Unable to make tempdir %s.\n", dname);
    exit(EXIT_FAILURE);
  } 

  //count jpegs in tmp dir
  unsigned count=0;
  count_jpegs(&count, dname);

  //assert count is zero
  assert(!count);

  //loop over extensions. make a tempfile for each (4 total).
  for(char** pext = jpeg_exts; *pext; ++pext){
    //make tempfile path object
    path* ppath = path_new(dname, "test_file_XXXXXX", *pext);
    //create full path string
    char* pfull = path_full(ppath);
    //make tempfile
    if(0>mkstemps(pfull, path_has_ext(ppath) ? 
      1/*dot*/ + strlen(ppath->ext) : 0))
    {
      fprintf(stderr, "Unable to make tempfile %s.\n", pfull);
      free(pfull);
      path_free(ppath);
      exit(EXIT_FAILURE);
    } 
    free(pfull);
    path_free(ppath);
  }

  //count jpegs in tmp dir
  count=0;
  count_jpegs(&count, dname);

  //assert count is 4
  assert(count == 4);

  //remove all regular files in dname
  int fd=0;
  struct dirent* pdirent=0;
  struct stat st={0};
  DIR* pdir = opendir(dname);
  while((pdirent = readdir(pdir))){
    path* ppath = path_new(dname, pdirent->d_name, "");
    char* fullpath = path_full(ppath);
    if(-1 == (fd = open(fullpath, O_RDONLY)) || fstat(fd, &st)){
      fprintf(stderr, "Unable to open/stat tempfile %s.\n", fullpath);
      exit(EXIT_FAILURE);
    } 
    if(S_ISREG(st.st_mode) && remove(fullpath)){
      fprintf(stderr, "Error removing tempfile %s.\n", fullpath);
      free(fullpath);
      free(ppath);
      exit(EXIT_FAILURE);
    } 
    free(fullpath);
    free(ppath);
  }
  closedir(pdir);

  //remove tmpdir
  if(remove(dname)){
    fprintf(stderr, "Error removing tempdir %s.\n", dname);
    exit(EXIT_FAILURE);
  } 

  puts("OK");
  return EXIT_SUCCESS;
}
#endif
