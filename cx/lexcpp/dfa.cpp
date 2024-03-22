#include "dfa.h"
DfaStates::DfaStates() {
  DfaStatesNum = 0;

  /* allocating memory for DfaArrays and assign index with every element */
  DfaArrays = new Dfa[Dfa_max]{};
  for (size_t i{0}; i < Dfa_max; i++)
    DfaArrays[i].dfaIndex = i;

  DfaDtran = new row[Dfa_max]{};
  Last_marked = DfaArrays;
}

/* copy a NfaArrays from NfaStates */
void DfaStates::setNfaArrays(const Nfa *nfas, size_t NfaSize) {
  nfaSize = NfaSize;
  NfaArrays = new Nfa[nfaSize]; /* truly allocate memory */
  for (size_t i = 0; i < nfaSize; i++) {
    if (nfas[i].bitset != nullptr)
      NfaArrays[i].bitset = new Set{*nfas[i].bitset};

    if (nfas[i].next != nullptr) {
      NfaArrays[i].next = &NfaArrays[nfas[i].next->stateNumber];
    }
    if (nfas[i].next2 != nullptr) {
      NfaArrays[i].next2 = &NfaArrays[nfas[i].next2->stateNumber];
    }
    NfaArrays[i].edge = nfas[i].edge;
    NfaArrays[i].accept = nfas[i].accept;
    NfaArrays[i].anchor = nfas[i].anchor;
    NfaArrays[i].stateNumber = nfas[i].stateNumber;
  }
}

/* copy Cmd from NfaStates */
void DfaStates::setCmd(const Cmd &cmd) { this->cmd = cmd; }
DfaStates::~DfaStates() {
  /* clean NfaArrays */
  for (size_t i{0}; i < nfaSize; i++) {
    if (NfaArrays[i].bitset != nullptr)
      delete NfaArrays[i].bitset;
  }
  delete[] NfaArrays;

  for (size_t i{0}; i < Dfa_max; i++)
    if (DfaArrays[i].set != nullptr)
      delete DfaArrays[i].set;
  delete[] DfaArrays;
  delete[] DfaDtran;
  delete[] accepting_states;

  for (size_t i{0}; i < NumGroups; i++) {
    if (Groups[i] != nullptr)
      delete Groups[i];
  }
}

void DfaStates::makeDfa() {
  if (cmd.Verbose > 1)
    fputs("making DFA:", stdout);

  make_dtran(0); /* convert Nfa to Dfa */

  accepting_states = new Accept[DfaStatesNum];

  /* the accepting string are copied to the target array */
  for (size_t i{0}; i < DfaStatesNum; i++) {
    accepting_states[i].str = DfaArrays[i].accept;
    accepting_states[i].anchor = DfaArrays[i].anchor;
  }

  size_t counter{0};
  for (size_t i{0}; i < DfaStatesNum; i++) {
    if (!DfaArrays[i].accept.empty())
      counter++;
  }
  std::cout << std::format(" there are {} accpeting string ", counter)
            << std::endl;

  if (cmd.Verbose) {
    printf("\n%d out of %zd DFA states in initial machine.\n", DfaStatesNum,
           Dfa_max);
    printf("%ld bytes required for uncompressed tables\n\n",
           DfaStatesNum * 128 * sizeof(char) + DfaStatesNum * sizeof(char));
  }
}

static void pstate(Dfa *); /* for debug */
static void ps(Set *);     /* for debug */

