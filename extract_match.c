/* extract_match.c

gcc extract_match.c -DTEST_EXTRACT_MATCH -o extract-match -Wall -g

Extract the first match of @regex in @source. If non-null, the returned pointer 
char* must be freed by the caller, and points to a null terminated string. If 
there is no match. */

#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <assert.h>

char* extract_match(char* source, char* pattern){
  regex_t regex;
  regmatch_t groups[1];
  if(regcomp(&regex, pattern, REG_EXTENDED)  // nonzero if compile fails
    || regexec(&regex, source, 1, groups, 0) // nonzero if no match
    || -1==groups[0].rm_so) // regmatch data are -1 if groups depleted
    return 0;
  const regoff_t n = groups[0].rm_eo - groups[0].rm_so; //match length
  char* match_substr = calloc(n+1, 1);
  strncpy(match_substr, source + groups[0].rm_so, n);
  match_substr[n]=0; // terminating null byte
  return match_substr;
}

#ifdef TEST_EXTRACT_MATCH 
int main(){
  char* source = "__ abc123 __";
  char* pattern = "[1-3]+._";
  char* match = extract_match(source, pattern);
  char* expect = "123 _";
  assert(!strcmp(match, expect));
  if(match) free(match);
  return EXIT_SUCCESS;
}
#endif
