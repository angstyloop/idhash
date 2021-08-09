/* join_paths.c 
 *
 * Joins two paths to produce a new one. The first path may optionally end with 
 * a '/', and the second path may optionally start with a '/'. If both are the
 * case, only one of the '/' is included in the new path. If neither is the 
 * case, a separating '/' is still included in the new path.
 * 
 * A preprocessor constant PATH_SEP is used instead of '/' directly.
 *

 * Compile with:
 *
 * gcc join_dir_to_name.c -Wall -g -o join-dir-to-name -DTEST_JOIN_DIR_TO_NAME
 * 

 * Usage: 
 *
 * join-dir-to-name <DIR> <NAME>
 *
 */

#ifndef GUARD_STDLIB
#define GUARD_STDLIB
#include <stdlib.h>
#endif

#ifndef GUARD_STDIO
#define GUARD_STDIO
#include <stdio.h>
#endif

#ifndef PATH_MAX
#  define PATH_MAX
#  define PATH_MAX 4096 //make large enough to hold any path
#endif

#ifndef PATH_SEP
#  define PATH_SEP
#  define PATH_SEP '/'
#endif

void join_paths(char out_buf[PATH_MAX], char* path_1, char* path_2){
  char sep[1]=""; 
  char* p = strrchr(path_1, '\0') - 1;   //path_1 last non-null char
  char* q = path_2;                      //path_2 first char
  if(PATH_SEP == *p)
    if(PATH_SEP == *q) ++q;              //avoid 2x '/'
  else
    if(PATH_SEP != *q) *sep=PATH_SEP;    //avoid no '/'
  
  snprintf(out_buf, PATH_MAX, "%s%s%s",  //print sep as needed
    path_1, sep, path_2); 
}

#ifdef TEST_JOIN_PATHS
int main(int argc, char* argv[argc]) {
  if(argc!=3){
    printf("Usage: %s <DIR> <NAME>", argv[0]);
    exit(EXIT_FAILURE);
  }

  char buf[path_max];
  char* expect;
  char* path_1;
  char* path_2;

  /* Handle trailing '/' in path_1, and no leading '/' in path_2.
   */
  expect = "/foo/bar/baz";
  path_1 = "/foo/bar/";
  path_2 = "baz";
  join_paths(buf, path_1, path_2);
  assert(!strcmp(buf, expect));

  /* Handle no trailing '/' in path_1, and no leading '/' in path_2.
   */
  expect = "/foo/bar/baz";
  path_1 = "/foo/bar";
  path_2 = "baz";
  join_paths(buf, path_1, path_2);
  assert(!strcmp(buf, expect));

  /* Handle no trailing '/' in path_1, and leading '/' in path_2.
   */
  expect = "/foo/bar/baz";
  path_1 = "/foo/bar";
  path_2 = "/baz";
  join_paths(buf, path_1, path_2);
  assert(!strcmp(buf, expect));

  /* Handle trailing '/' in path_1, and leading '/' in path_2.
   */
  expect = "/foo/bar/baz";
  path_1 = "/foo/bar/";
  path_2 = "/baz";
  join_paths(buf, path_1, path_2);
  assert(!strcmp(buf, expect));

  return EXIT_SUCCESS;
}
#endif
