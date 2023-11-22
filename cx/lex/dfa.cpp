#include <cstdio>
#ifdef MSDOS
#include <cstdlib>
#else
#include <malloc.h>
#endif
#include "../tool/debug.h"
#include "../tool/set.h"
#include "dfa.h"
#include "globals.h" /* externs for Verbose, etc. */

/*-------------------------------------------------------------------
 *   dfa.cpp make a dfa transition table from an NFA created with
 * Thompson's construction
 * ------------------------------------------------------------------
 *  Dtran is the deterministic transition table. It is indexed by state
 *  number along the major axis and by input character along the minor
 *  axis. Dstates is a list of deterministic states represented as sets
 *  of NFA states.
 *  Nstates is the number of valid entries in Dtran
 */

typedef struct dfa_state {
  unsigned group : 8; /* Group id, used by minimize() */
  unsigned mark : 1;  /* Mark used by make_dtran() */
  char *accept;       /* accept action if accept state */
  int anchor;         /* Anchor point if an accept state */
  SET *set;           /* Set of NFA states represented by this DFA state */
} DFA_STATE;

PRIVATE DFA_STATE *Dstates; /* DFA states table */

/*-----------------------------------------------------------------------*/

PRIVATE ROW *Dtran;             /* DFA transition table */
PRIVATE int Nstates;            /* Number of DFA states */
PRIVATE DFA_STATE *Last_marked; /* Most-recently marked DFA state in Dtran */

extern char *bin_to_ascii(int, int); /* in compiler.lib */

// Prototypes for subroutines in this file
PRIVATE DFA_STATE *get_unmarked(void);
PRIVATE int in_dstates(SET *);
PRIVATE void free_sets(void);
PRIVATE void make_dtran(int);
PUBLIC int dfa(char *(*)(), ROW *[], ACCEPT **);

int dfa(char *(*ifunct), ROW *(dfa[]), ACCEPT **acceptp) {
  /* Turns an NFA with the indicated start state (sstate) into a DFA and
   * returns the number of states in the DFA transition table. *dfap is
   * modified to point at that transition table and *acceptp is modified
   * to point at an array of accepting states (indexed by state number).
   * dfa() discards all the memory used for the initial NFA
   */

  ACCEPT *accept_states;
  int i;
  int start;

  start = nfa(ifunct); /* make the nfa */
  Nstates = 0;
  Dstates = (DFA_STATE *)calloc(DFA_MAX, sizeof(DFA_STATE));
  Dtran = (ROW *)calloc(DFA_MAX, sizeof(ROW));
  Last_marked = Dstates;

  if (Verbose)
    fputs("making DFA: ", stdout);

  if (!Dstates || !Dtran)
    ferr("Out of memory!");

  make_dtran(start); /* convert the NFA to a DFA */
  free_nfa(); /* Free the memory used for the nfa itself (but not the accept
                 strings) */

  Dtran = (ROW *)realloc(Dtran, Nstates * sizeof(ROW));
  accept_states = (ACCEPTS *)malloc(Nstates * sizeof(ACCEPT));

  if (!accept_states || !Dtran)
    ferr("Out of memory");

  for (i = Nstates; --i >= 0;) {
    accept_states[i].string = Dstates[i].accept;
    accept_states[i].anchor = Dstates[i].anchor;
  }
  free(Dstates);
  *dfap = Dtran;
  *acceptp = accept_states;

  if (Verbose) {
    printf("\n%d out of %d DFA states in initial machine.\n", Nstates, DFA_MAX);
    printf("%d bytes required for uncompressed tables\n\n",
           Nstates * MAX_CHARS * sizeof(TTYPE) + /* dtran */
               Nstates * sizeof(TTYPE));         /* accept */

    if (Verbose > 1) {
      printf("The un-minimized DFA looks like this:\n\n");
      pheader(stdout, Dtran, Nstates, accept_states);
    }
  }
  return Nstates;
}

int add_to_dstates(SET *NFA_set, char *accepting_string, int anchor) {
  int nextstate;

  if (Nstates > (DFA_MAX - 1))
    ferr("Too many DFA states");

  nextstate = Nstates++;
  Dstates[nextstate].set = NFA_set;
  Dstates[nextstate].accept = accepting_string;
  Dstates[nextstate].anchor = anchor;

  return nextstate;
}

PRIAVTE int in_dstates(SET *NFA_set) {
  /* If there's a set in Dstates that is identical to NFA_set,return the
   * index of the Dstate entry, else return -1.
   */

  DFA_STATE *p;

  for (p = &Dstates[Nstates]; --p >= Dstates;)
    if (IS_EQUIVALENT(NFA_set, p->set))
      return (p - Dstates);

  return -1;
}

PRIVATE DFA_STATE *get_unmarked() {
  /* Return a pointer to an unmarked state in Dstates, If no such state exists,
   * return NULL. Print an asterisk for each state to tell the user that the
   * program hasn't died while the table is being constructed.
   */

  for (; Last_marked < &Dstates[Nstates]; ++Last_marked) {
    if (!Last_marked->mark) {
      putc('*', stderr);
      fflush(stderr);

      if (Verbose) {
        fputs("------------------\n", stdout);
        printf("working on DFA state %d = NFA states", Last_marked - Dstates);

        pset(Last_marked->set, fprintf, stdout);
        putchar('\n');
      }

      return Last_marked;
    }
  }
  return NULL;
}

PRIVATE void free_sets() {
  /* Free then memory used for the NFA sets in all Dstates entries. */

  DFA_STATE *p;

  for (p = &Dstates[Nstates]; --p >= Dstates;)
    delset(p->set);
}

PRIVATE void make_dtran(int sstate) { /* Starting NFA states */
  SET *NFA_set;       /* Set of NFA states that define the next DFA state */
  DFA_STATE *current; /* State currently being expanded */
  int nextstate;      /* Goto DFA state for current char */
  char *isaccept;     /* Current DFA state is an accept(this is the accepting
                         string) */
  int anchor;         /* Anchor point, if any */
  int c;              /* Current input character */

  /* Initially Dstates contains a single, unmarked, start state formed by taking
   * the epsilon closure of the NFA start state. So, Dstates[0](and Dtran[0]) is
   * the start state.
   */

  NFA_set = newset();
  ADD(NFA_set, sstate);

  Nstates = 1;
  Dstates[0].set = e_closure(NFA_set, &Dstates[0].accept, &Dstates[0].anchor);
  Dstates[0].mark = 0;

  while (current = get_unmarked()) /* Make the table */
  {
    current->mark = 1;

    for (c = MAX_CHARS; --c >= 0;) {
      if (NFA_set = move(current->set, c))
        NFA_set = e_closure(NFA_set, &isaccept, &anchor);

      if (!NFA_set) /* no outgoing transitions */
        nextstate = F;
      else if (nextstate = in_dstates(NFA_set) != -1)
        delset(NFA_set);
      else
        nextstate = add_to_dstates(NFA_set, isaccept, anchor);

      Dtran[current - Dstates][c] = nextstate;
    }
  }
  putc('\n', stderr); /* Terminate string of *'s printed in get_unmarked(); */

  free_sets(); /* Free the memory used for the DFA_STATE sets */
}
