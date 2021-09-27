/*
gcc meter.c -DTEST_METER -o test-meter -g -Wall

A toy example for a progress bar.
*/

#ifndef STDLIB_H
#  define STDLIB_H
#  include <stdlib.h>
#endif

#ifndef STDIO_H
#  define STDIO_H
#  include <stdio.h>
#endif

#ifndef STRING_H
#  define STRING_H
#  include <string.h>
#endif

#ifndef UNISTD_H
#  define UNISTD_H
#  include <unistd.h>
#endif

#include <fcntl.h>

// Call a function @f with signature @n times. Return the last result.
void* run(void* (*f)(void*), void* a, unsigned n){
  void* y;  
  for(unsigned i=0; i<n; ++i) y=f(a);
  return y;
}

void error(char* e){
  perror(e);
  exit(EXIT_FAILURE);
}

void write_string_to_stdout(char* str){
  int z=0, n=0;
  if(!(n = strlen(str))) error("write_string_to_stdout: zero length string.");
  if(1>(z = write(STDOUT_FILENO, str, n))) error("write");
}

void add_star(){
  write_string_to_stdout("*");
}

void remove_star(){
  write_string_to_stdout("\b \b");
}

void sleep_one_second(){
  sleep(1);
}

void* add_star_and_wait_1_sec(void* a){
  add_star();
  sleep_one_second();
  return 0;
}

void* remove_star_and_wait_1_sec(void* a){
  remove_star();
  sleep_one_second();
  return 0;
}

#ifdef TEST_METER
int main(){
  run(add_star_and_wait_1_sec, 0, 5);
  run(remove_star_and_wait_1_sec, 0, 5);
}
#endif
