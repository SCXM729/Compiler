#include "../tool/compiler.h"
#include "../tool/debug.h"
#include "../tool/set.h"
#include "dfa.h"
#include "globals.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include<cstring>

/* squash.cpp: This module contains the routine to compress a table
 * horizontally and vertically by removing redundant columns and rows,
 * and then print the compressed table. I haven't been as careful about
 * making this routine general purpose because it's only useful to Lex.
 * The pairs compression routines in pairs.cpp are used to compress the
 * occs- and llama-generated tables so they're a little more compilcated.
 */

PRIVATE int Col_map[MAX_CHARS];
PRIVATE int Row_map[DFA_MAX];

#define NCOLS 16
#define TYPE "YY_TYPE"    /* Declared type of output tables */
#define CLASS "YYPRIVATE" /* Storage class of all the tables */

PRIVATE int col_equiv(int *, int *, int);
PRIVATE void col_cpy(int *, int *, int, int, int);
PRIVATE void reduce(ROW *, int *, int *);
PRIVATE void print_col_map(FILE *);
PRIVATE void print_row_map(FILE *, int);
void cnext(FILE *, int *, int);
void cnext(FILE *, char *);
int squash(FILE *, ROW *, int, int, char *);

#define ROW_EQUIV(r1, r2, ncols) (memcpy(r1, r2, ncols * sizeof(int)) == 0)
#define ROW_CPY(r1, r2, ncols) (memcpy(r1, r2, ncols * sizeof(int)))