/* make DfaDtran */
void DfaStates::make_dtran(int sstate) {
  Set *nfaSet = new Set; /* set of NFA states that define the next DFA state */

  Dfa *current;         /* State currently being expanded */
  size_t nextstate;     /* Goto DFA state for current char */
  std::string isaccept; /* current DFA state is an accept (this is the
                     accepting string) */
  int anchor;           /* Anchor point, if any */
  char c;               /* Current input character */

  /* Initially Dstates contains a single, unmarked, start state formed by
   * taking the epsilon closure of the NFA start state. So, Dstates[0](and
   * Dtran[0]) is the start state.
   */
  nfaSet->add(sstate);

  DfaStatesNum = 1;
  DfaArrays[0].set =
      e_closure(nfaSet, DfaArrays[0].accept, &DfaArrays[0].anchor);
  DfaArrays[0].mark = false;

  while ((current = get_unmarked())) {
    D(printf("New unmarked state(stateIndex %zd):\n\t", current->dfaIndex));
    D(pstate(current));
    D(ps(current->set));

    current->mark = true;

    for (c = 127; c >= 0; c--) {

      nfaSet = move(current->set, c);
      if (nfaSet != nullptr) {
        nfaSet = e_closure(nfaSet, isaccept, &anchor);
        // nfaSet->psets();
        // std::cout << std::endl;
      }

      if (nfaSet == nullptr)
        nextstate = F;
      else if (nextstate = in_dstates(nfaSet); nextstate != -1) {
        delete nfaSet;
        nfaSet = nullptr;
      } else {
        // nfaSet->psets();
        // std::cout << std::endl;
        nextstate = add_to_dstates(nfaSet, isaccept, anchor, c);
      }

      DfaDtran[current->dfaIndex][c] = nextstate;
    }

    if (nfaSet != nullptr) {
      nfaSet->psets();
    } else {
      std::cout << "nfaSet is nullptr " << std::endl;
    }
  }
}

/* return a pointer to an unmarked state in DfaArrays. If no such state
 * exists, return nullptr. Print an asterisk for each state to tell the user
 * that the program hasn't died while the table is being constructed.
 */
Dfa *DfaStates::get_unmarked() {
  for (; Last_marked < &DfaArrays[DfaStatesNum]; ++Last_marked) {
    // std::cout << "DfaStatesNum is " << DfaStatesNum << std::endl;
    if (!Last_marked->mark) {
      putc('*', stdout);
      fflush(stdout);
      if (cmd.Verbose > 1) {
        std::cout << std::format("{:-^50}\n", '-');
        printf("working on DFA state[%zd] = Nfa states{",
               Last_marked->dfaIndex);
        Last_marked->set->psets();
        printf("}\n");
      }

      return Last_marked;
    }
  }
  return nullptr;
}

/* if there's a set in DfaArrays that is identical to nfaSet, return the
 * index of the DfaArrays entry, else return -1
 */
int DfaStates::in_dstates(Set *nfaSet) {
  Dfa *p;

  Dfa *atArrayEnd = &DfaArrays[DfaStatesNum];
  for (p = DfaArrays; p < atArrayEnd; ++p) {
    if (*nfaSet == *p->set) {
      /* if same, return index */
      return p->dfaIndex;
    }
  }

  return -1;
}

/* add a new Dfa state to the DfaArrays array and increments the
 * number-of-states counter (DfaStatesNum) */
size_t DfaStates::add_to_dstates(Set *nfaSet,
                                 const std::string &accepting_string,
                                 int anchor, int ichar) {

  size_t nextstate;
  nextstate = DfaStatesNum++;

  if (DfaStatesNum > Dfa_max - 1) {
    throw std::out_of_range("Too many Dfa states");
  }
  D(if (cmd.Verbose > 1))
  D(printf("Adding new Dfa state (%zd) on %c(%d):", nextstate, ichar, ichar);)

  DfaArrays[nextstate].set = nfaSet;
  DfaArrays[nextstate].accept = accepting_string;
  DfaArrays[nextstate].anchor = anchor;

  D(DfaArrays[nextstate].set->psets());
  D(printf("\n"));

  return nextstate;
}

#ifdef DEBUG

static void ps(Set *set) {
  putchar('{');
  set->psets();
  printf("}\n");
}

static void pstate(Dfa *state) {
  printf("Dstate[%zd]", state->dfaIndex);
  if (state->mark)
    printf("marked");

  if (!state->accept.empty())
    printf("accepting %s<%s>%s", state->anchor & START ? "^" : "",
           state->accept.c_str(), state->anchor & END ? "$" : "");
}
#endif
