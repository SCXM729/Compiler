#include "squash.h"

PUBLIC int squash(FILE *fp, ROW *dtran, int nrows, int ncols, char *name) {
  /* Compress (and output) dtran using equivalent-column elimination.
   * Return the number of bytes required for the compressed tables
   * (including the map but not the accepting array)
   */

  int oncols = ncols; /* original column count */
  int onrows = nrows; /* original row count */

  reduce(dtran, &nrows, &ncols); /* Compress the tables */

  print_col_map(fp);
  print_row_map(fp, onrows);

  fprintf(fp, "%s %s %s[ %d ][ %d ]=\n", SCLASS, TYPE, name, nrows, ncols);
  print_array(fp, (int *)dtran, nrows, ncols);

  return ((nrows * ncols * sizeof(TTYPE)) + (onrows * sizeof(TTYPE)) +
          oncols * sizeof(TTYPE));
}

PRIVATE int col_equiv(int *col1, int *col2, int nrows) {
  /* Return 1 if the two columns are equivalent, else return 0 */

  while (--nrows >= 0 && *col1 == *col2) {
    col1 += MAX_CHARS; /* Advance to next cell in the column */
    col2 += MAX_CHARS;
  }
  return (!((nrows) >= 0));
}

PRIVATE void col_cpy(int*dest,int*src,int nrows,int n_src_cols,int n_dest_cols){
/* dest: Top of destination column; src: Top of source column
  * nrows: Number of rows
  * n_src_cols: Number of columns in source array
  * n_dest_cols: Number of columns in destination array
  */

  /* Copy a column from one array to another. Both arrays are nrows deep,
  * the source array is n_src_cols wide and the destination array is
  * n_dest_cols wide.
  */
  while(--nrows>=0){
    *dest=*src;
    dest+=n_dest_cols;
    src+=n_src_cols;
  }
 }
