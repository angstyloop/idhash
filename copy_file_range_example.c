/* copy_file_range_example.c
 *
 * > gcc -o copy copy_file_range_example.c
 * > 
 */

#define _GNU_SOURCE 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

/* On versions of glibc < 2.27, need to use syscall.
 * 
 * To determine glibc version used by gcc, compute an integer representing the
 * version. The "strides" are chosen to allow enough space for two-digit 
 * minor version and patch level.
 *
 */
#define GCC_VERSION (__GNUC__*10000 + __GNUC_MINOR__*100 + __gnuc_patchlevel__)
#if GCC_VERSION < 22700
static loff_t copy_file_range(int fd_in, loff_t* off_in, int fd_out, 
  loff_t* off_out, size_t s, unsigned int flags)
{
  return syscall(__NR_copy_file_range, fd_in, off_in, fd_out, off_out, s,
    flags);
}
#endif

int main(int argc, char* argv[argc]){
  int fd_in, fd_out;
  struct stat stat;
  loff_t s, n;
  if(argc!=3){
    fprintf(stderr, "Usage: %s <SOURCE> <DESTINATION>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  if((fd_in = open(argv[1], O_RDONLY)) < 0){
    perror("open(argv[1])");
    exit(EXIT_FAILURE);
  }
  if(fstat(fd_in, &stat)){
    perror("fstat");
    exit(EXIT_FAILURE);
  }
  s = stat.st_size;
  if((fd_out = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, 0644)) < 0){
    perror("open(argv[2])");
    exit(EXIT_FAILURE);
  }
  do {
    if((n = copy_file_range(fd_in, NULL, fd_out, NULL, s, 0)) < 0){
      perror("copy_file_range()");
      exit(EXIT_FAILURE);
    }
    s-=n;
  } while (s > 0 && n > 0);
  close(fd_in);
  close(fd_out);
  return EXIT_SUCCESS;
}
