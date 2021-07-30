gcc -DPRINT_IDHASH_DISTANCE -g -Wall idhash.c -o idhash `pkg-config vips --cflags --libs`

# other compile options
#PRINT_RESULT_TO_STDOUT
