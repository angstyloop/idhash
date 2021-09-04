/*
gcc -g -Wall count_jpegs.c -o test-count-jpegs -DTEST_COUNT_JPEGS

Count the jpegs in a directory.
*/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

void count_jpegs(int* n, DIR* pdir){
  // iterate through each regular file in directory
  //   get name of file
  //   count file names ending in ".jpeg", ".jpg", ".JPG", or ".JPEG".
}

int main(){
  //make tmp dir
  //count jpegs in tmp dir
  //assert count is zero
  //make a jpg file in tmp dir for each jpeg pattern (4)
  //count jpegs in tmp dir
  //assert count is 4
  //remove files (!)
  //remove tmpdir (!)
  //
  //(!) not required
}
