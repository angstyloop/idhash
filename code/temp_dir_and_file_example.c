/*
gcc temp_dir_and_file_example.c -o temp-dir-and-file-example -g -Wall
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
  // create temporary directory
  char tdname[] = "/tmp/tmpdir.XXXXXX"; 
  if(!mkdtemp(tdname)){
    fprintf(stderr, "Unable to open temporary directory %s.\n", tdname);
    exit(EXIT_FAILURE);
  } 
  // print temporary directory name
  puts(tdname);
  // form temporary file name (full path to file, as opposed to just the 
  // basename)
  char tfbase[] = "tmpfile.XXXXXX.jpg"; // note strlen(".jpg") == 4 
  int len = strlen(tdname) + 1/*slash*/ + strlen(tfbase) + 1/*null*/;
  char tfname[len];
  snprintf(tfname, len, "%s/%s", tdname, tfbase);
  // create temporary file in temporary directory
  int tfd=-1;
  if(0>(tfd = mkstemps(tfname, 4))){
    fprintf(stderr, "Unable to open temporary file %s.\n", tfname);
    exit(EXIT_FAILURE);
  } 
  // print temporary file name
  puts(tfname);
  // clean up
  close(tfd);
  if(remove(tfname)){
    fprintf(stderr, "Unable to remove temporary file %s.\n", tfname);
    exit(EXIT_FAILURE);
  } 
  if(remove(tdname)){
    fprintf(stderr, "Unable to remove temporary directory %s.\n", tdname);
    exit(EXIT_FAILURE);
  } 
  return EXIT_SUCCESS;
}
