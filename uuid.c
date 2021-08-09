/* uuid.c
 * 
 * Defines function uuid
 *
 * Print a universally unique identifer, created using Linux uuid_generate.
 *
 * 
 * Compile
 *
 * gcc uuid.c -o uuid -luuid -Wall -g
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

#include <stdlib.h>
#include <stdio.h>
#include <uuid/uuid.h>

char* uuid(char out[UUID_STR_LEN]){
  uuid_t b;
  uuid_generate(b);
  uuid_unparse_lower(b, out);
  return out;
}

int main(){
  char out[UUID_STR_LEN]={0};
  puts(uuid(out));
  return EXIT_SUCCESS;
}
