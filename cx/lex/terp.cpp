#include "../tool/debug.h"
#include "../tool/set.h"
#include "nfa.h"
#include <cctype>
#include <cstdio>

/* implement nfa */

PUBLIC int nfa(char *(*)());
PUBLIC void free_nfa(void);
PUBLIC SET *e_closure(SET *, char **, int *);
PUBLIC SET *move(SET *, int);

/*--------------------------------------------------*/

#define LARGEST_INT (int)(((unsigned)(~0)) >> 1)

PRIVATE NFA *Nfa;       /* Base address of NFA array */
PRIVATE int Nfa_states; /* Number of states in NFA */

/*---------------------------------------------------*/

PUBLIC int nfa(char *(*input_routine)()) {
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

PUBLIC void free_nfa() { free(Nfa); }

PUBLIC SET *e_closure(SET *input, char **accept, int *anchor) {
  /* input   is the set states to examine.
   * *accept is modified to point at the string associated with an accepting
   *          state (or to NULL if the state isn't an accepting state)
   * *anchor is modified to hold the anchor point, if any
   *
   * Computes the epsilon closure set for the input states. The output set
   * will contain all states that can be reached by marking epsilon transitions
   * from all NFA states in the input set. Returns an empty set if the input
   * set or the closure set is empty, modifies *accept to point at the accepting
   * string if one of the elements of the output state is an accepting state
   */

  int stack[NFA_MAX]; /* stack of untested states */
  int *tos;           /* stack pointer */
  NFA *p;             /* NFA state being examined */
  int i;              /* State number of */
  int accept_num = LARGEST_INT;

  if (!input)
    goto abort;
  *accept = NULL; /* Reference to algorithm */
  tos = &stack[-1];

  for (next_number(NULL); (i = next_number(input)) >= 0;)
    *++tos = i;

  while (INBOUNDS(stack, tos)) {
    i = *tos--;
    p = &Nfa[i];
    if (p->accept && (i < accept_num)) {
      accept_num = 1;
      *accept = p->accept;
      *anchor = p->anchor;
    }

    if (p->edge == EPSILON) {
      if (p->next) {
        i = p->next - Nfa;
        if (!MEMBER(input, i)) {
          ADD(input, i);
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
  return input;
}

PUBLIC SET *move(SET *inp_set, int c) {
  /* inp_set (input set) c (transition on this character) */
  /* Return a set that contains all NFA states that can be reached
   * by making transition on "c" from any states in "inp_set". Return NULL
   * if there are no such transitions. The inp_set is not modified
   */

  int i;
  NFA *p;             /* current NFA state */
  SET *outset = NULL; /* output set */

  for (i = Nfa_states; --i > 0;) {
    if (MEMBER(inp_set, i)) {
      p = &Nfa[i];
      if (p->edge == c || (p->edge == CCL && TEST(p->bitset, c))) {
        if (!outset)
          outset = newset();

        ADD(outset, p->next - Nfa);
      }
    }
  }
  return (outset);
}

#ifdef MAIN
#define ALLOCATE
#include "globals.h" /* extension for verbose */

#define BSIZE 256

PRIVATE char Buf[BSIZE];  /* input buffer */
PRIVATE char *Pbuf = Buf; /* current position in input buffer */
PRIVATE char *Expr;       /* regular expression from command line */
PRIVATE char *Expr;       /* regular expression from command line */

int nextchar() {
  if (!Pbuf) {
    if (!fgets(Buf, BSIZE, stdin))
      return null;
    Pbuf = Buf;
  }
  return *Pbuf++;
}

PRIVATE void printfbuf() {
  fputs(Buf, stdout); /* print the buffer and force a read */
  *Pbuf = 0;          /* on the next call to nextchar() */
}

PRIVATE char *getline() {
  static int first_time_called = 1;
  if (!first_time_called)
    return NULL;

  first_time_called = 0;
  return Expr;
}

int main(int argc, char *argv[]) {
  int sstate;          /* Starting NFA state */
  SET *start_dfastate; /* Set of starting nfa states */
  SET *current;        /* current DFA state */
  SET *next;
  int accept; /* cur. DFA state is an accept */
  int c;      /* current input character */
  int anchor;

  if (argc == 2)
    fprintf(stderr, "expression is %s \n", argv[1]);
  else {
    fprintf(stderr, "usage: terp pattern <input\n");
    exit(1)
  }

  /* 1): Compile the NFA; initialize move() & e_closure().
   * 2): Create the initial state, the set of all NFA states
   * that can be reached by making epsilon transition from
   * the NFA start state.
   * 3): Initialize the current state to the start state
   */

  Expr = argv[1];

  sstate = nfa(getline);

  next = newset();
  ADD(next, sstate);
  if (!(start_dfastate = e_closure(next, &accept, &anchor))) {
    fprintf(stderr, "Internal error: State machine is empty\n");
    exit(1);
  }

  current = newset();
  assign(current, start_dfastate);

  /* Now interpret the NFA: The next state is the set of all NFA states that
   * can be reached after we've made a transition on the current input character
   * from any of the NFA states in the current state. The current input line is
   * printed every time an accept state is encountered. The machine is reset to
   * the initial state when a failure transition is encountered
   */

  while (c = nextchar()) {
    if (next = e_closure(move(current, c), &accept, &achor)) {
      if (accept)
        printbuf(); /*  accept */
      else {
        delset(current); /* keep looking */
        current = next;
        continue;
      }
    }

    delset(next); /* reset */
    assigne(current, start_dfastate);
  }
}
#endif
