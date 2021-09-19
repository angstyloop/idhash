/*
gcc block_symbol.c -o test-block-symbol -DTEST_BLOCK_SYMBOL -g -Wall
*/

#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SYMBOLS_FILE "/home/falkor/source/idhash/block_alphabet.txt"

int char_in_closed_range(char c, char l, char r){
  return l <= c && c <= r;
}

int block_symbol_exists(char c){
  return char_in_closed_range(c, 'A', 'Z') 
    || char_in_closed_range(c, '0', '9');
}

// Compute the offsets of the numerical and alphabetical sections.
#define BLOCK_SYMBOL_WIDTH 7
#define BLOCK_SYMBOL_HEIGHT 5
#define BLOCK_ENTRY_HEIGHT (1/*index line*/ + BLOCK_SYMBOL_HEIGHT)
#define HEADER_HEIGHT (1/*dim line*/ + BLOCK_SYMBOL_HEIGHT/*blank block*/)
#define OFFSET_TO_0_9 HEADER_HEIGHT
#define OFFSET_TO_A_Z (OFFSET_TO_0_9 + 10 * BLOCK_ENTRY_HEIGHT)

// Check a character to see if a block symbol exists for it. If one does
// not, exit with failure.
void check_block_symbol(char c){
  if(!block_symbol_exists(c)){
    fprintf(stderr, "No block symbol for %c exists in %s.", c, 
      BLOCK_SYMBOLS_FILE);
    exit(EXIT_FAILURE);
  }
}

// The block alphabet file follows a format, so the index of the block 
// symbol entry in the flat file database can be computed.
int block_symbol_index(char c){
  check_block_symbol(c);
  int v=0;
  if(char_in_closed_range(c, 'A', 'Z'))
    v = OFFSET_TO_A_Z + (c - 'A') * BLOCK_ENTRY_HEIGHT;
  else if(char_in_closed_range(c, '0', '9'))
    v = OFFSET_TO_0_9 + (c - '0') * BLOCK_ENTRY_HEIGHT;
  return v;
}

// TODO: reimplement with fseek
void skip_lines(FILE* fp, unsigned nlines){
  if(!nlines) return;
  char* line=0;
  size_t n=0;
  for(int i=0; i<nlines; ++i){
    getline(&line, &n, fp);
  }
  free(line);
}

// Print @nlines lines beginning at the line numbered @pos. The first line
// has @pos = 0;
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
  if(line) free(line);
}

#ifdef TEST_PRINT_LINES
int main(){
  FILE* in = fopen(BLOCK_SYMBOLS_FILE, "r");
  print_lines(in, stdout, 2, 10); //lines #3 through #12
  return EXIT_SUCCESS;
}
#endif

// Merge horizontally @nlines lines of two files, starting at line @pos.
void print_lines_2(
  FILE* fpin1,
  FILE* fpin2,
  FILE* fpout,
  unsigned pos,
  unsigned nlines)
{
  skip_lines(fpin1, pos);
  skip_lines(fpin2, pos);
  char* line1=0, * line2=0;
  size_t n1=0, n2=0;
  ssize_t z1=0, z2=0;
  unsigned i=0;
  while(i < nlines 
    && 0<(z1 = getline(&line1, &n1, fpin1))
    && 0<(z2 = getline(&line2, &n2, fpin2)))
  {
    // Write line from column 1, without newline.
    fwrite(line1, 1, '\n' == line1[z1-1] ? z1-1 : z1, fpout);
    // Write a space separating the two columns.
    fwrite(" ", 1, 1, fpout);
    // Write line from column 2, ensuring a newline after each compound line
    // in the output file, except for possibly the last line.
    fwrite(line2, 1, z2, fpout);
    if('\n' != line2[z2-1] && i != nlines-1) fwrite("\n", 1, 1, fpout);
    ++i;
  }
  if(line1) free(line1);
  if(line2) free(line2);
}

#ifdef TEST_PRINT_LINES_2
int main(){
  FILE* in1 = fopen(BLOCK_SYMBOLS_FILE, "r");
  FILE* in2 = fopen(BLOCK_SYMBOLS_FILE, "r");
  print_lines_2(in1, in2, stdout, 1, 5); // lines #2 through #6
  return EXIT_SUCCESS;
}
#endif

// Merge horizontally @nlines lines of files in null-terminated array 
// @files.
void print_lines_n(
  FILE** fpvin,
  FILE* fpout,
  unsigned pos,
  unsigned nlines
){
  char* line=0;
  size_t n=0;
  ssize_t z=0;
  FILE** fp=0;
  // Skip to target line with index @pos in the vector of file pointers.
  for(FILE** fp=fpvin; *fp; ++fp) skip_lines(*fp, pos);
  // Starting at the target line, loop over @nlines lines.
  for(int i=0; i<nlines; ++i){
    // Loop over all files.
    for(FILE** fp=fpvin; *fp; ++fp){
      // Get the line with index $i - this includes the terminating newline 
      // character if present.
      z = getline(&line, &n, *fp);
      // Write the line to the output file. avoid writing the newline 
      // character, but note that the last line may be reached, which may
      // or may not have a terminating newline character.
      fwrite(line, 1, '\n' == line[z-1] ? z-1 : z, fpout);
      // If $fp is not the last file pointer, write a separating space
      // character to the output file.
      if(*(fp+1)) fwrite(" ", 1, 1, fpout);
    }
    // If line $i does not end in a newline character already and is
    // not the last of the @nlines lines, write a terminating newline
    // character to the output file.
    if('\n' != line[z-1] && nlines-1 != i) fwrite("\n", 1, 1, fpout);
  }
  // Clean up.
  if(line) free(line);
}

#ifdef TEST_PRINT_LINES_N
int main(){
  FILE* in1 = fopen(BLOCK_SYMBOLS_FILE, "r");
  FILE* in2 = fopen(BLOCK_SYMBOLS_FILE, "r");
  print_lines_n(in1, in2, stdout, 1, 5);
  return EXIT_SUCCESS;
}
#endif

void block_symbol_print(FILE* out, char c){
  unsigned index = block_symbol_index(c);
  FILE* in = fopen(BLOCK_SYMBOLS_FILE, "r");
  if(!in){
    perror("block_symbol_print: Unable to open block-alphabet file.");
    exit(EXIT_FAILURE);
  } 
  skip_lines(in, 1);
  print_lines(in, out, index, BLOCK_SYMBOL_HEIGHT);
  fclose(in);
}

#define BLOCK_SYMBOL_DIR "/home/falkor/source/idhash/block_symbols"

// Create a new file for each block symbol in the block symbols file.
void split_block_symbol_file(){
  FILE* in = fopen(BLOCK_SYMBOLS_FILE, "r");
  if(!in){
    perror("split_block_symbol_file: Unable to open block-alphabet file.");
    exit(EXIT_FAILURE);
  }

  fclose(in);
}

#ifdef TEST_BLOCK_SYMBOL
int main(){
  block_symbol_print(stdout, 'A');
  return EXIT_SUCCESS;
}
#endif
