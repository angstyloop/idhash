/* copy.c
 *
 * Compile with:
 * 
 * gcc -DTEST_IT_OUT -Wall -g -o test_it_out copy.c
 *
 * Defines function copy:
 *
 * Copy a source file to a destination file. If the destination file already
 * exists, this clobbers it. If the destination file does not exist, it is
 * created. 
 *
 * Uses a buffer in user-space, so may not perform as well as 
 * copy_file_range, which copies in kernel-space.
 *
 */

#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE 65536 //2^16

int copy(const char* in_path, const char* out_path){
  size_t n;
  FILE* in, * out;
  char* buf = calloc(BUF_SIZE, 1);
  if(!(in = fopen(in_path, "rb"))){
    printf("hai1");
    return EXIT_FAILURE;
  }
  if(!(out = fopen(out_path, "wb"))) {
    printf("hai2");
    return EXIT_FAILURE;
  }
  for(;;){
    if(!(n = fread(buf, 1, BUF_SIZE, in)))
      break;
    if(!(fwrite(buf, 1, n, out)))
      break;
  }
  free(buf);
  fclose(in);
  fclose(out);
  return EXIT_SUCCESS;
}

/* Test it out.
 *
 * BASH:
 *
 * gcc -DTEST_IT_OUT -Wall -g -o test_it_out copy.c
 * echo 'Hello, world!' > src_file.txt
 * ./test_it_out src_file.txt dst_file.txt
 * diff src_file.txt dst_file.txt
 *
 */
#ifdef TEST_IT_OUT
int main(int argc, char* argv[argc]){
  if(argc!=3){
    printf("Usage: %s <SOURCE> <DESTINATION>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  copy(argv[1], argv[2]);
  return EXIT_SUCCESS;
}
#endif
