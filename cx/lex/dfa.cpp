#include "dfa.h"
#include "../tool/compiler.h"
#include "../tool/debug.h"
#include "../tool/set.h"
#include "globals.h" /* externs for Verbose, etc. */
#include "nfa.h"
#include <cstdio>
#include <cstdlib>

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

/* DFA states table, the DFA state number is the array index */
static DFA_STATE *Dstates;

/*-----------------------------------------------------------------------*/

static ROW *Dtran;             /* DFA transition table */
static int Nstates;            /* Number of DFA states */
static DFA_STATE *Last_marked; /* Most-recently marked DFA state in Dtran */

// Prototypes for subroutines in this file
static DFA_STATE *get_unmarked(void);
static int in_dstates(SET *);
static void free_sets(void);
static void make_dtran(int);
static int add_to_dstates(SET *, char *, int, int);

int dfa(char *(*ifunct)(), ROW *(dfap[]), ACCEPT **acceptp) {
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
    ferr("No memory for DFA transition matrix");

  make_dtran(start); /* convert the NFA to a DFA */
  free_nfa(); /* Free the memory used for the nfa itself (but not the accept
                 strings) */

  Dtran = (ROW *)realloc(Dtran, Nstates * sizeof(ROW));
  accept_states = (ACCEPT *)malloc(Nstates * sizeof(ACCEPT));

  if (!accept_states || !Dtran)
    ferr("Out of memory");

  /* the accepting strings are copied to the target array */
  for (i = Nstates; --i >= 0;) {
    accept_states[i].string = Dstates[i].accept;
    accept_states[i].anchor = Dstates[i].anchor;
  }

  free(Dstates);
  *dfap = Dtran;
  *acceptp = accept_states;

  if (Verbose) {
    printf("\n%d out of %d DFA states in initial machine.\n", Nstates, DFA_MAX);
    printf("%ld bytes required for uncompressed tables\n\n",
           Nstates * MAX_CHARS * sizeof(TTYPE) + /* dtran */
               Nstates * sizeof(TTYPE));         /* accept */
  }

  if (Verbose > 1) {
    printf("The un-minimized DFA looks like this:\n\n");
    pheader(stdout, Dtran, Nstates, accept_states);
  }

  return Nstates;
}

static int add_to_dstates(SET *NFA_set, char *accepting_string, int anchor,
                          int ichar) {
  /* adds a new DFA state to the Dstates array and increments the
   * number-of-states counter, Nstates
   * */
  int nextstate;
  nextstate = Nstates++;

  if (Nstates > (DFA_MAX - 1))
    ferr("Too many DFA states\n");
  D(if (Verbose > 1))
  D(printf("Adding new DFA state (%d) on %c(%d):", nextstate, ichar, ichar);)

  Dstates[nextstate].set = NFA_set;
  Dstates[nextstate].accept = accepting_string;
  Dstates[nextstate].anchor = anchor;

  D(pset(Dstates[nextstate].set);)
  D(printf("\n");)

  return nextstate;
}

static int in_dstates(SET *NFA_set) {
  /* If there's a set in Dstates that is identical to NFA_set,return the
   * index of the Dstate entry, else return -1.
   */

  DFA_STATE *p;

  DFA_STATE *end = &Dstates[Nstates];
  for (p = Dstates; p < end; ++p)
    if (IS_EQUIVALENT(NFA_set, p->set))
      return (p - Dstates);

  return -1;
}

static DFA_STATE *get_unmarked() {
  /* Return a pointer to an unmarked state in Dstates, If no such state exists,
   * return NULL. Print an asterisk for each state to tell the user that the
   * program hasn't died while the table is being constructed.
   */

  for (; Last_marked < &Dstates[Nstates]; ++Last_marked) {
    if (!Last_marked->mark) {
      putc('*', stderr);
      fflush(stderr);
      if (Verbose > 1) {
        fputs("------------------\n", stdout);
        printf("working on DFA state %ld = NFA states{", Last_marked - Dstates);
        pset(Last_marked->set);
        printf("}");
        putchar('\n');
      }

      return Last_marked;
    }
  }
  return NULL;
}

static void free_sets() {
  /* Free then memory used for the NFA sets in all Dstates entries. */

  DFA_STATE *p;
  DFA_STATE *end = &Dstates[Nstates];

  for (p = Dstates; p < end; ++p)
    delset(p->set);
}

static void pstate(DFA_STATE *);
static void make_dtran(int sstate) {
  /* sstate representing Starting NFA states */
  SET *NFA_set; /* Set of NFA states that define the next DFA state */

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

  while ((current = get_unmarked())) /* Make the table */
  {
    D(printf("New unmarked state:\n\t");)
    D(pstate(current);)

    current->mark = 1;

    for (c = MAX_CHARS; --c >= 0;) {
      if ((NFA_set = move(current->set, c))) {
        NFA_set = e_closure(NFA_set, &isaccept, &anchor);
      }

      if (!NFA_set) /* no outgoing transitions */
        nextstate = F;
      else if ((nextstate = in_dstates(NFA_set)) !=
               -1) /* if next dfa state has existed */
        delset(NFA_set);
      else
        nextstate = add_to_dstates(NFA_set, isaccept, anchor, c);

      Dtran[current - Dstates][c] = nextstate;

      /* D(if (Verbose > 1 && nextstate != F)
            printf("Dfa state %ld goes to %d on %s(%d)\n", current - Dstates,
                   nextstate, bin_to_ascii(c, 1), c);) */
    }
  }
  putc('\n', stderr); /* Terminate string of *'s printed in get_unmarked(); */

  free_sets(); /* Free the memory used for the DFA_STATE sets */
}

#ifdef DEBUG

static void ps(SET *set) {
  putchar('{');
  pset(set);
  printf("}\n");
}

static void pstate(DFA_STATE *state) {
  printf("Dstates[%ld] ", (long)(state - Dstates));

  if (state->mark)
    printf("marked ");

  if (state->accept)
    printf("accepting %s<%s>%s", state->anchor & START ? "^" : "",
           state->accept, state->anchor & END ? "$" : "");
  ps(state->set);
}
#endif
