#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char* argv[argc]) {
  if(argc!=2){
    printf("Usage: %s <FILENAME>", argv[0]);
    exit(EXIT_FAILURE);
  }
  DIR* d = opendir(argv[1]);
  struct dirent* e;
  if(d!=NULL){
    while(e = readdir(d)){
      puts(e->d_name);
    }
  }else{
    perror("Couldn't open the directory.");
    exit(EXIT_FAILURE);
  }
  closedir(d);
  return EXIT_SUCCESS;
}
