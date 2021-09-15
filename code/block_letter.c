/*
gcc block_letter.c -o block-letter -DTEST_BLOCK_LETTER -g -Wall
*/

#include <stdio.h>
#include <stdlib.h>

#define BLOCK_ALPHABET_FILE "/home/falkor/source/idhash/block_alphabet.txt"

int char_in_closed_range(char c, char l, char r) {
  return l <= c && c <= r;
}

int block_letter_exists(char c){
  return char_in_closed_range(c, 'A', 'Z') 
    || char_in_closed_range(c, '0', '9');
}

#define BLOCK_LETTER_WIDTH 7
#define BLOCK_LETTER_HEIGHT 5
#define BLOCK_ENTRY_HEIGHT (1/*index line*/ + BLOCK_LETTER_HEIGHT)
#define HEADER_HEIGHT (1/*WIDTHxHEIGHT*/ + BLOCK_LETTER_HEIGHT/*blank block*/)
#define OFFSET_TO_0_9 HEADER_HEIGHT
#define OFFSET_TO_A_Z (OFFSET_TO_0_9 + 10 * BLOCK_ENTRY_HEIGHT)


void check_block_letter(char c){
  if(!block_letter_exists(c)){
    fprintf(stderr, "No block letter for %c exists in %s.", c, 
      BLOCK_ALPHABET_FILE);
    exit(EXIT_FAILURE);
  }
}

// The block alphabet file follows a format, so the line number (the @index) 
// of the block letter entry can be computed.
int block_letter_index(char c){
  check_block_letter(c);
  int v=0;
  if(char_in_closed_range(c, 'A', 'Z'))
    v = OFFSET_TO_A_Z + (c - 'A') * BLOCK_ENTRY_HEIGHT;
  else if(char_in_closed_range(c, '0', '9'))
    v = OFFSET_TO_0_9 + (c - '0') * BLOCK_ENTRY_HEIGHT;
  return v;
}

void skip_lines(FILE* fp, unsigned nlines){
  if(!nlines) return;
  char* line=0;
  size_t n=0;
  for(int i=0; i<nlines; ++i){
    getline(&line, &n, fp);
  }
  free(line);
}

// Print @n lines beginning at the @pos. The first line has @pos = 0;
void print_lines(FILE* fp_in, FILE* fp_out, unsigned pos, unsigned nlines){
  skip_lines(fp_in, pos);
  char* line=0;
  size_t n=0;
  ssize_t z=0;
  unsigned i=0;
  while(i < nlines && 0<(z = getline(&line, &n, fp_in))){
    fwrite(line, 1, z, fp_out);
    ++i;
  }
}

void block_letter_print(FILE* out, char c){
  unsigned index = block_letter_index(c);
  FILE* in = fopen(BLOCK_ALPHABET_FILE, "r");
  if(!in){
    perror("block_letter_print: Unable to open block-alphabet file");
    exit(EXIT_FAILURE);
  } 
  skip_lines(in, 1);
  print_lines(in, out, index, BLOCK_LETTER_HEIGHT);
  fclose(in);
}

#ifdef TEST_BLOCK_LETTER
int main(){
  block_letter_print(stdout, 'A');
  return EXIT_SUCCESS;
}
#endif
