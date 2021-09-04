/* 
gcc mkdtemp_example.c -o mkdtemp-example -g -Wall
*/

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(){
  char tdname[] = "/tmp/tmpdir.XXXXXX";
  if(!mkdtemp(tdname)){
    perror("mkdtemp");
    exit(EXIT_FAILURE);
  }
  puts(tdname);
  if(remove(tdname)){
    perror("remove");
    exit(EXIT_FAILURE);
  }
  return(EXIT_SUCCESS);
}
