/* copy.c
 *
 * Defines function copy:
 *
 * Copy source file to destination file on the same filesystem (possibly NFS).
 * If the destination file does not exist, it is created. If the destination
 * file does exist, the old data is truncated to zero and replaced by the 
 * source data. The copy takes place in the kernel space.
 *
 * Compile with:
 *
 * gcc -DTEST_IT_OUT -Wall -g -o test_it_out copy.c
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
 * version. The strides are chosen to allow enough space for two-digit 
 * minor version and patch level.
 *
 */
#define GCC_VERSION (__GNUC__*10000 + __GNUC_MINOR__*100 + __gnuc_patchlevel__)
#if GCC_VERSION < 22700
static loff_t copy_file_range(int in, loff_t* off_in, int out, 
  loff_t* off_out, size_t s, unsigned int flags)
{
  return syscall(__NR_copy_file_range, in, off_in, out, off_out, s,
    flags);
}
#endif

/* The copy function.
 */
int copy(const char* src, const char* dst){
  int in, out;
  struct stat stat;
  loff_t s, n;
  if(0>(in = open(src, O_RDONLY))){
    perror("open(src, ...)");
    exit(EXIT_FAILURE);
  }
  if(fstat(in, &stat)){
    perror("fstat(in, ...)");
    exit(EXIT_FAILURE);
  }
  s = stat.st_size; 
  if(0>(out = open(dst, O_CREAT|O_WRONLY|O_TRUNC, 0644))){
    perror("open(dst, ...)");
    exit(EXIT_FAILURE);
  }
  do{
    if(1>(n = copy_file_range(in, NULL, out, NULL, s, 0))){
      perror("copy_file_range(...)");
      exit(EXIT_FAILURE);
    }
    s-=n;
  }while(0<s && 0<n);
  close(in);
  close(out);
  return EXIT_SUCCESS;
}

/* Test it out.
 *
 * BASH:
 *
 * gcc -DTEST_IT_OUT -Wall -g -o test_it_out copy.c
 * echo 'Hello, world!' > src_file.txt
 * ./test_it_out src_file.txt dst_file.txt
 * diff src_file.txt
 * diff dst_file.txt
 *
 */
#ifdef TEST_IT_OUT
int main(int argc, char* argv[argc]){
  if(argc!=3){
    printf("Usage: %s <SOURCE> <DESTINATION>", argv[0]);
    exit(EXIT_FAILURE);
  }
  copy(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif
