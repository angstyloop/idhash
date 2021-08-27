/* bit_array.h: 
 *
 * This file contains code for manipulating 64-bit integers viewed as 
 * arrays of bits. They are used as the bins of the histogram object
 * defined in histogram.c, as well as for the importance array and
 * and distance hash. 
 */

#ifndef GLIB_H
#define GLIB_H
#include <glib-2.0/glib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

/* Use these functions to manipulate the bit arrays, represented 
 * as 64-bit integers.
 */

/* Get the value of the kth bit from a bit array b.
 */
guint bit_array_get(guint64 z, guint k) {
  return z & ((guint64)1 << (k % 64)) ? 1 : 0;
}

/* Set the kth bit in a bit array b (i.e. set value to 1).
 */
void bit_array_set(guint64* z, guint k) {
  *z |= (guint64)1 << (k % 64);
}

/* Unset the kth bit in a bit array b (i.e. set value to 0).
 */
void bit_array_unset(guint64* z, guint k) {
  *z &= ~((guint64)1 << (k % 64));
}

/* Sum up all the 1 bits in a guint64, left-shifting to iterate over the bits.
 */
int bit_array_sum(guint64 z) {
  int count = 0;
  while(z){
    count += z & 1;
    z >>= 1;
  }
  return count;
}

/* Print the bit array as a square matrix of 1's and 0's.
 */
void bit_array_print_matrix(guint64 z){
  int d=0;//offset
  char s[129]={0};// 64 '1' or '0' + 56 ' ' + 8 '\n'  + 1 '\0' = 129 chars
  for(int i=0; i<8; ++i){
    for(int j=0; j<7; ++j){// iterate over first 7 bits in row
      s[j+8*i+d] = bit_array_get(z, j+8*i+d)?'1':'0';
      s[j+8*i+(++d)]=' ';//pr-incr offset & add newline after bit
    }
    s[7+8*i+d] = bit_array_get(z, 7+8*i+d)?'1':'0';//set last bit in row
    s[7+8*i+(++d)]='\n';//pre-incr offset & add newline after last bit in row
  } 
  s[128] = '\0';
  printf("%s", s);//print all at once, instead of bit by bit
}
