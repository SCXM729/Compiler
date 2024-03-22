#include "../tool/compiler.h" /* for prototypes only */
/*------------------------------------------------------------
 * print_ar.cpp general-purpose subroutine to print out a
 * 2-dimensional array.
 */

typedef int ATYPE;
#define NCOLS 10 /* Number of columns used to print arrays */

void print_array(FILE *fp, int *array, int nrows, int ncols) {
  /* Print the C source code to initialize the two-dimensional array
   * pointed to by "array". Print only the initialize part of the declaration
   */

  int i;
  int col; /* Output column */

  fprintf(fp, "{\n");
  for (i = 0; i < nrows; i++) {
    fprintf(fp, "/* %02d */ ( ", i);

    for (col = 0; col < ncols; col++) {
      fprintf(fp, "%3d", *array++);
      if (col < ncols - 1)
        fprintf(fp, ", ");

      if ((col % NCOLS) == NCOLS - 1 && col != ncols - 1)
        fprintf(fp, "\n    ");
    }

    if (col > NCOLS)
      fprintf(fp, "\n    ");

    fprintf(fp, " )%c\n", i < nrows - 1 ? ',' : ' ');
  }
  fprintf(fp, "};\n");
}
