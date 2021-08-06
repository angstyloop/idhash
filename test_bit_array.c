/* 
 * test_bit_array.c 
 */

#include <assert.h>

#include "bit_array.h"

void test_bit_array_get(){
  guint64 bits = 5; // <61 zeroes>101
  guint k = 1;
  guint result = bit_array_get(bits, k);
  assert(result == 0);
}

void test_bit_array_set(){
  guint64 bits = 5;
  guint k = 1;
  bit_array_set(&bits, k);
  guint result = bit_array_get(bits, k);
  assert(result == 1);
}

void test_bit_array_unset(){
  guint64 bits = 5;
  guint k = 1;
  bit_array_set(&bits, k);
  bit_array_unset(&bits, k);
  guint result = bit_array_get(bits, k);
  assert(result == 0);
} 

void test_bit_array_print_matrix() {
  bit_array_print_matrix(G_MAXUINT64);//should be 64 ones. confirm w/eyeballs.
}

void test_bit_array() {
  test_bit_array_get();
  test_bit_array_set();
  test_bit_array_unset();
  test_bit_array_print_matrix();
}

#ifdef TEST_BIT_ARRAY
int main() {
  test_bit_array();
  return EXIT_SUCCESS;
}
#endif
