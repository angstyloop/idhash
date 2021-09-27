/// @file console.h

#ifndef ERROR_EXIT_H
#  define ERROR_EXIT_H

#  ifndef STDLIB_H
#    define STDLIB_H
#    include <stdlib.h>
#  endif

#  ifndef STDIO_H
#    define STDIO_H
#    include <stdio.h>
#  endif

#  ifndef ERRNO_H
#    define ERRNO_H
#    include <errno.h>
#  endif

/** @brief  Print a message and exit.
 *
 *  Print an optional message with perror and then exit. If message is 0 or null, exit without
 *  printing a message.
 * 
 *  @param message
 */
void error_exit(char* message);

#endif
