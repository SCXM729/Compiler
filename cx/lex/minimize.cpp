#include "../tool/compiler.h"
#include "../tool/set.h"
#include "dfa.h"
#include "globals.h" /* externs for Verbose */
#include "nfa.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* minimize.cpp: make a minimal DFA by eliminating equivalent states */

static SET *Groups[DFA_MAX]; /* Groups of equivalent states in Dtran
                                (collections of partitions) */
static int Numgroups;        /* Number of groups in Groups */
static int Ingroup[DFA_MAX]; /* the Inverse of Groups */

/*--------------------------------------------------------------------
 * Prototypes for subroutines in this file:
 */

static void fix_dtran(ROW *[], ACCEPT **);
static void init_groups(int, ACCEPT *);
int min_dfa(char *(*)(), ROW *[], ACCEPT **);
static void minimize(int, ROW *[], ACCEPT **);
static void pgroups(int);

#ifdef DEBUG
static void setp(SET *);
#endif
/*----------------------------------------------------------------*/

int min_dfa(char *(*ifunct)(), ROW *(dfap[]), ACCEPT *(*acceptp)) {
  /* Make a minimal DFA, eliminating equivalent states. Return the number of
   * states in the minimized machine. *sstatep = the new start state.
   */

  int nstates; /* Number of DFA states */

  memset(Groups, 0, sizeof(Groups));
  memset(Ingroup, 0, sizeof(Ingroup));
  Numgroups = 0;

  nstates = dfa(ifunct, dfap, acceptp);
  minimize(nstates, dfap, acceptp);

  return Numgroups;
}

static void init_groups(int nstates, ACCEPT *accept) {
  SET **last;
  int i, j;

  last = &Groups[0];
  Numgroups = 0;

  for (i = 0; i < nstates; i++) {
    for (j = i; --j >= 0;) {
      /* Check to see if a group already exists that has the same accepting
       * string as the current state. If so, add the current state to the
       * already existing group and skip past the code that would create a new
       * group. Note that since all nonaccepting states have NULL accept
       * strings, this loop puts all of these together into a single group. Also
       * note that the test in the for loop always fails for group 0, which
       * can't be an accepting state.
       */

      if (accept[i].string == accept[j].string) {
        ADD(Groups[Ingroup[j]], i);
        Ingroup[i] = Ingroup[j];
        goto match;
      }
    }

    /* Create a new group and put the current state into it. Note that ADD()
     * has side effects, so "last" can't be incremented in the ADD invocation
     */

    *last = newset();
    ADD(*last, i);
    Ingroup[i] = Numgroups++;
    ++last;
  match:;
  }

  if (Verbose) {
    printf("Initial groupings:\n");
    pgroups(nstates);
  }
}

static void minimize(int nstates, ROW *(dfap[]), ACCEPT **acceptp) {
  /* nstates : number of states in dtran[] */
  /* DFA transition table to compress */
  /* Set of accept states */

  int old_numgroups; /* Used to see if we did anything in this pass */
  int c;             /* Current character */
  SET **current;     /* Current group being processed */
  SET **cnew;        /* New partition being created */
  int first;         /* State # of first element of current group */
  int next;          /* State # of next element of current group */
  int goto_first;    /* target of transition from first[c]. */
  int goto_next;     /* other target of transition from first[c]. */

  ROW *dtran = *dfap;
  ACCEPT *accept = *acceptp;

  init_groups(nstates, accept);
  do {
    old_numgroups = Numgroups;
    for (current = &Groups[0]; current < &Groups[Numgroups]; ++current) {
      if (num_ele(*current) <= 1)
        continue;

      cnew = &Groups[Numgroups];
      *cnew = newset();

      next_member(NULL);
      first = next_member(*current);

      while ((next = next_member(*current)) >= 0) {
        for (c = MAX_CHARS; --c >= 0;) {
          goto_first = dtran[first][c];
          goto_next = dtran[next][c];

          if (goto_first != goto_next &&
              (goto_first == F || goto_next == F ||
               Ingroup[goto_first] != Ingroup[goto_next])) {
            REMOVE(*current, next); /* Move the start to */
            ADD(*cnew, next);       /* the new partitiion */
            Ingroup[next] = Numgroups;
            break;
          }
        }
      }

      if (IS_EMPTY(*cnew))
        delset(*cnew);
      else
        ++Numgroups;
    }
  } while (old_numgroups != Numgroups);

  if (Verbose > 1) {
    printf("\nStates grouped as follows after minimization:\n");
    pgroups(nstates);
  }
  fix_dtran(dfap, acceptp);
}

static void fix_dtran(ROW *dfap[], ACCEPT **acceptp) {
  /*   Reduce the size of the dtran, using the group set mode by minimize().
   * Return the state number of the start state. The original dtran and accept
   * arrays are destroyed and replaced with the smaller versions.
   *   Consider the first element of each group (current) to be a
   * "representative" state . Copy that state to the new transition table,
   * modifying all the transitions in the representative state so that they'll
   * go to the group within the old state is found.
   */

  SET **current;
  ROW *newdtran;
  ACCEPT *newaccept;
  int state;
  int i;
  int *src, *dest;
  ROW *dtran = *dfap;
  ACCEPT *accept = *acceptp;
  SET **end = &Groups[Numgroups];

  newdtran = (ROW *)calloc(Numgroups, sizeof(ROW));
  newaccept = (ACCEPT *)calloc(Numgroups, sizeof(ACCEPT));

  if (!newdtran || !newaccept)
    ferr("Out of memory");

  next_member(NULL);
  for (current = Groups; current < end; ++current) {
    dest = &newdtran[current - Groups][0];
    state = next_member(*current); /* All groups have at */
    src = &dtran[state][0];        /* least one element */

    newaccept[current - Groups] = accept[state]; /* Struct. assignment */

    for (i = MAX_CHARS; --i >= 0; src++, dest++) {
      *dest = (*src == F) ? F : Ingroup[*src];
    }
  }

  free(*dfap);
  free(*acceptp);
  *dfap = newdtran;
  *acceptp = newaccept;
}

static void pgroups(int nstates) {
  /* Print all the groups used for minimization. */

  SET **current;
  SET **end = &Groups[Numgroups];

  for (current = Groups; current < end; ++current) {
    printf("\tgroup %ld: {", current - Groups);
    pset(*current);
    printf("}\n");
  }
  printf("\n");
  while (--nstates >= 0)
    printf("\tstate %2d is in group %2d\n", nstates, Ingroup[nstates]);
}

#ifdef DEBUG
/* The following function is not called. It's here so that it can be called
 * from the debugger if you like.
 */

static void setp(SET *set) {
  pset(set);
  putchar('\n');
}
#endif
