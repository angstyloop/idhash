/*
gcc do_work.c -o do-work -DTEST_DO_WORK -g -Wall

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

[x] Run idhash_directory on duplicates/ and non-duplicates/. This will take
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

typdef range guint[2];

#define max(pmax, x, y) do {\
  *pmax = x < y ? y : x;    \
}while(0);

#define min(pmin, x, y) do {\
  *pmin = x > y ? y : x;     \
}while(0);

void range_containing(range* range_out, range range_a, range range_b){
  min(&range_out[0], range_a[0], range_b[0]);
  max(&range_out[1], range_a[1], range_b[1]);
}

#define DEFAULT_JPEGS_DIR "/home/falkor/source/idhash/jpegs"
#define DEFAULT_NUMBERED_JPEGS_DIR "/home/falkor/source/idhash/numbered-jpegs"
#define DEFAULT_DUPLICATES_DIR "/home/falkor/source/idhash/duplicates"
#define DEFAULT_NONDUPLICATES_DIR "/home/falkor/source/idhash/non-duplicates"
#define DEFAULT_DUPLICATES_DATA_FILE "/home/falkor/source/idhash/duplicates.dat"
#define DEFAULT_NONDUPLICATES_DATA_FILE "/home/falkor/source/idhash/non-duplicates.dat"
#define DEFAULT_ROC_PLOT_FILE "/home/falkor/source/idhash/roc.plot"

void do_work(){
  // count the images (tested)
  char* dname = "foo";
  unsigned image_count=0;
  count_images(&count, DEFAULT_JPEGS_DIR);

  // generate numbered files (tested)
  generate_numbered_files(DEFAULT_JPEGS_DIR, DEFAULT_NUMBERED_JPEGS_DIR, 1);

  // generate duplicates and non-duplicates (tested)
  generate_duplicates(DEFAULT_NUMBERED_JPEGS_DIR, DEFAULT_DUPLICATES_DIR);
  generate_nonduplicates(DEFAULT_NUMBERED_JPEGS_DIR, DEFAULT_NONDUPLICATES_DIR);

  // run idhash_directory on duplicates/ and non-duplicates/ (winging it)
  int iterations = 1000;
  idhash_directory(DEFAULT_DUPLICATES_DIR, DEFAULT_DUPLICATES_DATA_FILE, 
    image_count, iterations);

  // use idhash_stats_process_data_file to get the range for the threshold. 
  // compute the smallest range containing both ranges.

  // duplicates
  range dup_range={0};
  FILE* fp=0;
  if(!(fp = fopen(DEFAULT_DUPLICATES_DATA_FILE, "r"))){
    fprintf(stderr, "Failed to print duplicates data file %s\n",
      DEFAULT_DUPLICATES_DATA_FILE);
    exit(EXIT_FAILURE);
  }  
  idhash_stats_process_data_file(&dup_range[0], &dup_range[1], fp); //(tested)
  fclose(fp);

  // nonduplicates
  range nondup_range={0};
  if(!(fp = fopen(DEFAULT_NONDUPLICATES_DATA_FILE, "r"))){
    fprintf(stderr, "Failed to print non-duplicates data file %s\n",
      DEFAULT_NONDUPLICATES_DATA_FILE);
    exit(EXIT_FAILURE);
  }
  //(tested)
  idhash_stats_process_data_file(&nondup_range[0], &nondup_range[1], fp);
  fclose(fp);

  range thresh_range={0};
  range_containing(&thresh_range, dup_range, nondup_range); //(winging it)

  // compute the optimal threshold based on the data
  guint threshold=0;
  roc_source* psource = roc_source_create();
  roc_source_init(psource, DEFAULT_DUPLICATES_DATA_FILE, 
    DEFAULT_NONDUPLICATES_DATA_FILE);
  roc_optimal_threshold(&threshold, psource, thresh_range); //(winging it)
  roc_source_destroy(psource);

  // make a plotfile for gnuplot
  if(!(fp = fopen(DEFAULT_ROC_PLOT_FILE, "r"))){
    fprintf(stderr, "Failed to open plot file %s\n", DEFAULT_ROC_PLOT_FILE);
    exit(EXIT_FAILURE);
  } 
  roc_curve_print(psource, fp, thresh_range); //(winging it)
  fclose(fp);
}

#ifdef test_do_work
int main() {
  printf("Hey screw you buddy!");
  return EXIT_SUCCESS;
}
#endif
