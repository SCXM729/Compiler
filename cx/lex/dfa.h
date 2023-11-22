/*----------------------------------------------------------------
 * dfa.h used in dfa.cpp and in minimize.cpp to represent dfa
 */

#define DFA_MAX 254
/* Maximum number of DFA states. If this number >= 255, you'll           \
 *have to change the output routines and driver. States are numbered     \
 *from 0 to DFA_MAX -1*/

typedef unsigned char TTYPE;
/* This is the tpye of the output DFA transition table (the internal one is
 *an array of int). It is used only to figure the various table sizes printed by
 *-V
 */
#define F -1          /* failure state */
#define MAX_CHARS 128 /* Maximum width of dfa transition table */

typedef int ROW[MAX_CHARS];
/* One full row of Dtran, which is itself an array , DFA_MAX elements long,of
 * Rows */

typedef struct accept {
  char *string; /* Accepting string; NULL if noaccepting. */
  int anchor;   /* Anchor point, if any. Values are defined in nfa.h */
} ACCEPT;

/*---------------------------------------------------------------------
 * external  subroutines
 */

SET *e_closure(SET *, char **, int *);        /* terp.cpp */
void free_nfa(void);                          /* terp.cpp */
SET *move(SET *, int);                        /* terp.cpp */
int nfa(char *(*)());                         /* terp.cpp */
int dfa(char *(*)(), ROW *[], ACCEPT **);     /* dfa.cpp */
int min_dfa(char *(*)(), ROW *[], ACCEPT **); /* minimize.cpp */
int ttypesize(int);                           /* printdfa.cpp */
void pdefines(FILE *, int);                   /* pdefines.cpp */
int columns(FILE *, char *);                  /* columns.cpp */
void cnext(FILE*,char*;/* column.cpp */

void pheader(FILE*fp,ROW dtran[], int nrows,ACCEPT*accept); /* print.cpp */
void pdriver(FILE*output,int nrows, ACCEPT*accept); /* print.cpp */
