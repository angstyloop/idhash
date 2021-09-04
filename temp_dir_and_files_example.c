/*
gcc temp_dir_and_files_example.c -o temp-dir-and-files-example -g -Wall
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct path path;
struct path {
  char* dir;
  char* name;
  char* ext;
};

path* path_create(){
  return calloc(1, sizeof(path));
}
path* path_init(
  path* ppath, 
  char dir[static 1],
  char name[static 1],
  char ext[static 1])
{
  size_t n=0; 

  n = strlen(dir);
  ppath->dir = calloc(n+1, 1);
  strncpy(ppath->dir, dir, n);

  n = strlen(name);
  ppath->name = calloc(n+1, 1);
  strncpy(ppath->name, name, n);

  n = strlen(ext);
  ppath->ext = calloc(n+1, 1);
  strncpy(ppath->ext, ext, n);

  return ppath;
}
path* path_new(
  char dir[static 1],
  char name[static 1],
  char ext[static 1])
{
  return path_init(path_create(), dir, name, ext);
}
path* path_free(path* ppath){
  if(ppath->dir) free(ppath->dir);
  if(ppath->name) free(ppath->name);
  if(ppath->ext) free(ppath->ext);
  free(ppath);
  return ppath;
}
path* path_print(path* ppath){
  printf("dir: %s%*cname: %s%*cext: %s%*c\n",
    ppath->dir, 4, ' ', 
    ppath->name, 4, ' ', 
    ppath->ext, 4, ' ');
  return ppath;
}
char* path_full(path* ppath){
  size_t n = strlen(ppath->dir) + 1/*slash*/ + strlen(ppath->name) + 1/*dot*/ 
    + strlen(ppath->ext) + 1/*nullbyte*/;
  char* fullpath = calloc(n, 1);
  snprintf(fullpath, n, "%s/%s.%s", ppath->dir, ppath->name, ppath->ext);
  return fullpath;
}
char* path_full_no_ext(path* ppath){
  size_t n = strlen(ppath->dir + 1/*slash*/ + strlen(ppath->name) +
    + 1/*nullbyte*/);
  char* p = calloc(n, 1);
  snprintf(p, n, "%s/%s", ppath->dir, ppath->name);
  return p;
}
char* path_full_no_ext_w_dot(path* ppath){
  size_t n = strlen(ppath->dir + 1/*slash*/ + strlen(ppath->name) + 1/*dot*/ 
    + 1/*nullbyte*/);
  char* p = calloc(n, 1);
  snprintf(p, n, "%s/%s.", ppath->dir, ppath->name);
  return p;
}
char* path_ext_w_dot(path* ppath){
  size_t n = 1/*dot*/ + strlen(ppath->ext);
  char* p = calloc(n, 1);
  snprintf(p, n, ".%s", ppath->ext);
  return p;
}
path* path_set_ext(path* ppath, char ext[static 1]){
  if(ppath->ext) free(ppath->ext);
  size_t n = strlen(ext) + 1/*nullbyte*/;
  ppath->ext = calloc(n, 1);
  strncpy(ppath->ext, ext, n-1);
  return ppath;
}

int main(){
  //use "jpg" as the first extension
  path* ppath = path_new("/tmp/tempdir.XXXXXX", "tempfile.XXXXXX", "jpg");

  //create temporary directory
  if(!mkdtemp(ppath->dir)){
    fprintf(stderr, "Unable to open temporary directory from template %s.\n",
      ppath->dir);
    exit(EXIT_FAILURE);
  } 

  //print temporary directory name
  puts(ppath->dir);

  //create first full path string
  char* p_full = path_full(ppath);

  //create first temporary file
  if(0>(mkstemps(p_full, strlen(ppath->ext) + 1))){
    fprintf(stderr, "Unable to create temporary file %s\n", p_full);
    path_free(ppath);
    free(p_full);
    exit(EXIT_FAILURE);
  } 
  //print first temporary file full path string
  puts(p_full);

  //remove second temporary file
  if(remove(p_full)){
    fprintf(stderr, "Unable to remove temporary file %s.\n", p_full);
    exit(EXIT_FAILURE);
  } 

  //clean up first temporary file full path string
  free(p_full);

  // set new extension for second temporary file path to "jpeg" (instead of 
  // "jpg")
  path_set_ext(ppath, "jpeg");

  // create second full path string
  p_full = path_full(ppath);

  //create second temporary file
  if(0>(mkstemps(p_full, strlen(ppath->ext) + 1))){
    fprintf(stderr, "Unable to create temporary file %s\n", p_full);
    free(p_full);
    path_free(ppath);
    exit(EXIT_FAILURE);
  } 

  //print second temporary file path string
  puts(p_full);

  //remove second temporary file
  if(remove(p_full)){
    fprintf(stderr, "Unable to remove temporary file %s.\n", p_full);
    exit(EXIT_FAILURE);
  }  

  //clean up second temporary file path string
  free(p_full);

  //remove temporary directory
  if(remove(ppath->dir)){
    fprintf(stderr, "Unable to remove temporary directory %s.\n", ppath->dir);
    exit(EXIT_FAILURE);
  } 

  //clean up path object
  path_free(ppath);

  //all done!
  return EXIT_SUCCESS;
}
