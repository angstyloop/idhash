#ifndef GUARD_VIPS
#define GUARD_VIPS
#include <vips/vips.h>
#endif

#include <inttypes.h>

/********************************************************************** 
 *                              DATA TYPES                            *  
 **********************************************************************/ 

/* A result containing the x-direction difference hash, y-direction difference
 * hash, x-direction importance, and y-direction importance, in that order.
 */
typedef struct idhash_result idhash_result;
struct idhash_result {
  guint64 dx,
  guint64 dy,
  guint64 ix,
  guint64 iy
};

/**********************************************************************  
 *                            INTERFACE                               *
 *********************************************************************/

/* Compute the distance between two IDHashes, described by the given components
 * of difference hashes and importances, all represented by bit arrays
 * implemented on top of 64-bit integers.
 *
 * Math:
 *
 * 1. OR() the importances (union)
 * 2. XOR() the difference hashes (symmetric difference: in one, but not both) 
 * 3. AND() results 1 and 2 (intersection)
 * 4. sum the 1's (counting members)
 * 5. add the sums for each component (adding orthogonal components).
 */
static inline guint idhash_distance(
  guint64 difference_hash_1_x,
  guint64 difference_hash_1_y,
  guint64 importance_1_x,
  guint64 importance_1_y,
  guint64 difference_hash_2_x, 
  guint64 difference_hash_2_y,
  guint64 importance_2_x,
  guint64 importance_2_y);


/* Compute the IDHash Distance between the two IDHashes whose components are 
 * stored in the two given idhash_result objects.
 */
static inline guint idhash_dist(idhash_result res_1, idhash_result res_2);

/* Print the x-direction difference hash, y-direction difference hash,
 * x-direction importance, and y-direction importance, in that order,
 * for a given IDHash result.
 */
static inline void idhash_print_result(idhash_result res);


/* Print the difference hash given a histogram.
 */
static inline void histogram_print_hash(histogram hist);


/* Print the importance given a histogram.
 */
static inline void histogram_print_importance(histogram hist);

/* Computes the x and y IDHashes, on two concurrent threads.
 *
 * Prints the four bit arrays representing the x and y difference hashes and
 * importances as four 64-bit integers, in this order:
 *
 * <dx> <dy> <ix> <iy>
 *
 * The IDHash Distance between two images is computed as:
 * 
 * dist = sum( (d1x ^ d2x) & (i1x | i2x) ) + sum( (d1y ^ d2y) & (i1y | i2y) )
 *
 * where the pseudocode function sum() counts the number of 1 bits, and the
 * symbole ^, &, and | correspond to boolean logic operators XOR, AND, and OR r * respectively.
 *
 * The two components are orthogonal and can be computed separately, which 
 * this implementation does on two separate threads.
 */
void idhash_pixels(
  PixelRGB *pixels,
  int width,
  int height,
  idhash_result* res);

/* Write the the IDHash Components for the image at @filepath to standard 
 * output.
 */
void idhash_filepath(char* filepath, idhash_result* res);

/**********************************************************************/  
/*                           IMPLEMENTATION                           */
/**********************************************************************/ 

static inline guint idhash_distance(
  guint64 difference_hash_1_x,
  guint64 difference_hash_1_y,
  guint64 importance_1_x,
  guint64 importance_1_y,
  guint64 difference_hash_2_x, 
  guint64 difference_hash_2_y,
  guint64 importance_2_x,
  guint64 importance_2_y)
{
  const guint64 
    d1_x = difference_hash_1_x,
    d1_y = difference_hash_1_y,
    i1_x = importance_1_x,
    i1_y = importance_1_y,
    d2_x = difference_hash_2_x,
    d2_y = difference_hash_2_y,
    i2_x = importance_2_x,
    i2_y = importance_2_y;

  return bit_array_sum((d1_x ^ d2_x) & (i1_x | i2_x)) 
    + bit_array_sum((d1_y ^ d2_y) & (i1_y | i2_y));

/*******************************************************************
 * Classic DHash would look like this instead:                     *
 *                                                                 *
 * return bit_array_sum(d1_x ^ d2_x) + bit_array_sum(d1_y ^ d2_y); *
 *                                                                 *  
 *******************************************************************/
}

static inline guint idhash_dist(idhash_result res_1, idhash_result res_2) {
  return idhash_distance(res_1.dx, res_1.dy, res_1.ix, res_1.iy,
    res_2.dx, res_2.dy, res_2.ix, res_2.iy);
}

