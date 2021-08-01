#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef GUARD_GLIB
#define GUARD_GLIB
#include <glib-2.0/glib.h>
#endif

#include "bit_array.c"

int test_bit_array_get(){
  guint64 bits = 5; // <61 zeroes>101
  guint k = 1;
  guint result = bit_array_get(bits, k);
  return result == 0;
}

int test_bit_array_set(){
  guint64 bits = 5;
  guint k = 1;
  bit_array_set(&bits, k);
  guint result = bit_array_get(bits, k);
  return result == 1;
}

int test_bit_array_unset(){
  guint64 bits = 5;
  guint k = 1;
  bit_array_set(&bits, k);
  bit_array_unset(&bits, k);
  guint result = bit_array_get(bits, k);
  return result == 0;
} 

void test_bit_array() {
  assert(test_bit_array_get());
  assert(test_bit_array_set());
  assert(test_bit_array_unset());
}

#ifdef TEST_BIT_ARRAY
int main() {
  test_bit_array();
  return EXIT_SUCCESS;
}
#endif
