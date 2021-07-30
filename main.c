#ifdef PRINT_RESULT_TO_STDOUT
int main(int argc, char **argv) {
  if (VIPS_INIT(argv[0]))
    vips_error_exit(NULL);

  /* Compute the IDHash of the image at the given filepath (@argv[1]), and save
   * it to @result.
   */
  idhash_result res = {0};
  idhash_filepath(argv[1], &res);

  /* Print the result, formatted like this: 
   * <dhash_x> <dhash_y> <importance_x> <importance_y>
   */
  idhash_print_result(res);

  return EXIT_SUCCESS;
}
#endif

#ifdef PRINT_IDHASH_DISTANCE
int main (int argc, char **argv) {
  if (VIPS_INIT(argv[0]))
    vips_error_exit(NULL);
  char* filepath_1 = argv[1];
  char* filepath_2 = argv[2];
  idhash_result res_1 = {0};
  idhash_result res_2 = {0};
  idhash_filepath(filepath_1, &res_1);
  idhash_filepath(filepath_2, &res_2);
  const guint d = idhash_dist(res_1, res_2);
  printf("%i", d);
}
#endif

