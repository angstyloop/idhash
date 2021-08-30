/* idhash_directory.c
 *
 * Compute the idhash distance of every pair of image files in a directory,   
 * where the pairs follow the naming convention "i_a.jpg" / "i_b.jpg", i
 * an integer in [1, n]. Write the distances and names to a file @data, with 
 * each line corresponding to a single pair.
 * 
 * e.g. 
 * 
 * <name a> <name b> <distance> 
 *
 * COMPILE
 * 
gcc -o idhash-directory -g -Wall idhash.h bit_array.h histogram.h idhash_directory.c `pkg-config vips --cflags --libs` -luuid -lm
 *
 * RUN
 *
 * ./idhash-directory <TARGET_DIR> <DATA_FILE> <N_FILES> <N_DATA>
 *
 */

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef ASSERT_H
#  define ASSERT_H
#  include <assert.h>
#endif

#ifndef STAT_H
#  define STAT_H
#  include <sys/stat.h>
#endif

#ifndef TYPES_H
#  define TYPES_H
#  include <sys/types.h>
#endif

#ifndef UNISTD_H
#  define UNISTD_H
#  include <unistd.h>
#endif

#ifndef CMATH_H
#  define CMATH_H
#  include <math.h>
#endif

#ifndef IDHASH_H
#  define IDHASH_H
#  include "idhash.h"
#endif

#ifndef IDHASH_STATS_H
#  define IDHASH_STATS_H
#  include "idhash_stats.c"
#endif

#ifndef SZ_PATH
#  define SZ_PATH 4096
#endif

/*
Compute the idhash between pair of @nfiles image files in @dir. Repeat the 
computation @ndata times for each pair, and print statistics such as mean
and standard deviation to @dat.
*/
void idhash_directory(char dir[static 1], char dat[static 1],
  int nfiles, int ndata)
{
  char path_a[SZ_PATH]={0}, path_b[SZ_PATH]={0};
  FILE* f_dat; 
  if(!(f_dat = fopen(dat, "w"))){
    fprintf(stderr, "Failed to open data file %s\n", dat);
    exit(EXIT_FAILURE);
  } 
  idhash_stats* stats = idhash_stats_create(ndata);
  for(int i=1; i<nfiles+1; ++i){
    char* slash = dir[strlen(dir)-1] == '/' ? "" : "/";
    snprintf(path_a, SZ_PATH, "%s%s%d_a.jpg", dir, slash, i);
    snprintf(path_b, SZ_PATH, "%s%s%d_b.jpg", dir, slash, i);
    idhash_stats_init(stats, path_a, path_b);
    // print stats to file
    idhash_stats_print(stats, f_dat, 0); // 0 => don't print data
  }
  idhash_stats_destroy(stats);
  fclose(f_dat);
}

int main(int argc, char* argv[argc]){
  if(!(argc==5 && *argv[1] && *argv[2] && *argv[3] && *argv[4])){
    fprintf(stderr, "Usage: %s <TARGET_DIR> <DATA_FILE> <N_MAX>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  idhash_directory(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));
  return EXIT_SUCCESS;
}

