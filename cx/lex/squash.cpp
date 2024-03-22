#include "../tool/compiler.h"
#include "dfa.h"
#include <cstdlib>
#include <cstring>

/* This module contains the routines to compress a table horizontally and
 * vertically by removing*/

static int Col_map[MAX_CHARS];
static int Row_map[DFA_MAX];

#define NCOLS 16
#define TYPE "YY_TTYPE"
#define SCLASS "YYPRIVATE"

static int col_equiv(int *col1, int *col2, int nrows);
static void col_cpy(int *dest, int *src, int nrows, int n_src_cols,
                    int n_dest_cols);
static void reduce(ROW *dtran, int *p_nrows, int *p_ncols);
static void print_col_map(FILE *fp);
static void print_row_map(FILE *fp, int nrows);
static void pmap(FILE *fp, int *p, int n);

#define ROW_EQUIV(r1, r2, ncols) (memcmp(r1, r2, ncols * sizeof(int)) == 0)
#define ROW_CPY(r1, r2, ncols) (memcpy(r1, r2, ncols * sizeof(int)))

int squash(FILE *fp, ROW *dtran, int nrows, int ncols, const char *name) {
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

  /* dtran + row map +col map */
  return ((nrows * ncols * sizeof(TTYPE)) + (onrows * sizeof(TTYPE)) +
          oncols * sizeof(TTYPE));
}

static int col_equiv(int *col1, int *col2, int nrows) {
  /* Return 1 if the two columns are equivalent, else return 0 */

  while (--nrows >= 0 && *col1 == *col2) {
    col1 += MAX_CHARS; /* Advance to next cell in the column */
    col2 += MAX_CHARS;
  }
  return (!((nrows) >= 0));
}

static void col_cpy(int *dest, int *src, int nrows, int n_src_cols,
                    int n_dest_cols) {
  /* dest: Top of destination column
   * src: Top of source column
   * nrows: Number of rows
   * n_src_cols: Number of columns in source array
   * n_dest_cols: Number of columns in destination array
   */

  /* Copy a column from one array to another. Both arrays are nrows deep,
   * the source array is n_src_cols wide and the destination array is
   * n_dest_cols wide.
   */
  while (--nrows >= 0) {
    *dest = *src;
    dest += n_dest_cols;
    src += n_src_cols;
  }
}

static void reduce(ROW *dtran, int *p_nrows, int *p_ncols) {
  /* dtran: DFA transition table
   * p_nrows: # of states in dtran
   * p_ncols: Pointer to column count
   */

  /* Reduce dtran horizontally and vertically, filling the two map
   * arrays with character mapping to the final, reduced transition
   * table. Return the number of columns in the reduced dtran table.
   *
   * Col_map is the x (character) axis, Row_map is the y (next state) axis.
   */

  int ncols = *p_ncols; /* number of columns in original machine */
  int nrows = *p_nrows; /* number of rows in original machine */
  int r_ncols;          /* number of columns in reduced machine */
  int r_nrows;          /* number of rows in reduced machine */
  SET *save;            /* rows or columns that will remain in table */
  int *current;         /* first of serveral identical columns */
  int *compressed;      /* pointer to compressed array */
  int *p;
  int i, j;

  /* first do the columns */

  memset(Col_map, -1, sizeof(Col_map));
  save = newset();

  for (r_ncols = 0;; r_ncols++) {
    /* Skip past any states in the Col_map that have already been
     * processed. If the entire Col_map has been processed, break.
     */
    for (i = r_ncols; Col_map[i] != -1 && i < ncols; i++)
      ;

    if (i >= ncols)
      break;

    /* Add the current column to save set. It eventually ends up in the
     * reduced array as column "r_ncols" so modify the Col_map entry
     * according. Now, scan through the array looking for duplicates
     * of the current column (pointed to by current). If you find a
     * duplicate, make the assocaited Col_map entry also point to "r_ncols"
     */

    ADD(save, i);
    Col_map[i] = r_ncols;

    current = &dtran[0][i];
    p = current + 1;

    for (j = i; ++j < ncols; p++)
      if (Col_map[j] == -1 && col_equiv(current, p, nrows))
        Col_map[j] = r_ncols;
  }
  /* Compress the array horizontally by removing all of the columns that
   * aren't in the save set. We're doing this by moving all the columns
   * that are in the save set to the proper position in a newly allocated
   * array. You can't do it in place because there's no guarantee that
   * the equivalent rows are next to each other.
   */

  if (!(compressed = (int *)malloc(nrows * r_ncols * sizeof(int))))
    ferr("Out of memory");

  p = compressed;
  for (next_member(NULL); (i = next_member(save)) != -1;)
    col_cpy(p++, &dtran[0][i], nrows, ncols, r_ncols);
  /* Eliminate equivalent rows, working on the reduced array
   * created in the previous step. The algorihtm used is the same.
   */

  memset(Row_map, -1, sizeof(Row_map));
  CLEAR(save);

  for (r_nrows = 0;; r_nrows++) {
    for (i = r_nrows; Row_map[i] != -1 && i < nrows; i++)
      ;
    if (i >= nrows)
      break;

    ADD(save, i);
    Row_map[i] = r_nrows;

    current = compressed + i * r_ncols;
    p = compressed + (i + 1) * r_ncols;

    for (j = i; ++j < nrows; p += r_ncols)
      if (Row_map[j] == -1 && ROW_EQUIV(current, p, r_ncols))
        Row_map[j] = r_nrows;
  }
  /* Actually compress rows, copying back into the original array space.
   * Note that both dimensions of the array have been changed.
   */
  p = (int *)dtran;

  for (next_member(NULL); (i = next_member(save)) != -1;) {
    ROW_CPY(p, compressed + i * r_ncols, r_ncols);
    p += r_ncols;
  }

  delset(save);
  free(compressed);

  *p_ncols = r_ncols;
  *p_nrows = r_nrows;
}

