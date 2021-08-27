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

#ifndef SZ_PATH
#define SZ_PATH 4096
#endif

#define SZ_BUF 4096

static void error(char* s);

#ifdef TEST_IDHASH_PROCESS
int main(int argc, char* argv[argc]){
  if(!(3==argc && *argv[1] && *argv[2])){
    fprintf(stderr, "Usage: %s <IMAGE_A> <IMAGE_B>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  // create the child output pipe - a pipe for child output <0.->
  // since this is before the fork, both parent and child processes will have their 
  // own copy of the child output pipe.
  int out[2]={0};
  if(0>pipe(out)) 
    error("pipe out");
  // create child process
  int pid=-1;
  if(!(pid = fork())){ 
    //child process

    // close files referenced by the stdout and stderr file descriptors.
    close(1); 
    close(2);

    // make the stdout and stderr file descriptors both reference the write end of 
    // the child output pipe
    dup2(out[1], 1); 
    dup2(out[1], 2);

    // close the read end of the child output pipe. the child's copy of the pipe
    // doesn't need this.
    close(out[0]);

    // compute the idhash distance between the two images at the given paths.
    idhash_result res_a={0}, res_b={0};
    idhash_filepath(argv[1], &res_a);
    idhash_filepath(argv[2], &res_b);

    // print the distance to the write end of the child output pipe
    printf("%u", idhash_dist(res_a, res_b));

    // no more writing. close the write end of the child output pipe. all ends of
    // the child's copy of the child output pipe are closed now.
    close(out[1]);

    // child done.
    return EXIT_SUCCESS;
  }

  //parent process

  // close the write end of the parent's copy of the child output pipe
  close(out[1]);

  // read from child output into @buf, and write from buf to parent output, until 0 bytes
  // are read, or a read/write error occurs. note read will block until at least 1 byte
  // is written by the child, or there is an error, or the end of the stream is reached.
  char buf[SZ_BUF]={0};
  for(int n=0; 0<(n=read(out[0], buf, SZ_BUF)) && n==write(1, buf, n) ;);

  // no more reading. close the read end of the child output pipe. all ends of the 
  // parent's copy of the child output pipe are closed now.
  close(out[0]); 

  // use the SIGCHLD sent by the child process upon its termination to check the termination
  // status of the child, __removing it from the process entry table__. This prevents the
  // child from becoming a zombie - an entry in the process table for a terminated but not
  // cleaned up process. if the signal hasn't been sent yet, blocks until the signal is 
  // received.
  waitpid(pid, 0, 0);

  //... do more stuff ...

  // parent done.
  return EXIT_SUCCESS;
}
#endif

static void error(char* s){
  perror(s);
  exit(EXIT_FAILURE);
}
