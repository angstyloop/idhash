/*
gcc -g -Wall idhash_stats.c -o test-idhash-stats -DTEST_IDHASH_STATS `pkg-config vips --libs --cflags` -lm

more test macro constants:
TEST_IDHASH_STATS_PARSE_LINE
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

#ifndef CTYPES_H
#  define CTYPES_H
#  include <ctype.h>
#endif

#ifndef FLOAT_H
#  define FLOAT_H
#  include <float.h>
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
  char paths[2][SZ_PATH];
  int ndata;
  guint* data;
  guint min;
  guint max;
  double mean;
  double variance;
  double std_dev;
  double rel_std_dev;
};

// Note idhash_stats objects are frequently allocated statically, without
// data guint[] arrays.

idhash_stats* idhash_stats_create(int ndata){
  if(1>ndata){
    fprintf(stderr, "Error: @ndata must be a nonzero positive integer.\n");
    exit(EXIT_FAILURE);
  } 
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
  char path_a[static 1],
  char path_b[static 1])
{
  strncpy(stats->paths[0], path_a, SZ_PATH);
  strncpy(stats->paths[1], path_b, SZ_PATH);
  guint sum=0;
  stats->min = stats->data[0];
  stats->max=0;
  idhash_result res_a={0}, res_b={0};
  for(int i=0; i < stats->ndata; ++i){
    idhash_filepath(path_a, &res_a);
    idhash_filepath(path_b, &res_b);
      
    // TODO collect timing info here

    stats->data[i] = idhash_dist(res_a, res_b);

    sum += stats->data[i];
    if(stats->max < stats->data[i]) stats->max = stats->data[i];
    else if(stats->min > stats->data[i] ) stats->min = stats->data[i];
  }
  stats->mean = (double) sum / (double) stats->ndata;
  double sum_of_square_residuals=0;
  for(int i=0; i < stats->ndata; ++i){
    const double r = stats->data[i] - stats->mean;
    sum_of_square_residuals += r*r;
  }
  stats->variance = sum_of_square_residuals / stats->ndata;
  stats->std_dev = sqrt(stats->variance);
  stats->rel_std_dev = stats->mean ? 
    100 * stats->std_dev / stats->mean
    : -1;
  return stats;
}

/* Print an idhash_stats object @stats as a row in the file at @fp. If 
show_data is true, the actual distance values from all the trials are 
appended to the end of each row. 
*/
void idhash_stats_print(idhash_stats* stats, FILE* fp, int show_data){
  fprintf(fp, "%s %s %u %u %.2f %.2f %.2f %.2f", 
    stats->paths[0], stats->paths[1], stats->min, stats->max, stats->mean,
    stats->variance, stats->std_dev, stats->rel_std_dev);
  if(show_data){
    for(int j=0; j < stats->ndata; ++j){
      fprintf(fp, " %u", stats->data[j]);
    }
  }
  fprintf(fp, "\n");
}

#define IDHASH_STATS_HEADER "path_a path_b min max mean variance std_dev rel_std_dev"
void idhash_stats_print_header(FILE* fp){
  fprintf(fp, "%s\n", IDHASH_STATS_HEADER);
}

/* Exit if the null character '\0' is at @p 
*/
static void null_check(char p[static 1]){
  if(!*p){
    fprintf(stderr, "Null byte encountered early.\n");
    exit(EXIT_FAILURE);
  } 
}

static void header_error(char* line){
  if(line){
    fprintf(stderr, "malformed header line:\n%s\n", line);
    free(line);
  }
  exit(EXIT_FAILURE);
}

// 1 or more digits at the end of the line
#define HEADER_VALUE_REG "[:digit:]+$"

