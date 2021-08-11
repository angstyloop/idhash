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

#ifndef GUARD_ASSERT
#  define GUARD_ASSERT
#  include <assert.h>
#endif

#ifndef GUARD_STRING
#  define GUARD_STRING
#  include <string.h>
#endif

#ifndef SZ_PATH
#  define SZ_PATH 4096 //make large enough to hold any path
#endif

#ifndef PATH_SEP
#  define PATH_SEP '/'
#endif

void join_dir_to_name(char out_buf[SZ_PATH], char dir[static 1], char* name){
  char sep[1] = "";
  char* p = strchr(dir, '\0');
  if(*dir && *(p-1) != PATH_SEP) *sep = PATH_SEP;
  snprintf(out_buf, SZ_PATH, "%s%s%s", dir, sep, name);
}

#ifdef TEST_JOIN_DIR_TO_NAME
int main() {
  char buf[SZ_PATH];

  /* Handle trailing '/' in @dir
   */
  {
    char* dir = "/foo/bar/";
    char* name = "baz";
    char* expect = "/foo/bar/baz";
    join_dir_to_name(buf, dir, name);
    assert(!strcmp(buf, expect));
  }

  /* Handle no trailing '/' in @dir
   */
  {
    char* dir = "/foo/bar";
    char* name = "baz";
    char* expect = "/foo/bar/baz";
    join_dir_to_name(buf, dir, name);
    assert(!strcmp(buf, expect));
  }

  /* Does NOT handle leading '/' in @name
   */
  {
    char* dir = "/foo/bar/";
    char* name = "/baz";
    char* expect = "/foo/bar//baz";
    join_dir_to_name(buf, dir, name);
    assert(!strcmp(buf, expect));
  }

  return EXIT_SUCCESS;
}
#endif
