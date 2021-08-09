/* join_dir_to_name.c 
 *
 * Joins a directory name to a name to produce a path. The directory name may
 * optionally end with a '/', but one will be added otherwise. If the name 
 * begins with a '/', there will be two.
 *
 * Compile with:
 *
 * gcc join_dir_to_name.c -Wall -g -o join-dir-to-name \
 * -DTEST_JOIN_DIR_TO_NAME
 * 
 * Usage: 
 *
 * join-dir-to-name <DIR> <NAME>
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

#ifndef PATH_MAX
#  define PATH_MAX 4096 //make large enough to hold any path
#endif

#ifndef PATH_SEP
#  define PATH_SEP '/'
#endif

void join_dir_to_name(char out_buf[PATH_MAX], char* dir, char* name){
  char sep[1]="";
  char* p = strrchr(dir, '\0');         // dir last non-null char
  if(*(p-1)) *sep=PATH_SEP;             // avoid 2x '/'
  snprintf(out_buf, PATH_MAX, "%s%s%s", // print sep as needed
    dir, sep, name);
}

#ifdef TEST_JOIN_DIR_TO_NAME
int main(int argc, char* argv[argc]) {
  if(argc!=3){
    printf("Usage: %s <DIR> <NAME>", argv[0]);
    exit(EXIT_FAILURE);
  }

  char buf[path_max];
  char* expect = "/foo/bar/baz";

  /* Handle trailing '/' in @dir
   */
  char* dir = "/foo/bar/";
  char* name = "baz";
  join_dir_to_name(buf, dir, name);
  assert(!strcmp(buf, expect));

  /* Handle no trailing '/' in @dir
   */
  dir = "/foo/bar";
  join_dir_to_name(buf, dir, name);
  assert(!strcmp(buf, expect));

  /* Does NOT handle leading '/' in @name
   */
  expect = "/foo/bar//baz"
  name = "/baz";
  join_dir_to_name(buf, dir, name);
  assert(!strcmp(buf, expect));

  return EXIT_SUCCESS;
}
#endif
