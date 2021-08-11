/* split.c
 *
 * Defines function split (and tests)
 * 
 * Split a string into a name and extension. The name and extension are copied
 * into the output buffers name[] and ext[]
 *
 * e.g. in path="/foo/bar.baz/" get name="bar" and ext="baz"
 *
 *
 * Compile
 *
 * gcc split.c -DTEST_SPLIT -o test-split -g -Wall
 *
 *
 * Run
 * 
 * ./test-split
 *
 *
 * Debug
 *
 * gdb test-split
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define SZ_NAME 256
#define SZ_PATH 4096

#define PATH_SEP '/'

void split(char name[NAME_MAX], char ext[SZ_NAME],
  char path[SZ_PATH]){
  char* p_null, * p_sep, * p_ext, * p_tr_sep=NULL;
  // Get a pointer to the terminating null character, so we don't keep calling
  // strchr/strrchr
  p_null = strchr(path, '\0'); // assume this is never null
  // Use the path separator to strip the path. 
  p_sep = p_null;
  for(;;){
    if(*p_sep == PATH_SEP) break;
    if(p_sep == path){
      p_sep=NULL;
      break;
    }
    --p_sep;
  }
  //Skip over trailing separator, if there is one.
  if(p_sep && !*(p_sep+1)){
    // Set null byte so strrchr can find the next one. Save the old position.
    *p_sep=0;
    p_tr_sep = p_sep;
    for(;;){
      if(*p_sep == PATH_SEP) break;
      if(p_sep == path){
        p_sep=NULL;
        break;
      }
      --p_sep;
    }
  }
  // Allow for hidden filenames like .foo and .bar.baz
  p_ext = p_null;
  for(;;){
    if(*p_ext == '.') break;
    if(*p_ext == PATH_SEP || p_ext == path){
      p_ext=NULL;
      break;
    }
    --p_ext;
  }
  const int has_nontr_ext = p_ext && p_ext != path && p_ext != p_sep+1;
  // If there's a non-trailing extension dot, copy the extension to the output 
  // buffer. A trailing extension would be like "foo."
  if(has_nontr_ext) {
    strncpy(ext, p_ext+1, SZ_NAME);
    // Set dot separator '.' to the null byte, so strncpy only sees the name
    *p_ext=0; 
  }
  // Copy to name buffer
  strncpy(name , p_sep ? p_sep+1 : path, NAME_MAX);
  // Set dot separator back, if any
  if(has_nontr_ext) *p_ext = '.';
  // Set trailing separator back, if any
  if(p_tr_sep) *p_tr_sep = '/';
}

#ifdef TEST_SPLIT
int main(){
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "foo.bar.baz";
    split(name, ext, path);
    assert(!strcmp(name, "foo.bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "foo.bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "foo.bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo.baz/fiz.boz/";
    split(name, ext, path);
    assert(!strcmp(name, "fiz"));
    assert(!strcmp(ext, "boz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "foo"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo.baz";
    split(name, ext, path);
    assert(!strcmp(name, "foo"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/bar...baz";
    split(name, ext, path);
    assert(!strcmp(name, "bar.."));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/bar..baz";
    split(name, ext, path);
    assert(!strcmp(name, "bar."));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/bar.";
    split(name, ext, path);
    assert(!strcmp(name, "bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/bar.baz";
    split(name, ext, path);
    assert(!strcmp(name, "bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/.bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[SZ_PATH]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/..bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "..bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/...bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "...bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[SZ_PATH]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = ".bar";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = ".bar.baz";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = ".bar.";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/.bar/";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/.bar./";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/.bar../";
    split(name, ext, path);
    assert(!strcmp(name, ".bar."));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/.bar.../";
    split(name, ext, path);
    assert(!strcmp(name, ".bar.."));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[NAME_MAX]={0}, ext[SZ_NAME]={0};
    char path[SZ_PATH] = "/foo/.bar...baz/";
    split(name, ext, path);
    assert(!strcmp(name, ".bar.."));
    assert(!strcmp(ext, "baz"));
  }
  return EXIT_SUCCESS;
}
#endif
