/** @file error_exit.c
 *  @author Sean C. Allen (sean.christian.allen.ii@gmail.com)
 *  @date 20210927
 */

#include "error_exit.h"

void error_exit(char* message){
  if(message) perror("message");
  exit(EXIT_FAILURE);
}

/* Test
gcc -DTEST_ERROR_EXIT_A -g -Wall -o test-error-exit-a error_exit.c && gcc -DTEST_ERROR_EXIT_B -g -Wall -o test-error-exit-b error_exit.c && ./test-error-exit-a && ./test-error-exit-b
*/

#ifdef TEST_ERROR_EXIT_A
#include <assert.h>
int main(){
  // Accepts 0 (or NULL).
  error_exit(0);
  assert(0);
}
#endif

#ifdef TEST_ERROR_EXIT_B
#include <assert.h>
int main(){
  // Accepts a string message.
  error_exit("No error, won't be printed.");
  assert(0);
}
#endif
