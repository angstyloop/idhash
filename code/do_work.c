/*
gcc do_work.c -o do-work -DTEST_DO_WORK -g -Wall `pkg-config vips --cflags --libs` -lm -luuid

Pre-step

(x == coded)

[x] Get a test set of images using the applescript/javascript google chrome 
searcher. 

[x] Count the images.

[x] Generate numbered files.

[x] Generate duplicates/ and non-duplicates/. These are directories containing
    pairs of images. Each pair will be classified as a duplicate or a 
    non-duplicate, based on the idhash_distance between the two images, and
    a threshold that will be tuned by picking the "perfect classifier" point
    on a Receiver Operating Characteristic Curve (ROC).

x] Run idhash_directory on duplicates/ and non-duplicates/. This will take
    the idhash distance, averaged over many iterations, for each pair, and
    print these values and more statistics to a data file. After calling 
    idhash_directory twice, the result will be two data files, one for
    the duplicates directory, and one for the non-duplicates directory.

[x] Use idhash_stats_process_data_file to get the ranges for the threshold 
    from the .dat files.

[x] After making an roc_source from the data files, use roc_optimal_threshold 
    to compute the optimal threshold based on the data.

[x] With this threshold in hand, make a plotfile for gnuplot. 

[x] Check that the optimal threshold actually looks optimal.

[ ] Plot the ROC with gnuplot.

[ ] TeX the things

[ ] Containerize the shee

Done!
*/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
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

#ifndef FCNTL_H
#  define FCNTL_H
#  include <fcntl.h>
#endif

#ifndef VIPS_H
#  define VIPS_H
#  include <vips/vips.h>
#endif

#ifndef COUNT_IMAGES_H
#  define COUNT_IMAGES_H
#  include "count_jpegs.c"
#endif

#ifndef GENERATE_NUMBERED_FILES_H
#  define GENERATE_NUMBERED_FILES_H
#  include "generate_numbered_files.c"
#endif

#ifndef GENERATE_DUPLCATES_H
#  define GENERATE_DUPLCATES_H
#  include "generate_duplicates.c"
#endif

#ifndef GENERATE_NONDUPLICATES_H
#  define GENERATE_NONDUPLICATES_H
#  include "generate_nonduplicates.c"
#endif

#ifndef IDHASH_DIRECTORY_H
#  define IDHASH_DIRECTORY_H
#  include "idhash_directory.c"
#endif

#ifndef ROC_SOURCE_H
#  define ROC_SOURCE_H
#  include "roc_source.c"
#endif

#ifndef ROC_POINT_H
#  define ROC_POINT_H
#  include "roc_point.c"
#endif

guint max(guint x, guint y){
  return x < y ? y : x;
}

guint min(guint x, guint y){
  return x > y ? y : x;
}

void range_containing(guint range_out[2], guint range_a[2], guint range_b[2]){
  range_out[0] = min(range_a[0], range_b[0]);
  range_out[1] = max(range_a[1], range_b[1]);
}

#ifndef DEFAULT_JPEGS_DIR
#  define DEFAULT_JPEGS_DIR "/home/falkor/src/idhash/jpegs"
#endif

#ifndef DEFAULT_NUMBERED_JPEGS_DIR
#  define DEFAULT_NUMBERED_JPEGS_DIR "/home/falkor/src/idhash/numbered-jpegs"
#endif

#ifndef DEFAULT_DUPLICATES_DIR
#  define DEFAULT_DUPLICATES_DIR "/home/falkor/src/idhash/duplicates"
#endif

#ifndef DEFAULT_NONDUPLICATES_DIR
#  define DEFAULT_NONDUPLICATES_DIR "/home/falkor/src/idhash/non-duplicates"
#endif

#ifndef DEFAULT_DUPLICATES_DATA_FILE
#  define DEFAULT_DUPLICATES_DATA_FILE "/home/falkor/src/idhash/duplicates.dat"
#endif

#ifndef DEFAULT_NONDUPLICATES_DATA_FILE
#  define DEFAULT_NONDUPLICATES_DATA_FILE "/home/falkor/src/idhash/non-duplicates.dat"
#endif

#ifndef DEFAULT_ROC_PLOT_FILE
#  define DEFAULT_ROC_PLOT_FILE "/home/falkor/src/idhash/roc.plot"
#endif

#ifndef DEFAULT_ITERATIONS
#  define DEFAULT_ITERATIONS 100
#endif

void do_work(){
  // count the images (tested)
  unsigned jpeg_count=0;
  count_jpegs(&jpeg_count, DEFAULT_JPEGS_DIR);

  // generate numbered files (tested)
  generate_numbered_files(DEFAULT_JPEGS_DIR, DEFAULT_NUMBERED_JPEGS_DIR, 1);

  // generate duplicates and non-duplicates (tested)
  generate_duplicates(DEFAULT_NUMBERED_JPEGS_DIR, DEFAULT_DUPLICATES_DIR);
  generate_nonduplicates(DEFAULT_NUMBERED_JPEGS_DIR, DEFAULT_NONDUPLICATES_DIR);

  // run idhash_directory on duplicates/ and non-duplicates/ (winging it)
  int iterations = DEFAULT_ITERATIONS;
  idhash_directory(DEFAULT_DUPLICATES_DIR, DEFAULT_DUPLICATES_DATA_FILE, 
    jpeg_count, iterations);
  idhash_directory(DEFAULT_NONDUPLICATES_DIR, DEFAULT_NONDUPLICATES_DATA_FILE,
    jpeg_count, iterations);

  // use idhash_stats_process_data_file to get the range for the threshold. 
  // compute the smallest range containing both ranges.

  // duplicates
  guint dup_range[2]={0};
  FILE* fp=0;
  if(!(fp = fopen(DEFAULT_DUPLICATES_DATA_FILE, "r"))){
    fprintf(stderr, "Failed to print duplicates data file %s\n",
      DEFAULT_DUPLICATES_DATA_FILE);
    exit(EXIT_FAILURE);
  }  
  idhash_stats_process_data_file(dup_range, dup_range+1, fp); //(tested)
  fclose(fp);

  // nonduplicates
  guint nondup_range[2]={0};
  if(!(fp = fopen(DEFAULT_NONDUPLICATES_DATA_FILE, "r"))){
    fprintf(stderr, "Failed to print non-duplicates data file %s\n",
      DEFAULT_NONDUPLICATES_DATA_FILE);
    exit(EXIT_FAILURE);
  }
  //(tested)
  idhash_stats_process_data_file(nondup_range, nondup_range+1, fp);
  fclose(fp);

  guint thresh_range[2]={0};
  range_containing(thresh_range, dup_range, nondup_range); //(winging it)

  // compute the optimal threshold based on the data
  guint threshold=0;
  roc_source* psource = roc_source_create();
  roc_source_init(psource, DEFAULT_DUPLICATES_DATA_FILE, 
    DEFAULT_NONDUPLICATES_DATA_FILE);
  roc_optimal_threshold(&threshold, psource, thresh_range); //(winging it)
  printf("optimal threshold: %u\n", threshold);

  // make a plotfile for gnuplot
  if(!(fp = fopen(DEFAULT_ROC_PLOT_FILE, "w"))){
    fprintf(stderr, "Failed to open plot file %s\n", DEFAULT_ROC_PLOT_FILE);
    exit(EXIT_FAILURE);
  } 
  roc_curve_print(psource, fp, thresh_range); //(winging it)

  // clean up
  roc_source_destroy(psource);
  fclose(fp);
}

#ifdef TEST_DO_WORK
int main() {
  do_work();
  return EXIT_SUCCESS;
}
#endif
