/*
gcc -g -Wall roc_point.c -o test-roc-point -DTEST_ROC_POINT `pkg-config vips --libs --cflags` -lm

gcc -g -Wall roc_point.c -o test-roc-curve-print -DTEST_ROC_CURVE_PRINT `pkg-config vips --libs --cflags` -lm



gcc -g -Wall roc_point.c -o test-roc-optimal-threshold -DTEST_ROC_OPTIMAL_THRESHOLD `pkg-config vips --libs --cflags` -lm

The ROC curve is a graphical representation of the predictive power of a 
classifier. It shows how much of an improvement the classifier is over a
random one (a coin flip). It was first used in the 1940s to quantify the 
performance of radar operators (who had to classify pings as friend or 
enemy).

The area under the curve (AUC), a value in [0,1], is used to quantify 
the predictive power of a classifier. In medicine, for breast tumor 
classification, .9 is considered good enough to be acceptable.
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

#ifndef ROC_SOURCE_H
#  define ROC_SOURCE_H
#  include "roc_source.c"
#endif

#ifndef DEFAULT_DUPLICATES_DATA_FILE
#  define DEFAULT_DUPLICATES_DATA_FILE "/home/falkor/idhash/duplicates.dat"
#endif

#ifndef DEFAULT_NONDUPLICATES_DATA_FILE
#  define DEFAULT_NONDUPLICATES_DATA_FILE "/home/falkor/idhash/non-duplicates.dat"
#endif

typedef struct roc_point roc_point;
struct roc_point {
  double fpr; //x-coordinate of a point in the ROC curve
  double tpr; //y-coordinate of a point in the ROC curve
};

// Go through duplicates data file and count the true positives and false
// negatives. Compute the true positive rate TPR, and set the y-coordinate
// of the roc_point at @point.
void roc_point_parse_dup_file(
  roc_point* point,
  roc_source* source,
  guint threshold)
{
  // Counts of true positives and false negatives.
  int tp=0, fn=0;
  // The idhash_stats::data field of @stats is empty, so static alloc fine.
  idhash_stats stats={0}; 
  // Read lines until end of file, or a getline error.
  char* line=0;
  size_t n=0;
  ssize_t z=0;
  while(0<(z = getline(&line, &n, source->fp_dup))){
    idhash_stats_parse_line(&stats, line);
    // If the duplicates are classified as... 
    // non-duplicates, it's a false negative.
    if(stats.mean > threshold) ++fn;
    // duplicates, it's a true positive.
    else ++tp; 
  }
  if(line) free(line);

  // Compute the true positive rate (x-axis of ROC curve).
  point->tpr = (tp + fn) ? (double) tp / (tp + fn) : -1;
  roc_source_reset_fp(source);
}

// Go through duplicates data file in @source and count the true negatives  
// and false positives.
void roc_point_parse_nondup_file(
  roc_point* point,
  roc_source* source,
  guint threshold)
{
  // counters for true negatives and false positives.
  int tn=0, fp=0;

  idhash_stats stats={0};
  
  // Read each line and act on it, until EOF is reached or getline error.
  char* line=0;
  size_t n=0;
  ssize_t z=0;
  while(0<(z = getline(&line, &n, source->fp_nondup))){
    idhash_stats_parse_line(&stats, line);
    // If the non-duplicates are classified as...
    // non-duplicates, it's a true negative
    if(stats.mean > threshold) ++tn;
    // duplicates, it's a false positive.
    else ++fp;
  }
  if(line) free(line);

  // Compute the false positive rate (y-axis of ROC curve)
  point->fpr = (fp + tn) ? (double) fp / (fp + tn) : -1;
  roc_source_reset_fp(source);
}

/* For a given data file and threshold value of the idhash distance, 
compute the true positive rate (TPR) and false positive rate (FPR).

TPR = TP / (TP + FN)
FPR = FP / (FP + TN)
*/
roc_point* roc_point_init(
  roc_point* ppoint,
  roc_source* source,
  guint threshold)
{
  roc_point_parse_dup_file(ppoint, source, threshold);
  roc_point_parse_nondup_file(ppoint, source, threshold);
  return ppoint;
}

// Square distance to (0, 1) on the ROC curve. At this point, FPR = 0, 
// and TPR = 1. In other words, there are no false positives, and there are 
// no false negatives. This distance is a measure of the (average) 
// predictive power of the duplicate detector, as compared to a random 
// duplicate/nonduplicate classifier.
double roc_square_distance_to_optimal(roc_point* p){
  return p->fpr * p->fpr + (1 - p->tpr) * (1 - p->tpr);
}

// Write to the file at @file_out the xy coordinates of points on the ROC
// curve generated by the @range of threshold values. The right endpoint,
// range[1], is not included, but the left endpoint is included - i.e. the
// range is "half-open". Use the duplicates.dat file at @file_dup, and the 
// non-duplicates.dat file at @file_nondup. 
//
// The output file is formatted for gnuplot. There are two columns: the 
// first column lists x values, the second column lists y values.
void roc_curve_print(
  roc_source* source,
  FILE* file_out,
  guint range[2])
{ 
  fprintf(file_out, "# FPR TPR threshold\n");
  for(guint i=range[0]; i<range[1]; ++i){
    roc_point point={0};
    roc_point_init(&point, source, i);
    fprintf(file_out, "%f %f %u\n", point.fpr, point.tpr, i);
  }
}

void roc_optimal_threshold(
  guint* threshold,
  roc_source* source,
  guint range[2])
{
  double dmin=DBL_MAX;
  for(guint i=range[0]; i<range[1]; ++i){
    roc_point point={0};
    roc_point_init(&point, source, i);
    double d = roc_square_distance_to_optimal(&point);
    if(dmin>d) {
      dmin=d;
      *threshold=i;
    };
  }
}

#ifdef TEST_ROC_POINT
int main(int argc, char** argv){
  if(2!=argc){
    fprintf(stderr, "Usage: %s <THRESHOLD>\n", argv[0]);
    exit(EXIT_FAILURE);
  } 

  roc_source* source = roc_source_create();
  roc_source_init(source, DEFAULT_DUPLICATES_DATA_FILE, DEFAULT_NONDUPLICATES_DATA_FILE);

  roc_point point={0};
  roc_point_init(&point, source, strtoul(argv[1], 0, 0));

  printf("fpr: %f    tpr: %f\n", point.fpr, point.tpr);

  roc_source_destroy(source);

  return EXIT_SUCCESS;
}
#elif TEST_ROC_CURVE_PRINT
int main(){
  roc_source* source = roc_source_create();
  roc_source_init(source, DEFAULT_DUPLICATES_DATA_FILE, DEFAULT_NONDUPLICATES_DATA_FILE);
  guint range[2] = {0, 50};
  roc_curve_print(source, stdout, range);
  roc_source_destroy(source);
  return EXIT_SUCCESS;
}
#elif TEST_ROC_OPTIMAL_THRESHOLD
int main(){
  roc_source* source = roc_source_create();
  roc_source_init(source, DEFAULT_DUPLICATES_DATA_FILE, DEFAULT_NONDUPLICATES_DATA_FILE);
  guint threshold=0;
  guint range[2] = {0, 50};
  roc_optimal_threshold(&threshold, source, range);
  printf("%u\n", threshold);
  roc_source_destroy(source);
  return EXIT_SUCCESS;
}
#endif

