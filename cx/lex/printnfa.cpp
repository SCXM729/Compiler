#include "../tool/debug.h"
#include "../tool/hash.h"
#include "nfa.h"
#include <cstdio>

PRIVATE void printccl(SET *);
PRIVATE void *plab(NFA *, NFA *);
PUBLIC void print_nfa(NFA *, int, NFA);

/*----------------------------------------------------
 * printnfa.cpp Routine to print out a NFA structure in human-readable form
 */

PRIVATE void printccl(SET *set) {
  static int i;
  putchar('[');
  for (i = 0; i < 0x7f; ++i) {
    if (TEST(set, i)) {
      if (i < ' ')
        printf("^%c", i + '@');
      else
        printf("%c", i);
    }
  }
  putchar(']');
}

PRIVATE char *plab(NFA *nfa, NFA *state) {
  /* Return a pointer to a buffer containing the state number. The buffer is
   * overwritten on each call so don't put more than one plab() call in an
   * argument to printf().
   */

  static char buf[32];

  if (!nfa || !state)
    return ("--");

  sprintf(buf, "%2d", state - nfa);
  return buf;
}

/*----------------------------------------------------------------*/

PUBLIC void print_nfa(NFA *nfa, int len, NFA *start) {
  NFA *s = nfa;

  printf("\n----------------------NFA------------------\n");

  for (; --len >= 0; nfa++) {
    printf("NFA state %s: ", plab(s, nfa));

    if (!nfa->next)
      printf("(TERMINAL)");
    else {
      printf("--> %s ", plab(s, nfa->next));
      printf("(%s) on ", plab(s, nfa->next2));

      switch (nfa->edge) {
      case CCL:
        printccl(nfa->bitset);
        break;
      case EPSILON:
        printf("EPSILON");
        break;
      default:
        pchar(nfa->edge, stdout);
        break;
      }
    }

    if (nfa == start)
      printf(" (START STATE)");

    if (nfa->accept)
      printf(" accepting %s<%s>%s", nfa->anchor & START ? "^" : "", nfa->accept,
             nfa->anchor & END ? "$" : "");
    printf("\n");
  }
  printf("\n--------------------------------------------------\n");
}