// Parse the header, which is currently just the number of files and number 
// of trials.
void idhash_stats_parse_header(
  int* nfiles[static 1],
  int* ndata[static 1],
  FILE* fp)
{
  {
    char* line=0, p=0;
    size_t len=0;
    ssize_t nread=0;
    //parse number of files (@nfiles) by extracting regex match from 1st line
    if(1>(nread = getline(&line, &len, fp))
      || ! (p = extract_match(line, HEADER_VALUE_REG))){
      header_error(line);
    }
    free(line);
    *nfiles = strtoul(p, 0, 0);
    free(p);
  }

  {
    char* line=0, p=0;
    size_t len=0;
    ssize_t nread=0;
    //parse number of trials (@ndata) by extracting regex match from 2nd line
    if(1>(nread = getline(&line, &len, fp))
      || ! (p = extract_match(line, HEADER_VALUE_REG))){
      header_error(line);
    }
    free(line);
    *ndata = strtoul(p, 0, 0);
    free(p); 
  }

}

/* Parse @line - which is at least one character long, presumably the 
terminating null byte - into @stats, an idhash_stats object, presumably 
created with idhash_stats_create, but not initialized with 
idhash_stats_init. Does not set idhash_stats::data, nor ::ndata.
*/
void idhash_stats_parse_line(idhash_stats* stats, char line[static 1]){
  null_check(line);

  char* p=0, * q=line;

  for(p=q; isspace(*p); ++p);
  null_check(p);
  for(q=p; !isspace(*q); ++q);
  null_check(q);
  memcpy(stats->paths[0], p, q-p);

  for(p=q; isspace(*p); ++p);
  null_check(p);
  for(q=p; !isspace(*q); ++q);
  null_check(q);
  memcpy(stats->paths[1], p, q-p);

  stats->min = strtoul(p=q, &q, 10);
  null_check(q);

  stats->max = strtoul(p=q, &q, 10);
  null_check(q);

  stats->mean = strtod(p=q, &q);
  null_check(q);

  stats->variance = strtod(p=q, &q);
  null_check(q);

  stats->std_dev = strtod(p=q, &q);
  null_check(q);

  stats->rel_std_dev = strtod(p=q, 0);
}

/* Determine key features of a data file at @fp, produced by 
idhash_directory, such  max/min values of the statistical quantities # 
files, and # trials. Currently just finds the min and max means. These
determine the range used to vary the threshold to produce the ROC curve.
*/
void idhash_stats_process_data_file(guint* min, guint* max, FILE* fp){
  int nfiles=0, ndata=0;
  idhash_stats_parse_header(&nfiles, &ndata, fp);
  char* line=0;
  size_t len=0;
  ssize_t nread=0;
  idhash_stats stats={0}; // static alloc fine (data not parsed)

  double lowest_mean = DBL_MAX, highest_mean=0;

  while(0<(nread = getline(&line, &len, fp))){
    idhash_stats_parse_line(&stats, line);
    if(lowest_mean > stats.mean) highest_mean = stats.mean;
    else if(highest_mean < stats.mean) lowest_mean = stats.mean;
    free(line);
  }

  if(!(lowest_mean < DBL_MAX && highest_mean < DBL_MAX)){
    fprintf(stderr, "mean exceeds DBL_MAX\n");
    exit(EXIT_FAILURE);
  } 

  if(!(lowest_mean >= 0 && highest_mean >= 0)){
    fprintf(stderr, "mean is negative\n");
    exit(EXIT_FAILURE);
  } 

  *min = (guint)(lowest_mean);//floor
  *max = (guint)(highest_mean + 1);//ceil
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
  int ndata = atoi(argv[3]);
  assert(ndata > 0);
  idhash_stats* stats = idhash_stats_create(ndata);
  idhash_stats_init(stats, argv[1], argv[2]);
  idhash_stats_print(stats, stdout, 1);
  idhash_stats_destroy(stats);
  exit(EXIT_SUCCESS);
}
#elif TEST_IDHASH_STATS_PARSE_LINE
int main(){
  char* line = "duplicates/9_a.jpg duplicates/9_b.jpg 9.000000 1.000000 1.000000 11.111111\n";
  puts(line);
  idhash_stats stats={0}; // don't need data, so can allocate statically
  idhash_stats_parse_line(&stats, line);
  idhash_stats_print(&stats, stdout, 1);
  exit(EXIT_SUCCESS);
}
#endif
