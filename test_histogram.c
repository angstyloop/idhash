#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef GUARD_GLIB
#define GUARD_GLIB
#include <glib-2.0/glib.h>
#endif

#ifndef GUARD_BIT_ARRAY
#define GUARD_BIT_ARRAY
#include "bit_array.h"
#endif

#include "histogram.h"
static inline void histogram_insert(histogram* hist, guint8 j, int k);
static inline guint histogram_get(histogram* hist, guint8 j, int k);
static inline void histogram_remove(histogram* hist, int j, int k);
static inline void histogram_importance(histogram* hist);
static void histogram_median(histogram* hist);
static void histogram_process_pixel_pair(histogram* hist, PixelRGB* pixels,
  const int index, const int next);
static void* histogram_thread_y(void* _arg);
static void* histogram_thread_y(void* _arg);
static void* histogram_thread_x(void* _arg);
static inline void histogram_print_xy(histogram hist_x, histogram hist_y);


static void test_histogram_insert(){
  guint8 bin_index=0;
  int bit_index=1;
  histogram h={0}; 
  histogram_insert(&h, bin_index, bit_index);
  assert(1==bit_array_get(h.bins[bin_index], bit_index));
}

static void test_histogram_remove(){
  guint8 bin_index=0;
  int bit_index=1;
  histogram h={0};
  histogram_insert(&h, bin_index, bit_index);
  histogram_remove(&h, bin_index, bit_index);
  assert(0==bit_array_get(h.bins[bin_index], bit_index));
}

static void test_histogram_get(){
  guint8 bin_index=0;
  int bit_index=1;
  histogram h={0};
  histogram_insert(&h, bin_index, bit_index);
  assert(histogram_get(&h, bin_index, bit_index) == 1);
}

/* 
 * Math fact: 4*64==256
 *
 * For each of the 64 four-bin-wide chunks in the 256-bin (or 64-chunk) array,  * set a single bit in each of the four bins in that chunk to 1.
 *
 * Then the histogram of bins will look like this:
 * {1 1 1 1 2 2 2 2 4 4 4 4 ... 2^63 2^63 2^63 2^63}
 *  ^
 *  |
 *  .__ A single bin, represented by a 64-bit integer.
 *
 * The union will look like this:
 *
 * 2^64 - 1 = G_MAXUINT64 =
 *
 * 1111111111111111111111111111111111111111111111111111111111111111 
 *
 * But the importance will be 2^64 - 1 - 2^32 - 1 = 
 *
 * G_MAXUINT64 - G_MAXUINT32 =
 *
 * 1111111111111111111111111111111100000000000000000000000000000000
 * |                              ||                              |
 * .______________________________..______________________________.
 *             32 bits                          32 bits
 *
 * since they lie below the median, which is simply the middle, since every
 * bin has a single bit set to 1.
 *
 */
histogram* init_test_histogram_1(histogram* h){
  for(int i=0; i<256; i+=4){
    for(int j=0; j<4; ++j){
      histogram_insert(h, i, i/4);
    }
  }
  h->median = 128; // Don't worry, test_histogram_median will override this.
  return h;        //   It's just there for test_histogram_importance.
}

/* 
 * Test histogram_median(histogram*).
 * 
 * Use the test histograms initialized by init_test_histogram_{1..3}() to 
 * test the histogram_median function. 
 *
 */
static void test_histogram_median(){
  histogram h={0};
  histogram_median(init_test_histogram_1(&h));
  assert(h.median == 128);
  memset(&h, 0, sizeof(h));

  /* Put all 64 in the first bin. Then the median is just the first bin. 
   */
  const guint8 bin_index = 0;
  h.bins[bin_index] = G_MAXINT64;
  histogram_median(&h);
  assert(h.median == 0);
}


/* 
 * Test histogram_importance(histogram*).
 * 
 * Use the test histograms initialized by init_test_histogram_{1, 2}() to 
 * test the histogram_importance function. 
 *
 */
static void test_histogram_importance(){
  histogram h={0};
  histogram_importance(init_test_histogram_1(&h));
  assert(h.importance == G_MAXUINT64 - G_MAXUINT32);
}


/* Neigboring pixels have difference 0
 */
PixelRGB* init_test_pixels_0(PixelRGB pixels[64]){
  for(int i=0; i<64; ++i){
    pixels[i][0]=1;
  }
  return pixels;
}

/* Neigboring pixels have difference 1
 */
PixelRGB* init_test_pixels_1(PixelRGB pixels [64]){
  for(int i=0, j=1; i<64; i+=2, j+=2){
    pixels[i][0] = 0; 
    pixels[j][0] = 1;
  }
  return pixels;
}

static void test_histogram_process_pixel_pair(){
  histogram h={0};
  PixelRGB pixels[64]={0};

  /* Process the first (horizontal) pair of pixels created by 
   * init_test_pixels_0.
   */
  histogram_process_pixel_pair(&h, init_test_pixels_0(pixels), 0, 1);
  /* Neigboring pixels have difference 0.
   */
  assert(histogram_get(&h, 0, 0) == 1);

  memset(pixels, 0, 64*(sizeof(PixelRGB)));

  /* Process the first (horizontal) pair of pixels created by 
   * init_test_pixels_1.
   */
  histogram_process_pixel_pair(&h, init_test_pixels_1(pixels), 0, 1);
  /* Neigboring pixels have difference 1.
   */
  assert(histogram_get(&h, 1, 0) == 1);
}

static void test_histogram_thread_x(){
  histogram h={0};
  PixelRGB pixels[64]={0};
  histogram_thread_arg arg = {&h, init_test_pixels_0(pixels)};
  histogram_thread_x(&arg);
  /* All 64 horizontal neighbor pairs have difference 0;
   */
  assert(histogram_get(&h, 0, 0) == 64);
}

static void test_histogram_thread_y(){
  histogram h={0};
  PixelRGB pixels[64]={0};
  histogram_thread_arg arg = {&h, init_test_pixels_0(pixels)};
  histogram_thread_y(&arg);
  /* All 64 vertical neighbor pairs have difference 0;
   */
  assert(histogram_get(&h, 0, 0) == 64);
}

static void test_histogram(){
  test_histogram_insert();
  test_histogram_get();
  test_histogram_remove();
  test_histogram_median();
  test_histogram_importance();
  test_histogram_process_pixel_pair();
  test_histogram_thread_x();
  test_histogram_thread_y();
}

#ifdef TEST_HISTOGRAM
int main(){
  test_histogram();
  return EXIT_SUCCESS;
}
#endif
