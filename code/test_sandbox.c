/* test_sandbox.c
 *
 * Q: Are '..' and '.' regular files, according to GNU macro S_ISREG?
 *
 * A: No. Neither '.' nor '..' are regular files. This is demonstrated in
 * the test main.
 */

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef UNISTD_H
#  define UNISTD_H
#  include <unistd.h>
#endif

#ifndef ASSERT_H
#  define ASSERT_H
#  include <assert.h>
#endif

#ifndef STAT_H
#  define STAT_H
#  include <sys/stat.h>
#endif

#ifndef TYPES_H
#  define TYPES_H
#  include <sys/types.h>
#endif

#ifndef FCNTL_H
#  define FCNTL_H
#  include <fcntl.h>
#endif

int main(){
  {
    int fd=0;
    struct stat stat;
    if(0>(fd = open(".", O_RDONLY)))
      assert(0);
    if(fstat(fd, &stat))
      assert(0);
    assert(!S_ISREG(stat.st_mode));
  }
  {
    int fd=0;
    struct stat stat;
    if(0>(fd = open("..", O_RDONLY)))
      assert(0);
    if(fstat(fd, &stat))
      assert(0);
    assert(!S_ISREG(stat.st_mode));
  }
  return EXIT_SUCCESS;
}
