/*
gcc -g -Wall idhash_stats.c -o test-idhash-stats -DTEST_IDHASH_STATS `pkg-config vips --libs` -lm
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

#ifndef MATH_H
#  define MATH_H
#  include <math.h>
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
  int ndata;
  guint* data;
  char paths[2][SZ_PATH];
  double mean;
  double variance;
  double std_dev;
  double rel_std_dev;
};

idhash_stats* idhash_stats_create(int ndata){
  idhash_stats* stats = calloc(ndata, sizeof(idhash_stats));
  stats->ndata = ndata;
  stats->data = calloc(ndata, sizeof(guint));
  assert(stats->ndata>0);
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
  assert(stats->ndata);
  assert(res_a.path && *res_a.path);
  assert(res_b.path && *res_b.path);
  strncpy(stats->paths[0], res_a.path, SZ_PATH);
  strncpy(stats->paths[1], res_b.path, SZ_PATH);
  guint sum=0;
  for(int i=0; i < stats->ndata; ++i){
    stats->data[i] = idhash_dist(res_a, res_b);
    sum += stats->data[i];
  }
  stats->mean = (double) sum / (double) stats->ndata;
  double sum_of_square_residuals=0;
  for(int i=0; i < stats->ndata; ++i){
    const double r = stats->data[i] - stats->mean;
    sum_of_square_residuals += r*r;
  }
  stats->variance = sum_of_square_residuals / stats->ndata;
  stats->std_dev = sqrt(stats->variance);
  stats->rel_std_dev = 100 * stats->std_dev / stats->mean;
  return stats;
}

void idhash_stats_print(idhash_stats* stats, FILE* f, int show_data){
  fprintf(f, "%s %s %f %f %f %f", stats->paths[0], stats->paths[1], 
    stats->mean, stats->variance, stats->std_dev, stats->rel_std_dev);
  if(show_data){
    for(int j=0; j < stats->ndata; ++j){
      fprintf(f, " %d", stats->data[j]);
    }
  }
  fprintf(f, "\n");
}

#ifdef TEST_IDHASH_STATS
/*
Compute the idhash distance between FILE_A and FILE_B at the command line ndata 
times. Print statistics and data (the computed distances) to standard output.
*/
int main(int argc, char* argv[argc]){
  if(!(argc==4 && *argv[1] && *argv[2] && *argv[3])){
    fprintf(stderr, "Usage: %s <FILE_A> <FILE_B> <NDATA>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 
  idhash_result res_a={0}, res_b={0};
  idhash_filepath(argv[1], &res_a);
  idhash_filepath(argv[2], &res_b);
  assert(strlen(res_a.path));
  assert(strlen(res_b.path));
  int ndata = atoi(argv[3]);
  assert(ndata > 0);
  idhash_stats* stats = idhash_stats_create(ndata);
  idhash_stats_init(stats, res_a, res_b);
  idhash_stats_print(stats, stdout, 1);
  idhash_stats_destroy(stats);
  exit(EXIT_SUCCESS);
}
#endif
