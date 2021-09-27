/** @file roc_source.h
 *  @author Sean C. Allen (sean.christian.allen.ii@gmail.com)
 *  @date 20210927
 *
 *  @brief Contains filenames and file pointers for the duplicate and 
 *  non-duplicate data files used to generate the ROC curve.
 *
 *  This struct is a convenient handle for the names and pointers to the
 *  files that contain the data used to build the ROC curve.
 *
 *  The ROC curve is a graphical representation of the predictive power of a 
 *  classifier. It shows how much of an improvement the classifier is over a
 *  random one (a coin flip). It was first used in the 1940s to quantify the 
 *  performance of radar operators (who had to classify pings as friend or 
 *  enemy).
 *
 *  The area under the curve (AUC), a value in the interval [0,1], is used
 *  to quantify the predictive power of a classifier. In medicine, for the
 *  classification of abnormalities in medical images as tumor or non-tumor,
 *  an AUC of .9 is considered good enough for practice.
 */


#ifndef ROC_SOURCE_H
#  define ROC_SOURCE_H

#  ifndef STDLIB_H
#    define STDLIB_H
#    include <stdlib.h>
#  endif

#  ifndef STDIO_H
#    define STDIO_H
#    include <stdio.h>
#  endif

#  ifndef IDHASH_STATS_H
#    define IDHASH_STATS_H
#    include "idhash_stats.c"
#  endif

#  ifndef DEFAULT_DUP_FNAME
#    define DEFAULT_DUP_FNAME "/home/falkor/idhash/duplicates.dat"
#  endif

#  ifndef DEFAULT_NONDUP_FNAME
#    define DEFAULT_NONDUP_FNAME "/home/falkor/idhash/non-duplicates.dat"
#  endif

typedef struct roc_source roc_source;
/** 
 *  @struct roc_source
 *  @brief A struct holding a pair of FILE pointers and their names. 
 *  @var roc_source::dupname duplicates data file name 
 *  @var roc_source::nondupname non-duplicates data file name 
 *  @var roc_source::fp_dup duplicates data file pointer
 *  @var roc_source::fp_nondup non-duplicates data file pointer
 */
struct roc_source {
  char* dupname;
  char* nondupname;
  FILE* fp_dup;
  FILE* fp_nondup;
};

/** @brief Create a new empty roc_source.
 *  @return roc_source*
 */
roc_source* roc_source_create();

/** @brief Initialize an roc_source.
 *  @return roc_source*
 */
roc_source* roc_source_init(roc_source* source, char* dupname, char* nondupname);

/** @brief Destroy an roc_source, freeing its memory.
 *  @return roc_source*
 */
roc_source* roc_source_destroy(roc_source* source);

/** @brief Reset an roc_source's file pointers.
 *  @return roc_source*
 */
roc_source* roc_source_reset_fp(roc_source* source);

/** @brief Print a textual representation of roc_source to stdout.
 *  @return roc_source*
 */
roc_source* roc_source_print(roc_source* psource);

#endif
