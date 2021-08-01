#ifndef GUARD_GLIB
#define GUARD_GLIB
#include <glib-2.0/glib.h>
#endif

#ifndef GUARD_BIT_ARRAY
#define GUARD_BIT_ARRAY
#include "bit_array.c"
#endif

#ifndef GUARD_STDIO
#define GUARD_STDIO
#include <stdio.h>
#endif

/* A 3-d vector of 8-bit integers [0, 255], representing a single RGB pixel.
 */
typedef guint8 PixelRGB[3];

/* The histogram. The median gets set by the histogram_median function.
 * The histogram_importance function unions the bins - excluding those below
 * the median - to produce the importance array.
 * 
 * Each bin is a bit array represented by a 64-bit integer.
 */
typedef struct histogram histogram;
struct histogram {
  guint64 bins[256];
  int median;
  int difference[64];
  guint64 hash;
  guint64 importance;
};

/* Set the kth bit in the jth bin to 1.
 */
static inline void histogram_insert(histogram* hist, guint8 j, int k){ 
  bit_array_set(&hist->bins[j], k);
}

/* Get the value of the kth bit in the jth bin. 
 */
static inline guint histogram_get(histogram* hist, guint8 j, int k){
  return bit_array_get(hist->bins[j], k);
}

/* Set set the kth bit in the jth bin to 0.
 */
static inline void histogram_remove(histogram* hist, int j, int k){
 bit_array_unset(hist->bins+j, k);
}

/* Union the (empty) importance array together with the bins, but excluding 
 * bins below the median.
 */
static inline void histogram_importance(histogram* hist){
  for (int i=hist->median; i<256; i++) {
    hist->importance |= hist->bins[i];
  }
}

/* Find and set histogram.median by summing the 1 bits in each bin until
 * half the the number of differences (64/2=32) is reached. Then save
 * the index of the bin where 32 was reached. If the two middle differences
 * are the same, this is the index of the median difference. If the two middle
 * differences aren't the same, this is the index of the first difference
 * exceeding the median (since the median is an odd multiple of 1/2 in that
 * case).
 */
static void histogram_median(histogram* hist) {
  for (int count=0, i=0; i<256; i++) {
    if ((count += bit_array_sum(hist->bins[i])) > 32) {
      hist->median=i;
      break;
    }
  }
}

/* Argument to histogram_thread. Each thread owns its own histogram,
 * but the PixelRGB array is shared and immutable.
 */
typedef struct histogram_thread_arg histogram_thread_arg;
struct histogram_thread_arg {
  histogram* hist;
  PixelRGB* pixels;
};

/* Insert the difference between the pixel at index and the next one 
 * into the histogram of bit arrays.
 *
 * Can use this same method for both x- and y-direction differences because
 * pixels is a flat array representation of a 2-d array.
 */
static void histogram_process_pixel_pair(
  histogram* hist,
  PixelRGB* pixels,
  const int index,
  const int next)
{
  hist->difference[index] = pixels[next][0] - pixels[index][0];
  if (hist->difference[index] > 0)
    bit_array_set(&hist->hash, index);
  const int d = hist->difference[index];
  histogram_insert(hist, (guint8)(d >= 0 ? d : -d), index);
}

/* Compute the y-direction difference hash and importance.
 */
static void* histogram_thread_y(void* _arg) {
  histogram_thread_arg* arg = (histogram_thread_arg*) _arg;
  for (int x=0; x<8; x++) {
    for (int y=0; y<7; y++) {
      const int index = x + 8*y;
      const int next = x + 8*(y + 1);
      histogram_process_pixel_pair(arg->hist, arg->pixels, index, next);
    }
    const int first = x + 8*0;
    const int last = x + 8*7;
    histogram_process_pixel_pair(arg->hist, arg->pixels, first, last);
  }
  histogram_median(arg->hist);
  histogram_importance(arg->hist);
  pthread_exit(NULL);
}

/* Compute the x-direction difference hash and importance. 
 */
static void* histogram_thread_x(void* _arg) {
  histogram_thread_arg* arg = (histogram_thread_arg*) _arg;
  for (int y=0; y<8; y++) {
    for (int x=0; x<7; x++) {
      const int index = x + 8*y;
      const int next = x + 1 + 8*y;
      histogram_process_pixel_pair(arg->hist, arg->pixels, index, next);
    }
    const int last = 7 + 8*y;
    const int first = 0 + 8*y;
    histogram_process_pixel_pair(arg->hist, arg->pixels, first, last);
  }
  histogram_median(arg->hist);
  histogram_importance(arg->hist);
  pthread_exit(NULL);
}

/* Print the difference hashes and importances for both x- and y-directions
 * for a given x-histogram and y-histogram.
 */
static inline void histogram_print_xy(histogram hist_x, histogram hist_y) {
  printf("%" G_GUINT64_FORMAT " %" G_GUINT64_FORMAT 
    " %" G_GUINT64_FORMAT " %" G_GUINT64_FORMAT "\n",
    hist_x.hash, hist_y.hash,
    hist_x.importance, hist_y.importance);
}

