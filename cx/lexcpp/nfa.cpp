#include "nfa.h"
/* clean up all Nfa with it's bitset */
void NfaStates::cleanup() noexcept {
  for (int i{0}; i < nfaMax; i++) {
    Nfa np{nfaArrays[i]};

    if (np.bitset == nullptr)
      continue;
    else {
      // std::cout << "delete" << std::endl;
      delete np.bitset;
    }
  }
  delete[] nfaArrays;
}

/* use a stack to reuse the Nfa. when discard Nfa, the discarded Nfa push to
 * stack. when get Nfa, pop form stack if any otherwise from nfaArrays */
Nfa *NfaStates::getNewNfa() {
  if (++nfaStatesNum >= nfaMax)
    std::cout << "Not enough memory for Nfa" << std::endl;

  Nfa *p;
  if (used.empty()) {
    p = &nfaArrays[nextAlloc++];
  } else {
    // if (used.size() == 1) {
    p = used.top();
    used.pop();
  }
  p->edge = Edge::epsilon;
  return p;
}
void NfaStates::discardNfa(Nfa *nfa_to_discard) {
  nfaStatesNum--;
  // clearSingleNfa(nfa_to_discard);
  if (nfa_to_discard->bitset != nullptr) {
    delete nfa_to_discard->bitset;
    nfa_to_discard->bitset = nullptr;
  }

  nfa_to_discard->next = nullptr;
  nfa_to_discard->next2 = nullptr;
  nfa_to_discard->edge = Edge::empty;
  used.push(nfa_to_discard);
  // std::cout << "the size of stack " << used.size() << std::endl;
  if (used.size() > 32)
    std::cout << "Internal error: discard stack full" << std::endl;
};

/* print all regular expression */
void NfaStates::printRAs() const {
  for (int counter{0}; auto i : RAs)
    std::cout << std::format("{}, regular is {}\n", counter++, i.first);
}

void NfaStates::forMemoryCopy(Nfa *des, Nfa *src) {
  des->edge = src->edge;
  des->next = src->next;
  des->next2 = src->next2;
  des->accept = src->accept;
  des->anchor = src->anchor;
  des->bitset = src->bitset;
  src->bitset = nullptr;
}
