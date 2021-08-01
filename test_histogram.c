#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifndef GUARD_GLIB
#define GUARD_GLIB
#include <glib-2.0/glib.h>
#endif

#ifndef GUARD_BIT_ARRAY
#define GUARD_BIT_ARRAY
#include "bit_array.c"
#endif

#include "histogram.c"

void test_histogram_insert(){
  guint8 bin_index=0;
  int bit_index=1;
  histogram h={0}; 
  histogram_insert(&h, bin_index, bit_index);
  assert(1==bit_array_get(h.bins[bin_index], bit_index));
}

void test_histogram_remove(){
  guint8 bin_index=0;
  int bit_index=1;
  histogram h={0};
  histogram_insert(&h, bin_index, bit_index);
  histogram_remove(&h, bin_index, bit_index);
  assert(0==bit_array_get(h.bins[bin_index], bit_index));
}

void test_histogram_get(){
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
 * Set every bin in the histogram to 2^64-1.
 *
 * So the histogram of bins will look like this
 *
 * {2^64-1, 2^64-1, ..., 2^64-1} (256 entries).
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
 * But the importance will be 2^64 - 1 - 2^32 - 1 = G_MAXUINT64 =
 *
 * 1111111111111111111111111111111111111111111111111111111111111111
 * |                              ||                              |
 * .______________________________..______________________________.
 *             32 bits                          32 bits
 *
 * since they lie below the median, which is simply the middle, since every
 * bin has a single bit set to 1.
 *
 */
histogram* init_test_histogram_2(histogram* h){
  for(int i=0; i<256; ++i){
    h->bins[i] = G_MAXUINT64; // all 1's
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
void test_histogram_median(){
  histogram h={0};
  histogram_median(init_test_histogram_1(&h));

  assert(h.median == 128);
  memset(&h, 0, sizeof(h));

  histogram_median(init_test_histogram_2(&h));
  assert(h.median == 128);

  memset(&h, 0, sizeof(h));
  const bin_index = 0;
  const bit_index = 0;
  h.bins[bin_index] = G_MAXINT64;
  histogram_median(&h);
  assert(h.median = 0);
}


/* 
 * Test histogram_importance(histogram*).
 * 
 * Use the test histograms initialized by init_test_histogram_{1, 2}() to 
 * test the histogram_importance function. 
 *
 */
void test_histogram_importance(){
  histogram h={0};
  histogram_importance(init_test_histogram_1(&h));

  assert(h.importance == G_MAXUINT64 - G_MAXUINT32);
  memset(&h, 0, sizeof(h));

  histogram_importance(init_test_histogram_2(&h));
  assert(h.importance == G_MAXUINT64);

  assert(h.importance == G_MAXUINT64);
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

void test_histogram_process_pixel_pair(){
  const int bin_index=1-0;
  const int index=0;
  const int next=1;
  histogram h={0};
  PixelRGB pixels[64]={0};

  /* Neigboring pixels have difference 0
   */
  histogram_process_pixel_pair(&h, init_test_pixels_0(pixels), index, next);
  assert(histogram_get(&h, bin_index, index) == 0);

  memset(pixels, 0, 64*(sizeof(PixelRGB)));

  /* Neigboring pixels have difference 1
   */
  histogram_process_pixel_pair(&h, init_test_pixels_1(pixels), index, next);
  assert(histogram_get(&h, bin_index, index) == 1);
}

void test_histogram_thread_x(){
  histogram h={0};
  PixelRGB[64] pixels={0};
  histogram_thread_arg arg = {&h, &init_test_pixels_0(&pixels)};
  histogram_thread_x(&arg);
  assert(1);
  return 1;
}

void test_histogram_thread_y(){
  histogram h={0};
  PixelRGB[64] pixels={0};
  histogram_thread_arg arg = {&h, &init_test_pixels_0(&pixels)}
  histogram_thread_y(&arg);
  assert(1);
}

void test_histogram(){
  test_histogram_insert();
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
