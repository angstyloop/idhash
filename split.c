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

#ifndef EXT_MAX
#  define EXT_MAX 256
#endif

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif

#ifndef PATH_SEP
#  define PATH_SEP '/'
#endif

void split(char name[PATH_MAX], char ext[EXT_MAX],
  char path[PATH_MAX]){
  char* p_sep, * p_ext, * p_tr_sep=NULL;
  // Use the path separator to strip the path. Skip over a trailing separator.
  if((p_sep = strrchr(path, PATH_SEP)) && !*(p_sep+1)){
    // Set null byte so strrchr can find the next one. Save the old position.
    *p_sep=0;
    p_tr_sep = p_sep;
    p_sep = strrchr(path, PATH_SEP);
  }
  // Allow for hidden filenames like .foo and .bar.baz
  const int has_nontr_ext = (p_ext = strrchr(path, '.')) 
    && p_ext!=path // first char in path
    && p_ext!=p_sep+1; // first char after sep
  // If there's an extension, copy it to the output buffer.
  if(has_nontr_ext) {
    strncpy(ext, p_ext+1, EXT_MAX);
    // Set dot separator '.' to the null byte, so strncpy gets the name
    *p_ext=0; 
  }
  // Copy to name buffer
  strncpy(name , p_sep ? p_sep+1 : path, PATH_MAX);
  // Set dot separator back, if any
  if(has_nontr_ext) *p_ext = '.';
  // Set trailing separator back, if any
  if(p_tr_sep) *p_tr_sep = '/';
}

#ifdef TEST_SPLIT
int main(){
//  { 
//    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
//    char path[PATH_MAX] = "foo.bar.baz";
//    split(name, ext, path);
//    assert(!strcmp(name, "foo.bar"));
//    assert(!strcmp(ext, "baz"));
//  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "foo.bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "foo.bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo.baz/fiz.boz/";
    split(name, ext, path);
    assert(!strcmp(name, "fiz"));
    assert(!strcmp(ext, "boz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "foo"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo.baz";
    split(name, ext, path);
    assert(!strcmp(name, "foo"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/bar...baz";
    split(name, ext, path);
    assert(!strcmp(name, "bar.."));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/bar..baz";
    split(name, ext, path);
    assert(!strcmp(name, "bar."));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/bar.";
    split(name, ext, path);
    assert(!strcmp(name, "bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/bar.baz";
    split(name, ext, path);
    assert(!strcmp(name, "bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/.bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/..bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "..bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/...bar.baz/";
    split(name, ext, path);
    assert(!strcmp(name, "...bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = ".bar";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = ".bar.baz";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, "baz"));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = ".bar.";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/.bar/";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/.bar./";
    split(name, ext, path);
    assert(!strcmp(name, ".bar"));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/.bar../";
    split(name, ext, path);
    assert(!strcmp(name, ".bar."));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/.bar.../";
    split(name, ext, path);
    assert(!strcmp(name, ".bar.."));
    assert(!strcmp(ext, ""));
  }
  { 
    char name[PATH_MAX]={0}, ext[EXT_MAX]={0};
    char path[PATH_MAX] = "/foo/.bar...baz/";
    split(name, ext, path);
    assert(!strcmp(name, ".bar.."));
    assert(!strcmp(ext, "baz"));
  }
  return EXIT_SUCCESS;
}
#endif
