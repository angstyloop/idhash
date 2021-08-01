#ifndef GUARD_GLIB
#define GUARD_GLIB
#include <glib-2.0/glib.h>
#endif

/* Use these inline functions to manipulate the bit arrays, represented as
 * 64-bit integers.
 */

/* Get the value of the kth bit from a bit array b.
 */
static inline guint bit_array_get(guint64 b, guint k) {
  //assert(k >= 0);
  //assert(k < 64);
  return b & ((guint64)1 << (k % 64)) ? 1 : 0;
}

/* Set the kth bit in a bit array b.
 */
static inline void bit_array_set(guint64* b, guint k) {
  *b |= (guint64)1 << (k % 64);
}

/* Not used, but here for completeness.
 */

static inline void bit_array_unset(guint64* b, guint k) {
  *b &= ~((guint64)1 << (k % 64));
}

/* Sum up all the 1 bits in a guint64, left-shifting to iterate over the bits.
 */
static inline int bit_array_sum(guint64 b) {
  int count = 0;
  while (b) {
    count += b & 1;
    b >>= 1;
  }
  return count;
}

