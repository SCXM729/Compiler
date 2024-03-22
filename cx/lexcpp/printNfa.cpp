#include "compiler.h"
#include "dfa.h"
#include "nfa.h"
#include <cstdio>

static void printccl(Set *set) {
  static int i;
  putchar('[');
  for (i = 0; i < 127; ++i) {
    /* print unvisible character */
    if (set->test(i)) {
      if (i < ' ')
        printf("^%c", i + '@');
      else
        printf("%c", i);
    }
  }
  putchar(']');
}

static std::string mplab(const Nfa *nfa, const Nfa *state) {
  std::string buf;

  if (!nfa || !state)
    return std::string("--");

  buf = std::to_string(state - nfa);
  return buf;
}
static char *plab(Nfa *nfa, Nfa *state) {
  /* return a pointer to a buffer containing the state number. The buffer is
   * overwritten on each call so don't put more than on plab() call in an
   * argument to printf().
   */
  static char buf[32];

  if (!nfa || !state)
    return (char *)"--";

  sprintf(buf, "%ld", state - nfa);
  return buf;
}
void printNfa(const NfaStates &r) {

  int len = r.nextAlloc;
  const Nfa *move = r.nfaArrays;
  printf("\n---------------------------NFA-----------------------------\n");

  /* one step increment one index */
  for (; --len >= 0; move++) {
    // std::cout << std::format("NFA state {}: ", mplab(nfaArrays, move));
    std::cout << std::format("NFA state {}: ", move->stateNumber);
    // printf("NFA state %s: ", plab(s, nfa));

    /* if nfa doesn't have next */
    if (move->next == nullptr)
      std::cout << "(TERMINAL)";
    // printf("(TERMINAL)");
    else {
      /* have next */
      std::cout << std::format(
          "--> {} ({}) on ", move->next->stateNumber,
          move->next2 ? std::to_string(move->next2->stateNumber) : "--");
      // printf("--> %s ", plab(s, nfa->next));
      // printf("(%s) on ", plab(s, nfa->next2));
      // D(std::cout << std::format("{}\n", nfaArrays[2].edge);)

      switch (move->edge) {
      case Edge::ccl:
        printccl(move->bitset);
        break;
      case Edge::epsilon:
        printf("EPSILON");
        break;
      default:
        pchar(move->edge, stdout);
        break;
      }
    }

    if (move == r.nfaArrays)
      std::cout << " (START STATE)";
    // printf(" (START STATE)");

    if (!move->accept.empty())
      std::cout << std::format(
          " accepting {}<{}>{}", move->anchor & START ? "^" : "",
          move->accept.c_str(), move->anchor & END ? "$" : "");
    // printf(" accepting %s<%s>%s", nfa->anchor & START ? "^" : "",
    // nfa->accept.c_str(), nfa->anchor & END ? "$" : "");
    std::cout << std::endl;
  }
  printf("\n---------------------------NFA-----------------------------\n");
}
void printNfa(const DfaStates &r) {

  int len = r.nfaSize;
  const Nfa *move = r.NfaArrays;
  printf("\n---------------------------NFA-----------------------------\n");

  /* one step increment one index */
  for (; --len >= 0; move++) {
    // std::cout << std::format("NFA state {}: ", mplab(nfaArrays, move));
    std::cout << std::format("NFA state {}: ", move->stateNumber);
    // printf("NFA state %s: ", plab(s, nfa));

    /* if nfa doesn't have next */
    if (move->next == nullptr)
      std::cout << "(TERMINAL)";
    // printf("(TERMINAL)");
    else {
      /* have next */
      std::cout << std::format(
          "--> {} ({}) on ", move->next->stateNumber,
          move->next2 ? std::to_string(move->next2->stateNumber) : "--");
      // printf("--> %s ", plab(s, nfa->next));
      // printf("(%s) on ", plab(s, nfa->next2));
      // D(std::cout << std::format("{}\n", nfaArrays[2].edge);)

      switch (move->edge) {
      case Edge::ccl:
        printccl(move->bitset);
        break;
      case Edge::epsilon:
        printf("EPSILON");
        break;
      default:
        pchar(move->edge, stdout);
        break;
      }
    }

    if (move == r.NfaArrays)
      std::cout << " (START STATE)";
    // printf(" (START STATE)");

    if (!move->accept.empty())
      std::cout << std::format(
          " accepting {}<{}>{}", move->anchor & START ? "^" : "",
          move->accept.c_str(), move->anchor & END ? "$" : "");
    // printf(" accepting %s<%s>%s", nfa->anchor & START ? "^" : "",
    // nfa->accept.c_str(), nfa->anchor & END ? "$" : "");
    std::cout << std::endl;
  }
  printf("\n---------------------------NFA-----------------------------\n");
}
