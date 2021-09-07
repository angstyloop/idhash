/*
gcc path.c -o test-path -DTEST_PATH -g -Wall
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
  char* full;
};

static char* path_full(path* ppath);

path* path_create(){
  path* ppath = calloc(1, sizeof(path));
  ppath->dir = calloc(1, 1);
  ppath->name = calloc(1, 1);
  ppath->ext = calloc(1, 1);
  ppath->full = calloc(1, 1);
  return ppath;
}

// Return true iff dir is neither 0 nor "".
int path_has_dir(path* ppath){
  return *ppath->dir;
}

// Return true iff name is neither 0 nor "".
int path_has_name(path* ppath){
  return *ppath->name;
}

// Return true iff ext is neither 0 nor "".
int path_has_ext(path* ppath){
  return *ppath->ext;
}

// Return true iff the path object is empty.
int path_empty(path* ppath){
  return !( path_has_dir(ppath) 
    || path_has_name(ppath) 
    || path_has_ext(ppath) );
}

path* path_init(
  path* ppath, 
  char* dir,
  char* name,
  char* ext)
{
  size_t n=0; 

  if(*name){
    n = strlen(name);
    free(ppath->name);
    ppath->name = calloc(n+1, 1);
    strncpy(ppath->name, name, n);

    if(!strcmp("..", ppath->name)){
      free(ppath->full);
      ppath->full = calloc(2, 1);
      ppath->full[0] = ppath->full[1] = '.';
      return ppath;
    }

    if(!strcmp(".", ppath->name)){
      free(ppath->full);
      ppath->full = calloc(1, 1);
      ppath->full[0] = '.';
      return ppath;
    }
  }

  if(*dir){
    n = strlen(dir);
    free(ppath->dir);
    ppath->dir = calloc(n+1, 1);
    strncpy(ppath->dir, dir, n);
  }

  if(*ext){
    n = strlen(ext);
    free(ppath->ext);
    ppath->ext = calloc(n+1, 1);
    strncpy(ppath->ext, ext, n);
  }

  free(ppath->full);
  ppath->full = path_full(ppath);
  return ppath;
}

path* path_new(
  char* dir,
  char* name,
  char* ext)
{
  return path_init(path_create(), dir, name, ext);
}

path* path_free(path* ppath){
  free(ppath->dir);
  free(ppath->name);
  free(ppath->ext);
  free(ppath->full);
  return ppath;
}

path* path_print(path* ppath){
  printf("dir: %s%*cname: %s%*cext: %s%*c\n",
    ppath->dir, 4, ' ', 
    ppath->name, 4, ' ', 
    ppath->ext, 4, ' ');
  return ppath;
}

unsigned path_ext_len(path* ppath){
  return path_has_ext(ppath) ? strlen(ppath->ext) : 0;
}

static char* path_full(path* ppath){
  int wext = path_has_ext(ppath);
  int wdir = path_has_dir(ppath);

  size_t n = strlen(ppath->dir) + (wdir ? 1/*slash*/ : 0) 
    + strlen(ppath->name)
    + (wext ? 1/*dot*/ : 0)  + strlen(ppath->ext)
    + 1/*nullbyte*/;

  char* fullpath = calloc(n, 1);

  snprintf(fullpath, n, "%s%s%s%s%s", 
    ppath->dir,
    wdir ? "/" : "", 
    ppath->name, 
    wext ? "." : "",  
    ppath->ext);

  return fullpath;
}

path* path_set_ext(path* ppath, char* ext){
  free(ppath->ext);
  size_t n = strlen(ext) + 1/*nullbyte*/;
  ppath->ext = calloc(n, 1);
  strncpy(ppath->ext, ext, n-1);
  return ppath;
}

#ifdef TEST_PATH
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
  if(0>(mkstemps(p_full, strlen(ppath->ext) + 1/*dot*/))){
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
  if(0>mkstemps(p_full, strlen(ppath->ext) + 1)){
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
#endif
