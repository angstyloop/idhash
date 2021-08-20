#!/bin/bash
#gcc -g -Wall -DTEST_GENERATE_NONDUPLICATES generate_nonduplicates.c -o test-generate-nonduplicates -luuid > gcc.log 2>&1 
gcc -g -Wall generate_nonduplicates.c -o generate-nonduplicates -luuid > gcc.log 2>&1 

head -n10 gcc.log
