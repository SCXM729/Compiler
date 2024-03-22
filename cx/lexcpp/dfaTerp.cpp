#include "dfa.h"
#include "nfa.h"
#include <climits>
#include <cstdio>
#include <cstdlib>

Set *DfaStates::e_closure(Set *input, std::string &accept, int *anchor) {
  /* input   is the set of start states to examine.
   * accept is modified to point at the string associated with an accepting
   *          state (or to empty if the state isn't an accepting state)
   * *anchor is modified to hold the anchor point, if any
   *
   * Computes the epsilon closure set for the input states. The output set
   * will contain all states that can be reached by marking epsilon transitions
   * from all NFA states in the input set. Returns an empty set if the input
   * set or the closure set is empty, modifies accept to point at the accepting
   * string and *anchor is modified to hold the value of the NFA structure's
   * anchor if one of the elements of the output state is an accepting state,
   */

  std::stack<size_t> sstack; /* stack of untested states */
  Nfa *p;                    /* NFA state being examined */
  size_t i;                  /* State number of " */
  size_t accept_num = INT_MAX;

  // D(printf("e_closure of{");)
  // D(input->psets();)
  // D(printf("}=");)

  if (input == nullptr)
    goto abort;

  sstack = input->nextMember();

  while (!sstack.empty()) {
    /* pop the top element into i */
    size_t i{sstack.top()};
    sstack.pop();

    p = &NfaArrays[i];

    /* from a node, make a epsilon transition, to a set that containing many
     * nodes but only one accepting node. */
    if (!p->accept.empty() && (i < accept_num)) {
      accept_num = i;
      accept = p->accept;
      *anchor = p->anchor;
    }

    /* if there's an epsilon transition from State i ot State N */
    if (p->edge == Edge::epsilon) {
      [[likely]] if (p->next) {
        i = p->next->stateNumber;
        /* if i isn't in the closure set */
        if (!input->isMember(i)) {
          /* Add new node (i) to the closure set */
          input->add(i);
          /* Push new node (i) onto the stack */
          sstack.push(i);
        }
      }
      /* there will be next edge and nullptr next2 edge  that marked
       * with epsilon*/
      [[likely]] if (p->next2) {
        i = p->next2->stateNumber;
        if (!input->isMember(i)) {
          input->add(i);
          sstack.push(i);
        }
      }
    }
  }
abort:
  // D(printf("{");)
  // D(input->psets();)
  // D(printf(!accept.empty() ? "} ACCEPTING<%s>\n" : "}\n", accept.c_str());)
  return input;
}

/* inp_set (input set) c (transition on this character) */
/* Return a set that contains all NFA states that can be reached
 * by making transition on "c" from any states in "inp_set". Return NULL
 * if there are no such transitions. The inp_set is not modified
 */
Set *DfaStates::move(Set *inp_set, char c) {

  int i{};
  Nfa *p;                /* current NFA state */
  Set *outset = nullptr; /* output set */

  /* D(printf("move({");)
  D(pset(inp_set);)
  D(printf("}, ");)
  D(fputc(c, stdout);)
  D(printf(")=");) */

  for (i = nfaSize; i > 0; i--) {
    if (inp_set->isMember(i)) {
      p = &NfaArrays[i];
      if (p->edge == c || (p->edge == Edge::ccl && p->bitset->test(c))) {
        if (outset == nullptr) /* initialize only once */
          outset = new Set;
        // std::cout << "move add element " << (int)c << std::endl;
        outset->add(p->next - NfaArrays);
      }
    }
  }
  return outset;
}