static void print_col_map(FILE *fp) {
  static const char *text[] = {
      "The Yy_cmap[] and Yy_rmap arrays are used as follows:",
      "",
      " next_state= Yydtran[ Yy_rmap[current_state] ][ Yy_cmap[input_char] ];",
      "",
      "Character positions in the Yy_cmap array are:",
      "",
      "   ^@  ^A  ^B  ^C  ^D  ^E  ^F  ^G  ^H  ^I  ^J  ^K  ^L  ^M  ^N  ^O",
      "   ^P  ^Q  ^R  ^S  ^T  ^U  ^V  ^W  ^X  ^Y  ^Z  ^[  ^\\  ^]  ^^  ^_",
      "        !   \"   #   $   %   &   '   (   )   *   +   ,   -   .   /",
      "    0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?",
      "    @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O",
      "    P   Q   R   S   T   U   V   W   X   Y   Z   [   \\   ]   ^   _",
      "    `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o",
      "    p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~   DEL",
      NULL};

  comment(fp, text);
  fprintf(fp, "%s %s  Yy_cmap[%d] =\n{\n     ", SCLASS, TYPE, MAX_CHARS);
  pmap(fp, Col_map, MAX_CHARS);
}

static void print_row_map(FILE *fp, int nrows) {
  fprintf(fp, "%s %s  Yy_rmap[%d] =\n{\n     ", SCLASS, TYPE, nrows);
  pmap(fp, Row_map, nrows);
}

static void pmap(FILE *fp, int *p, int n) {
  /* fp: output stream
   * p: pointer to array
   * n: array size
   */
  /* Print a one-dimensional array.
   */

  int j;
  for (j = 0; j < (n - 1); j++) {
    fprintf(fp, "%3d,", *p++);

    if ((j % NCOLS) == NCOLS - 1)
      fprintf(fp, "\n     ");
  }

  fprintf(fp, "%3d\n};\n\n", *p);
}

void cnext(FILE *fp, const char *name) {
  /*   Print out a yy_next(state,c) subroutine for the compressed table.
   */

  static const char *text[] = {
      "yy_next(state,c) is given the current state number and input",
      "character and evaluates to the next state.", NULL};

  comment(fp, text);
  fprintf(fp, "#define yy_next(state,c) (%s[ Yy_rmap[state] ][ Yy_cmap[c] ])\n",
          name);
}
