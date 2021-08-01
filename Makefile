all: idhash-distance idhash-components

idhash-distance: idhash.c bit_array.c histogram.c main.c
	gcc -o idhash-distance -DPRINT_IDHASH_DISTANCE -g -Wall idhash.c bit_array.c histogram.c main.c `pkg-config vips --cflags --libs`

idhash-components: idhash.c bit_array.c histogram.c main.c
	gcc -o idhash-components -DPRINT_RESULT_TO_STDOUT -g -Wall idhash.c bit_array.c histogram.c main.c `pkg-config vips --cflags --libs` 

test-bit-array: bit_array.c test_bit_array.c
	gcc -DTEST_BIT_ARRAY -o test-bit-array -g -Wall bit_array.c test_bit_array.c `pkg-config glib-2.0 --cflags --libs` && ./test-bit-array

test-histogram: bit_array.c histogram.c test_histogram.c
	gcc -DTEST_HISTOGRAM -o test-histogram -g -Wall bit_array.c histogram.c test_histogram.c `pkg-config glib-2.0 --cflags --libs` && ./test-histogram

clean:
	rm -f idhash-distance idhash-components
