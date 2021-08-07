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
  FILE* f;
  struct dirent* e;
  if(d!=NULL){
    while(e = readdir(d)){
      if(!strstr("..", e->d_name)){
        continue;
      }
      f = fopen(e->d_name);
      if (!f){
        perror("Couldn't open file.");
        exit(EXIT_FAILURE);
      }
      fclose(f);
      //puts(e->d_name);
    }
  }else{
    perror("Couldn't open the directory.");
    exit(EXIT_FAILURE);
  }
  closedir(d);
  return EXIT_SUCCESS;
}
