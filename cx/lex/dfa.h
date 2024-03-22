#pragma once
#include "../tool/set.h"
#include <cstdio>
/*----------------------------------------------------------------
 * dfa.h used in dfa.cpp and in minimize.cpp to represent dfa
 */

#define DFA_MAX 254
/* Maximum number of DFA states. If this number >= 255, you'll have to change
 * the output routines and driver. States are numbered from 0 to DFA_MAX -1
 */

typedef unsigned char TTYPE;
/* This is the tpye of the output DFA transition table (the internal one is
 * an array of int). It is used only to figure the various table sizes printed
 * by -V
 */
#define F -1          /* Marks failure state in the tables */
#define MAX_CHARS 128 /* Maximum width of dfa transition table */

/* One full row of Dtran, which is itself an array , DFA_MAX elements long,of
 * Rows. Declaring a row as a distinct type helps you use pointers to traverse
 * individual columns. because incrementing a pointer to an entire row will skip
 * as many array elements as are in the row, effectively moving you down to the
 * next element of the current column
 */
typedef int ROW[MAX_CHARS];

/* Accepting string; NULL if noaccepting. */
/* Anchor point, if any. Values are defined in nfa.h */
typedef struct accept {
  char *string;
  int anchor;
} ACCEPT;

/*---------------------------------------------------------------------
 * external  subroutines
 */

SET *e_closure(SET *, char **, int *);    /* terp.cpp */
void free_nfa(void);                      /* terp.cpp */
SET *move(SET *, int);                    /* terp.cpp */
int nfa(char *(*input_routine)());        /* terp.cpp */
int dfa(char *(*)(), ROW *[], ACCEPT **); /* dfa.cpp */
int min_dfa(char *(*ifunct)(), ROW *(dfap[]),
            ACCEPT *(*acceptp)); /* minimize.cpp */
void pdefines(FILE *, int);      /* pdefines.cpp */
int columns(FILE *, char *);     /* columns.cpp */

void pheader(FILE *fp, ROW dtran[], int nrows, ACCEPT *accept); /* print.cpp */
void pdriver(FILE *output, int nrows, ACCEPT *accept);          /* print.cpp */

/* in input.cpp */
char *get_expr();
void lerror(int status, const char *fmt, ...);

/* in squash.cpp */
int squash(FILE *fp, ROW *dtran, int rows, int ncols, const char *name);
void cnext(FILE *fp, const char *name);

/* in sigon.cpp */
void signon();
