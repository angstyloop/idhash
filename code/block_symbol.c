/*
gcc block_symbol.c -o test-block-symbol -DTEST_BLOCK_SYMBOL -g -Wall
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_SYMBOLS_FILE "/home/falkor/src/idhash/block_alphabet.txt"

// Return true iff c is between l and r, inclusive.
int char_in_closed_range(char c, char l, char r){
  return l <= c && c <= r;
}

// Return true iff the block symbol is in the database. Shortcut just checks
// ranges.
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
// Skip @nlines lines in the file at @fp (advancing the seek position in the 
// file)
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
    // If line $i not the last of the @nlines lines, write a terminating 
    // newline character to the output file.
    if(nlines-1 != i) fwrite("\n", 1, 1, fpout);
  }
  // Clean up.
  if(line) free(line);
}

#ifdef TEST_PRINT_LINES_N
int main(){
  FILE* in1 = fopen(BLOCK_SYMBOLS_FILE, "r");
  FILE* in2 = fopen(BLOCK_SYMBOLS_FILE, "r");
  FILE* in3 = fopen(BLOCK_SYMBOLS_FILE, "r");
  FILE* fpv[] = {in1, in2, in3, 0};
  print_lines_n(fpv, stdout, 1, 5);
  fclose(in1);
  fclose(in2);
  fclose(in3);
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

#define BLOCK_SYMBOL_DIR "/home/falkor/src/idhash/block_symbols"
#define PATH_SEP "/"

// Create a new file in directory @dname for each block symbol in the block 
// symbol file @fname.
void split_block_symbols_file(char* fname, char* dname){
  FILE* fp = fopen(fname, "r");
  if(!fp){
    perror("split_block_symbols_file: Unable to open block-symbols file.");
    exit(EXIT_FAILURE);
  }
  // go to line for '0' and start there
  skip_lines(fp, OFFSET_TO_0_9);
  // loop through lines of block symbol file
  char* line= calloc(0, 1);
  size_t n=0;
  ssize_t z=0;
  int done=0;
  for(;;){
    // get the name from the first line of the block. break if EOF or error.
    if(1 > (z = getline(&line, &n, fp))) break;
    size_t szpath = strlen(dname)/*directory name*/ 
      + strlen(PATH_SEP)/*slash*/
      + ('\n' == line[z-1] ? z-1 : z)/*filename without newline*/ 
      + 1/*nullchar*/;
    char* path = calloc(szpath, 1);
    // note the newline char will not be written thanks to our choice of n, and 
    // that last remaining empty byte will be zeroed thanks to calloc
    snprintf(path, szpath, "%s%s%s", dname, PATH_SEP, line);
    // create a new file with that name.
    FILE* nfp = fopen(path, "w");
    free(path);
    // write the next BLOCK_SYMBOL_HEIGHT lines to the new file
    for(int i=0; i<BLOCK_SYMBOL_HEIGHT; ++i){
      if(1 > (z = getline(&line, &n, fp))){
        done=1;
        break;
      }
      fwrite(line, 1, z, nfp);
    }
    fclose(nfp);
    if(done) break;
  }

  if(line) free(line);
  fclose(fp);
}

#ifdef TEST_SPLIT_BLOCK_SYMBOLS_FILE
int main(){
  split_block_symbols_file(BLOCK_SYMBOLS_FILE, BLOCK_SYMBOL_DIR);
  return EXIT_SUCCESS;
}
#endif

// returns 0 if a string is all caps A-Z and 0-9, and 1 otherwise. @text is a 
// null-terminated string
int validate_text(char* text){
  for(char* p=text; *p; ++p){
    if(!block_symbol_exists(*p))
      return 0;
  }
  return 1;
}

// given a char @c returns the path to the file containing the block letter
// representation of that char.
char* char_to_block_letter_path(char c){
  if(!block_symbol_exists(c)){
    fprintf(stdout, "Block symbol for char '%c' doesn't exist.\n", c);
    exit(EXIT_FAILURE);
  }
  size_t n = strlen(BLOCK_SYMBOLS_DIR) + strlen(PATH_SEPARATOR) 
    + 1/*char*/ + 1/*nullbyte*/;
  char* path = calloc(n, 1);
  sprintf(path, n, "%s%s%c", BLOCK_SYMBOLS_DIR, PATH_SEPARATOR, c);
  return path;
}

// print the block letter version of a word.
void blockify_word(char* text){
  FILE* fp=0;
  // loop over chars
  for(char* p=text; *p; ++p){
    // get path to block letter file. error if DNE.
    char* path = char_to_block_letter_path(*p)
    // open block letter file
    if(!(fp = fopen(path, "rb"))){
      perror("fopen");
      exit(EXIT_FAILURE);
    }
    // write entire file contents to STDOUT
    size_t n=0;
    char* buf = calloc(SIZE_MAX, 1);
    while((n = fread(buf, 1, szbuf, fp)) && fwrite(buf, 1, szbuf, stdout));
    // clean up
    free(buf);
    free(path);
    fclose(fp);
  }
}

#ifdef test_blockify_word
int main(){
  blockify_word("POOP");
  return EXIT_SUCCESS;
}
#endif

#ifdef TEST_BLOCK_SYMBOL
int main(){
  block_symbol_print(stdout, 'A');
  return EXIT_SUCCESS;
}
#endif
