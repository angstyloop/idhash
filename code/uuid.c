/* uuid.c
 * 
 * Defines function uuid
 *
 * Print a universally unique identifer, created using Linux uuid_generate.
 *
 * 
 * Compile
 *
 * gcc -DTEST_UUID uuid.c -o uuid -luuid -Wall -g
 *
 *
 * Run
 * 
 * ./uuid
 * 
 *
 * Debug
 *
 * gdb uuid
 * b main
 * r
 *
 */

#ifndef GUARD_STDLIB
#  define GUARD_STDLIB
#  include <stdlib.h>
#endif

#ifndef GUARD_STDIO
#define GUARD_STDIO
#include <stdio.h>
#endif

#ifndef GUARD_UUID
#  define GUARD_UUID
#  include <uuid/uuid.h>
#endif

char* gen_uuid_str(char out[UUID_STR_LEN]){
  uuid_t b;
  uuid_generate(b);
  uuid_unparse_lower(b, out);
  return out;
}

#ifdef TEST_UUID
int main(){
  char out[UUID_STR_LEN]={0};
  puts(uuid(out));
  return EXIT_SUCCESS;
}
#endif
