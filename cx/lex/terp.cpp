#include "../tool/debug.h"
#include "../tool/set.h"
#include "dfa.h"
#include "nfa.h"
#include <climits>
#include <cstdio>
#include <cstdlib>

/* evaluates to the largest positive integer that be represented in an int,
 * works by shifting a number that contains all 1's to the right by one bit,
 * shifting a 0 into the high bit. */
#define LARGEST_INT (int)(((unsigned)(~0)) >> 1)

static NFA *Nfa;       /* Base address of NFA array */
static int Nfa_states; /* Number of states in NFA */

int nfa(char *(*input_routine)()) {
  /* Compile the NFA and initialize the various global variables used
   * by move() and e_closure(). Return the state number (index) of the
   * NFA start state. This routine must be called before either e_closure()
   * or move() are called. The memory used for the nfa can be freed with
   * free_nfa() (in thompson.cpp)
   */

  NFA *sstate;
  Nfa = thompson(input_routine, &Nfa_states, &sstate);
  return (sstate - Nfa);
}

void free_nfa() { free(Nfa); }

SET *e_closure(SET *input, char **accept, int *anchor) {
  /* input   is the set of start states to examine.
   * *accept is modified to point at the string associated with an accepting
   *          state (or to NULL if the state isn't an accepting state)
   * *anchor is modified to hold the anchor point, if any
   *
   * Computes the epsilon closure set for the input states. The output set
   * will contain all states that can be reached by marking epsilon transitions
   * from all NFA states in the input set. Returns an empty set if the input
   * set or the closure set is empty, modifies *accept to point at the accepting
   * string and *anchor is modified to hold the value of the NFA structure's
   * anchor if one of the elements of the output state is an accepting state,
   */

  int stack[NFA_MAX]; /* stack of untested states */
  int *tos;           /* stack pointer */
  NFA *p;             /* NFA state being examined */
  int i;              /* State number of " */
  int accept_num = INT_MAX;

  /* D(printf("e_closure of{");)
  D(pset(input);)
  D(printf("}=");) */

  if (!input)
    goto abort;
  *accept = NULL; /* Reference to algorithm */
  tos = &stack[-1];

  for (next_member(NULL); (i = next_member(input)) >= 0;)
    /* push all states in the input set into a stack */
    *++tos = i;

  while (INBOUNDS(stack, tos)) {
    /* pop the top element into i */
    i = *tos--;
    p = &Nfa[i];
    if (p->accept && (i < accept_num)) {
      accept_num = i;
      *accept = p->accept;
      *anchor = p->anchor;
    }

    /* if there's an epsilon transition from State i ot State N */
    if (p->edge == EPSILON) {
      if (p->next) {
        i = p->next - Nfa;
        /* if N isn't in the closure set */
        if (!MEMBER(input, i)) {
          /* Add N to the closure set */
          ADD(input, i);
          /* Push N onto the stack */
          *++tos = i;
        }
      }
      if (p->next2) {
        i = p->next2 - Nfa;
        if (!MEMBER(input, i)) {
          ADD(input, i);
          *++tos = i;
        }
      }
    }
  }
abort:
  /* D(printf("{");)
  D(pset(input);)
  D(printf(*accept ? "} ACCEPTING<%s>\n" : "}\n", *accept);) */
  return input;
}

SET *move(SET *inp_set, int c) {
  /* inp_set (input set) c (transition on this character) */
  /* Return a set that contains all NFA states that can be reached
   * by making transition on "c" from any states in "inp_set". Return NULL
   * if there are no such transitions. The inp_set is not modified
   */

  int i;
  NFA *p;             /* current NFA state */
  SET *outset = NULL; /* output set */

  /* D(printf("move({");)
  D(pset(inp_set);)
  D(printf("}, ");)
  D(fputc(c, stdout);)
  D(printf(")=");) */

  for (i = Nfa_states; --i > 0;) {
    if (MEMBER(inp_set, i)) {
      p = &Nfa[i];
      if (p->edge == c || (p->edge == CCL && MTEST(p->bitset, c))) {
        if (!outset)
          outset = newset();

        ADD(outset, p->next - Nfa);
      }
    }
  }
  return outset;
}