static inline void idhash_print_result(idhash_result res) {
  printf("%" G_GUINT64_FORMAT " %" G_GUINT64_FORMAT 
    " %" G_GUINT64_FORMAT " %" G_GUINT64_FORMAT "\n",
    res.dx, res.dy,
    res.ix, res.iy);
}

static inline void histogram_print_hash(histogram hist) {
  for (int x=0; x<8; ++x) {
    for (int y=0; y<8; ++y) {
      const int index = x + 8*y;
      const int bit = bit_array_get(hist.hash, index);
      printf("%i%*c", bit, 1, ' ');
    }
    putchar('\n');
  }
}

static inline void histogram_print_importance(histogram hist) {
  for (int x=0; x<8; ++x) {
    for (int y=0; y<8; ++y) {
      const int index = x + 8*y;
      const int bit = bit_array_get(hist.importance, index);
      printf("%i%*c", bit, 1, ' ');
    }
    putchar('\n');
  }
}

void idhash_pixels(PixelRGB *pixels, int width, int height, idhash_result* res)
{ 
  if (width != 8 || height != 8) 
    vips_error_exit("Input pixel array should be 8x8 but is %ix%i instead.",
      width, height);

  /* Compute the bit arrays (represented as 64-bit integers) for the difference
   * hash and the associated importance array, for both the x- and y-direction.
   */
  histogram hist_x = {0};
  histogram_thread_arg arg_x = {&hist_x, pixels};
  pthread_t thread_x = {0};
  pthread_create(&thread_x, NULL, histogram_thread_x, &arg_x);

  histogram hist_y = {0};
  histogram_thread_arg arg_y = {&hist_y, pixels};
  pthread_t thread_y = {0};
  pthread_create(&thread_y, NULL, histogram_thread_y, &arg_y);

  /* when both threads are done, print the difference hashes and importances
   * in this format: <dx> <dy> <ix> <iy>
   */
  pthread_join(thread_x, NULL);
  pthread_join(thread_y, NULL);

  const idhash_result buf = {
    hist_x.hash, 
    hist_y.hash,
    hist_x.importance,
    hist_y.importance,
  };

  memcpy(res, &buf, 4*sizeof(guint64));

  //histogram_print_xy(hist_x, hist_y);
  //putchar('\n');

  //histogram_print_hash(hist_x);
  //putchar('\n');

  //histogram_print_hash(hist_y);
  //putchar('\n');

  //histogram_print_importance(hist_x);
  //putchar('\n');

  //histogram_print_importance(hist_y);
  //putchar('\n');
}

void idhash_filepath(char* filepath, idhash_result* res) {
    VipsImage *in;
    PixelRGB *pixels;
    VipsImage *out;

  /* Open the file, scaling down to a 8x8 image for IDHash. Note that
   * JPEG shrink-on-load is used by vipsthumbnail if the source is a
   * JPEG. In my near-duplicate detection test, this has actually 
   * resulted in better matches than linear shrinking with vipsthumbnail.
   */
  const int width = 8;
  if (vips_thumbnail(filepath, &in, width, 
    "height", width, 
    "size", VIPS_SIZE_FORCE, 
    NULL))
    vips_error_exit(NULL);

  /* Convert to 8-bit RGB grayscale, dropping the alpha channel, if any. 
   */
  if (vips_colourspace(in, &out, VIPS_INTERPRETATION_B_W, NULL))
    vips_error_exit(NULL);
  g_object_unref(in);
  in = out;

  /* Extract the red band - they should all be the same after grayscale. 
   */
  if(vips_extract_band(in, &out, 0, "n", 1, NULL))
    vips_error_exit(NULL);
  g_object_unref(in);
  in = out;

  /* Force image into memory.
   */ 
  if(vips_image_wio_input(in)) {
    vips_error_exit("In file difference_hash.c:  In function "
      "main(): Error writing image into memory.");
  }

  /* Get a pointer to an array of PixelRGB. Each PixelRGB is a length 3 array.
   */
  pixels = (PixelRGB *) VIPS_IMAGE_ADDR(in, 0, 0);

  /* Compute the IDHash of the PixelRGB array, copying the results to
   * the result object.
   */ 
  idhash_pixels(pixels, in->Xsize, in->Ysize, res);

  g_object_unref(in);
}   

