#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bit_array.c"

typedef struct null_arg null_arg;
struct null_arg {};

void test_bit_array_get(null_arg* _arg){
  guint64 bits = 5; // <61 zeroes>101
  guint k = 1;
  guint result = bit_array_get(bits, k);
  assert(result == 0);
}

void test_bit_array_set(null_arg* _arg){
  guint64 bits = 5;
  guint k = 1;
  bit_array_set(bits, k);
  guint result = bit_array_get(bits, k);
  assert(result == 1);
}

void test_bit_array_unset(null_arg* _arg){
  guint64 bits = 5;
  guint k = 1;
  bit_array_set(bits, k);
  bit_array_unset(bits, k);
  guint result = bit_array_get(bits, k);
  assert(result == 0);
} 

