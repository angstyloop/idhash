/*
gcc do_work.c -o do-work -DTEST_DO_WORK -g -Wall

Pre-step

(x == coded)

[x] Get a test set of images using the applescript/javascript google chrome 
searcher. 

Code starts here

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

[] Use idhash_stats_process_data_file to get the ranges for the threshold 
   from the .dat files.

[] After making an roc_source from the data files, use roc_optimal_threshold 
   to compute the optimal threshold based on the data.

[] With this threshold in hand, make a plotfile for gnuplot. Check that
   the optimal threshold actually looks optimal.

[] Plot the ROC with gnuplot.

[] TeX the things

[] Containerize the shee

Done!
*/

#define DEFAULT_JPEGS_DIR "/home/falkor/source/idhash/jpegs"
#define DEFAULT_NUMBERED_JPEGS_DIR "/home/falkor/source/idhash/numbered-jpegs"
#define DEFAULT_DUPLICATES_DIR "/home/falkor/source/idhash/duplicates"
#define DEFAULT_NONDUPLICATES_DIR "/home/falkor/source/idhash/non-duplicates"
#define DEFAULT_DUPLICATES_DATA_FILE "/home/falkor/source/idhash/duplicates.dat"
#define DEFAULT_NONDUPLICATES_DATA_FILE "/home/falkor/source/idhash/non-duplicates.dat"

void do_work(){
  // count the images
  char* dname = "foo";
  unsigned image_count=0;
  count_images(&count, DEFAULT_JPEGS_DIR);

  // generate numbered files
  generate_numbered_files(DEFAULT_JPEGS_DIR, DEFAULT_NUMBERED_JPEGS_DIR, 1);

  // generate duplicates and non-duplicates
  generate_duplicates(DEFAULT_NUMBERED_JPEGS_DIR, DEFAULT_DUPLICATES_DIR);
  generate_nonduplicates(DEFAULT_NUMBERED_JPEGS_DIR, DEFAULT_NONDUPLICATES_DIR);

  // run idhash_directory on duplicates/ and non-duplicates/
  int iterations = 1000;
  idhash_directory(DEFAULT_DUPLICATES_DIR, DEFAULT_DUPLICATES_DATA_FILE, 
    image_count, iterations);

  // 

}

#ifdef test_do_work
int main() {
  printf("Hey screw you buddy!");
  return EXIT_SUCCESS;
}
#endif
