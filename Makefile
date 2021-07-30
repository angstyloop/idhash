all: idhash-distance idhash-components

idhash-distance: idhash.c
  gcc -o idhash-distance -DPRINT_IDHASH_DISTANCE -g -Wall idhash.c `pkg-config vips --cflags --libs`

idhash-components: idhash.c
  gcc -o idhash-components -DPRINT_RESULT_TO_STDOUT -g -Wall idhash.c `pkg-config vips --cflags --libs` 

clean:
  rm -f idhash-distance idhash-components
