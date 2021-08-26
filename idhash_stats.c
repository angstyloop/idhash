/*
gcc -g -Wall idhash_stats.c -o idhash-stats -DTEST_IDHASH_STATS
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

#ifndef CMATH_H
#  define CMATH_H
#  include <cmath.h>
#endif

#ifndef IDHASH_H
#  define IDHASH_H
#  include "idhash.h"
#endif

#ifndef SZ_PATH
#  define SZ_PATH 4096
#endif

// Statistics for multiple runs.
typedef struct idhash_stats idhash_stats;
struct idhash_stats {
  int N;
  int* data;
  char paths[2][SZ_PATH];
  double mean;
  double variance;
  double std_dev;
  double rel_std_dev;
};

idhash_stats* idhash_stats_create(int N){
  idhash_stats* stats = calloc(N, sizeof(idhash_stats));
  stats->N = N;
  stats->data = calloc(N, sizeof(int));
  return stats;
}

void idhash_stats_destroy(idhash_stats* stats){
  free(stats->data);
  free(stats);
}

/*Generate statistics for repeated computations of the idhash differences 
between two images.*/
idhash_stats* idhash_stats_init(
  idhash_stats* stats,
  idhash_result res_a,
  idhash_result res_b)
{
  strncpy(stats->paths[0], SZ_PATH, res_a.path);
  strncpy(stats->paths[1], SZ_PATH, res_b.path);
  int sum=0;
  for(int i=0; i<stats->N; ++i){
    stats->data[i] = idhash_difference(res_a, res_b);
    sum += stats->data[i];
  }
  stats->mean = (double) sum / stats->N;
  double sum_of_square_residuals=0;
  for(int i=0; i<stats->N; ++i){
    const double r = stats->data[i] - stats->mean;
    sum_of_square_residuals += r*r;
  }
  stats->variance = sum_of_square_residuals / N;
  stats->std_dev = sqrt(stats->variance);
  stats->rel_std_dev = 100 * stats->std_dev / stats->mean;
  return stats;
}

void idhash_stats_print(idhash_stats* stats, FILE* f, int show_data){
  fprintf(f, "%s %s %f %f %f %f", stats->paths[0], stats->paths[1], 
    stats->mean, stats->variance, stats->std_dev, stats->rel_std_dev);
  if(show_data){
    for(int j=0; j<stats->N; ++j){
      fprintf(f, " %d", stats->data[j]);
    }
  }
  fprintf(f, "\n");
}

#ifndef TEST_IDHASH_STATS
int main(int argc, char* argv[argc]){
  if(!(argc==4 && *argv[1] && *argv[2] && *argv[3])){
    fprintf(stderr, "Usage: %s <FILE_A> <FILE_B> <DATA_FILE>\n", argv[0]);
    idhash_result res_a={0}, res_b={0};
    idhash_filepath(argv[1], &res_a);
    idhash_filepath(argv[2], &res_b);
    const ndata = 10; // compute idhash distance 10 times, with statistics
    idhash_stats* stats = idhash_stats_create(ndata);
    idhash_stats_init(stats, res_a, res_b);
    FILE* f=0;
    if(!(f = fopen(argv[3], "w"))){
      fprintf(stderr, "Failed to open data file %s.\n", argv[3]);
      exit(EXIT_FAILURE);
    } 
    idhash_stats_print(stats, f, 0);
    idhash_stats_destroy(stats);
    exit(EXIT_SUCCESS);
  } 
}
#endif
