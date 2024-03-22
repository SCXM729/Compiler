#include "globals.h" /* extension for verbose */
#include "nfa.h"
#include <cstdio>
#include <cstdlib>
#define BSIZE 256

static char Buf[BSIZE];  /* input buffer */
static char *Pbuf = Buf; /* current position in input buffer */
static char *Expr;       /* regular expression from command line */

int Verbose = 2;
int No_lines = 0;
int Unix = 1;
int Public = 0;
const char *Template = "lex.par";
int Actual_lineno = 1;
int Lineno = 1;

int nextchar() {
  /* get next input character and buffers entire lines because egrep must print
   * the entire when a match is found */
  if (!*Pbuf)
    if (!fgets(Buf, BSIZE, stdin))
      return NULL;

  return *Pbuf++;
}

static void printbuf() {
  fputs(Buf, stdout); /* print the buffer and force a read */
  *Pbuf = 0;          /* on the next call to nextchar() */
}

static char *ccgetline() {
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
  /* in book, int accept */
  char *accept; /* cur. DFA state is an accept */
  int c;        /* current input character */
  int anchor;

  if (argc == 2) {
    fprintf(stderr, "expression is %s, need a text to be searched\n", argv[1]);
  } else {
    fprintf(stderr, "usage: terp pattern <input\n");
    exit(1);
  }

  /* 1): Compile the NFA; initialize move() & e_closure().
   * 2): Create the initial state, the set of all NFA states
   * that can be reached by making epsilon transition from
   * the NFA start state.
   * 3): Initialize the current state to the start state
   */

  Expr = argv[1]; /* 1 */
  sstate = nfa(ccgetline);

  start_dfastate = newset(); /* 2 */
  ADD(start_dfastate, sstate);
  if (!e_closure(start_dfastate, &accept, &anchor)) {
    fprintf(stderr, "Internal error: State machine is empty\n");
    exit(1);
  }

  current = newset(); /* 3 */
  ASSIGN(current, start_dfastate);

  /* Now interpret the NFA: The next state is the set of all NFA states that
   * can be reached after we've made a transition on the current input character
   * from any of the NFA states in the current state. The current input line is
   * printed every time an accept state is encountered. The machine is reset to
   * the initial state when a failure transition is encountered
   */

  while ((c = nextchar())) {
    next = e_closure(move(current, c), &accept, &anchor);
    if (accept) {
      printbuf(); /*  accept */
      if (next)
        ;
      delset(next); /* reset */
      ASSIGN(current, start_dfastate);
    } else { /* keep looking */
      delset(current);
      current = next;
    }
  }

  delset(current);        /* not required for main, but you'll */
  delset(start_dfastate); /* need it when adapting main() to a subroutine */
}
