#include "dfa.h"

/* make a minimal DFA by eliminating equivalent status */

void DfaStates::min_dfa() {
  NumGroups = 0;
  minimize();
}

void DfaStates::minimize() {
  int old_numgroups; /* used to see if we did anything in this pass */
  char c;            /* current character */
  Set **current;     /* current group being processed */
  Set **cnew;        /* New partition being created */
  int first;         /* State # of first element of current group */
  int next;          /* State # of next element of current group */
  int goto_first;    /* target of transition form first[c] */
  int goto_next;     /* other target of transition from first[c] */

  init_groups();
}

void DfaStates::init_groups() {
  Set **last;
  int i, j;

  last = &Groups[0];

  for (i = 0; i < DfaStatesNum; i++) {
    for (j = i; --j >= 0;) {
      /* Check to see if a group already exists that has the same accepting
       * string as the current state. If so, add the current state to the
       * already existing group and skip past the code that would create a new
       * group. Note that since all noaccepting states have NULL accept strings,
       * this loop puts all of these together into a single group. Also note
       * that the test in the for loop always fails for group 0, which can't be
       * an accpting state.
       */

      if (accepting_states[i].str == accepting_states[j].str) {
        Groups[Ingroup[j]]->add(i);
        Ingroup[i] = Ingroup[j];
        goto match;
      }
    }

    /* create a new group and put the current state into it. Note that add() has
     * side effects, so "last" can't be incremented in the add invocation
     */
    *last = new Set;
    (*last)->add(i);
    Ingroup[i] = NumGroups++;
    ++last;
  match:;
  }

  if (cmd.Verbose) {
    printf("Initial grouping:\n");
    pgroups();
  }
}

void DfaStates::pgroups() const {
  /* print all the groups used for minimization */

  Set *const *current;
  Set *const *end = &Groups[NumGroups];
  size_t i{0};

  for (current = Groups; current < end; ++current, ++i) {
    printf("\t group %ld: {", current - Groups);
    (*current)->psets();
    std::cout << accepting_states[i].str << std::endl;
    ;
    printf("}\n");
  }
  printf("\n");

  size_t nstates = DfaStatesNum;
  while (--nstates)
    printf("\tstate %zd is in group %2d\n", nstates, Ingroup[nstates]);
}
