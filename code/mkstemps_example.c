/*
gcc mkstemps_example.c -o mkstemps-example -g -Wall
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
  int fd_tmp=-1;
  char template[] = "/tmp/foo.XXXXXX.jpg";
  if(0>(fd_tmp = mkstemps(template, 4))){
    fprintf(stderr, "Unable to create temporary file from template %s.\n", 
      template);
    exit(EXIT_FAILURE);
  }
  puts(template);
  close(fd_tmp);
  return EXIT_SUCCESS;
}
