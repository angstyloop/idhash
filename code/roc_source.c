/*
gcc -g -Wall roc_source.c -o test-roc-source -DTEST_ROC_SOURCE `pkg-config vips --libs --cflags` -lm

The ROC curve is a graphical representation of the predictive power of a 
classifier. It shows how much of an improvement the classifier is over a
random one (a coin flip). It was first used in the 1940s to quantify the 
performance of radar operators (who had to classify pings as friend or 
enemy).
*/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef IDHASH_STATS_H
#  define IDHASH_STATS_H
#  include "idhash_stats.c"
#endif

#ifndef DEFAULT_DUP_FNAME
#  define DEFAULT_DUP_FNAME "duplicates.dat"
#endif

#ifndef DEFAULT_NONDUP_FNAME
#  define DEFAULT_NONDUP_FNAME "non-duplicates.dat"
#endif

// A class holding a pair of FILE pointers and their names. @fp_dup points 
// to the duplicates file. @fp_nondup points to the non-duplicates data
// file.
typedef struct roc_source roc_source;
struct roc_source {
  char* dupname;
  char* nondupname;
  FILE* fp_dup;
  FILE* fp_nondup;
};

roc_source* roc_source_create(){
  roc_source* source = calloc(1, sizeof(roc_source));
  return source;
}

roc_source* roc_source_init(
  roc_source* source,
  char dupname[static 1],
  char nondupname[static 1])
{
  const size_t ndup = strlen(dupname)+1;
  source->dupname = calloc(ndup, 1);
  strncpy(source->dupname, dupname, ndup);

  if(!(source->fp_dup = fopen(dupname, "r"))){
    fprintf(stderr, "Failed to open duplicates file: %s\n", dupname);
    exit(EXIT_FAILURE);
  } 

  const size_t nnondup = strlen(nondupname)+1;
  source->nondupname = calloc(nnondup, 1);
  strncpy(source->nondupname, nondupname, nnondup);

  if(!(source->fp_nondup = fopen(nondupname, "r"))){
    fprintf(stderr, "Failed to open non-duplicates file: %s\n", nondupname);
    exit(EXIT_FAILURE);
  }  

  return source;
}

roc_source* roc_source_destroy(roc_source* source){
  if(source->dupname) free(source->dupname);
  if(source->nondupname) free(source->nondupname);
  fclose(source->fp_dup);
  fclose(source->fp_nondup);
  free(source);
  return source;
}

roc_source* roc_source_reset_fp(roc_source* source){
  if(fclose(source->fp_dup) 
    || !(source->fp_dup = fopen(source->dupname, "r")))
  {
    fprintf(stderr, "Failed to reset duplicates file: %s\n",
      source->dupname);
    exit(EXIT_FAILURE);
  } 
  if(fclose(source->fp_nondup) 
    || !(source->fp_nondup = fopen(source->nondupname, "r")))
  {
    fprintf(stderr, "Failed to reset non-duplicates file: %s\n",
      source->nondupname);
    exit(EXIT_FAILURE);
  }  
  return source;
}

roc_source* roc_source_print(roc_source* psource){
  printf("dupname: %s    nondupname: %s    fp_dup: %p    fp_nondup: %p\n", 
    psource->dupname, psource->nondupname, (void*) psource->fp_dup, 
    (void*) psource->fp_nondup);
  return psource;
}

#ifdef TEST_ROC_SOURCE
int main(){
  roc_source* source = roc_source_create();
  roc_source_init(source, DEFAULT_DUP_FNAME, DEFAULT_NONDUP_FNAME);
  roc_source_print(source);
  roc_source_destroy(source);
  return EXIT_SUCCESS;
}
#endif