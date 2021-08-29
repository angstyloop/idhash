/* 
gcc -g -Wall idhash_process.c -o test-idhash-process -DTEST_IDHASH_PROCESS `pkg-config vips --cflags --libs`
*/

#ifndef ASSERT_H
#  define ASSERT_H
#  include <assert.h>
#endif

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef IDHASH_H
#  define IDHASH_H
#  include "idhash.h"
#endif

#ifndef TYPES_H
#  define TYPES_H
#  include <sys/types.h>
#endif

#ifndef WAIT_H
#  define WAIT_H
#  include <wait.h>
#endif

#define SZ_BUF 256
#define SZ_BUF_MAX 66535

#define IDHASH_DIST_EXEC "/home/falkor/source/idhash/idhash-distance"

static void error(char* s);

// @poutbuf: a pointer to the output buffer
// @pn: a pointer to the size of the output buffer
// @fd: the file descriptor of the input file
//
// Modifies *poutbuf and *pn.
//
// Read from file descriptor @fd into the output buffer at poutbuf,
// doubling the size of the output buffer at @poutbuf with realloc and
// modifying @pn as needed, until EOF is reached or an error occurs.
//
// Exit if realloc triggers an out-of-memory error, or if there are 
// more input chars than SIZE_MAX, or if there is a read error.
//
void readto(char** poutbuf, size_t* pn, int fd) {
  char readbuf[SZ_BUF]={0};
  if (!*pn || *pn < SZ_BUF) *pn = SZ_BUF;
  if (!*poutbuf) *poutbuf = calloc(*pn, 1);
  size_t ntotal=0;
  for(size_t z=0, d=0; z=read(fd, readbuf, SZ_BUF); d+=z){
    if(-1==z) error("read");
    // if the size at @pn isn't large enough for the input so far and an
    // end '\0', try to realloc.
    if(*pn <= ntotal += z){
      // per C99, don't create an object that exceeds SIZE_MAX.
      if(SIZE_MAX/2 >= ntotal) error("input too large");
      // make a copy of the old pointer, in case realloc triggers ENOMEM
      char* t = *poutbuf;
      // double the size of the output buffer
      if(!(t = realloc(*poutbuf, *pn = 2 * ntotal))){
        //if there was more work to be done, this would need to be freed
        //free(*poutbuf);
        // but here ENOMEM results in an exit (as it probably always should?)
        error("realloc");
      }
      *poutbuf = t;
    }
    memcpy(*poutbuf + d, readbuf, z);
  }
  (*poutbuf)[ntotal]=0;
}

void idhash_process(
  char** poutbuf,
  size_t* pn,
  char path_a[static 1],
  char path_b[static 1])
{
  // create the child output pipe. since this is before the fork, both parent 
  // and child processes will have their own copy of the child output pipe.
  int out[2]={0};
  if(0>pipe(out)) 
    error("pipe out");
  // create child process
  int pid=0;
  if(!(pid = fork())){ 
    //child process

    // close files referenced by the stdout and stderr file descriptors.
    close(1); 
    close(2);

    // make the stdout and stderr file descriptors both reference the write end 
    // of the child output pipe
    dup2(out[1], 1); 
    dup2(out[1], 2);

    // close the read end of the child output pipe. the child's copy of the pipe
    // doesn't need this (hence the name "child output pipe").
    close(out[0]);

    // replace current process image with idhash-dist path_a path_b 
    execl(IDHASH_DIST_EXEC, IDHASH_DIST_EXEC, path_a, path_b, NULL);

    error("execl failed");
  } else {
    // parent process

    // close the write end of the parent's copy of the child output pipe
    close(out[1]);

    // collect child process data output into the output buffer at @preadbuf
    readto(preadbuf, pn, fd);

    // no more reading. close the read end of the child output pipe. all ends 
    // of the parent's copy of the child output pipe are closed now.
    close(out[0]); 

    // use the SIGCHLD sent by the child process upon its termination to check 
    // the termination status of the child, removing it from the process 
    // entry table. This prevents the child from becoming a zombie - an 
    // entry in the process table for a terminated but not cleaned up process. 
    // if the signal hasn't been sent yet, blocks until the signal is received.
    waitpid(pid, 0, 0);

    //... do more stuff if you want ...

    // parent done.
  } 
}

#ifdef TEST_IDHASH_PROCESS
int main(int argc, char* argv[argc]){
  if(!(3==argc && *argv[1] && *argv[2])){
    fprintf(stderr, "Usage: %s <IMAGE_A> <IMAGE_B>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  char* outbuf=0;
  size_t n=0;

  idhash_process(&outbuf, &n, argv[1], argv[2]);
  printf("%s", outbuf);
  if(outbuf) free(outbuf);

  idhash_process(&outbuf, &n, argv[1], argv[2]);
  printf("%s", outbuf);
  if(outbuf) free(outbuf);

  idhash_process(&outbuf, &n, argv[1], argv[2]);
  printf("%s", outbuf);
  if(outbuf) free(outbuf);

  return EXIT_SUCCESS;
}
#endif

static void error(char* s){
  perror(s);
  exit(EXIT_FAILURE);
}
