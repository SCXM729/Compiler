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

  // row *dtran = DfaDtran;
  /* put all state that has same accept string to the same group */
  init_groups();

  /* the do while loop continously detecting the first element with the other
   * elements in the same group index. Because if first element has the same
   * transfer with next and the element after next, which meaning the fist and
   * next and after next all are same. If from 0 to NumGroups with a for loop,
   * NumGroups increment, then the for loop form 0 to NumGroups again until
   * NumGroups equal to old_numgroups */
  do {
    old_numgroups = NumGroups;

    /* skip when the current only has one element, because it doesn't need to be
     * changed */
    for (current = &Groups[0]; current < &Groups[NumGroups]; ++current) {
      if ((*current)->numEle() <= 1)
        continue;

      /* add new group (the original is 0 to NumGroups-1) */
      cnew = &Groups[NumGroups];
      *cnew = new Set;

      // (*current)->psets();

      /* using a stack to get Dfa from a group */
      auto sstack = (*current)->nextMember();
      first = sstack.top();
      // std::cout << "the first is " << first << std::endl;
      sstack.pop();

      while (!sstack.empty()) {
        next = sstack.top();
        // std::cout << "the next is " << next << std::endl;
        sstack.pop();

        for (c = 127; c >= 0; c--) {
          goto_first = DfaDtran[first][c];
          goto_next = DfaDtran[next][c];

          /* Ingroup[goto_first doesn't equal to Ingroup[goto_next] make sure
           * that they doesn't be sorted */
          if (goto_first != goto_next &&
              (goto_first == F || goto_next == F ||
               Ingroup[goto_first] != Ingroup[goto_next])) {
            // std::cout << "execuate " << std::endl;
            // std::cout << std::format("goto_first {}, goto_next {}\n",
            //                          goto_first, goto_next);
            // std::cout << std::format(
            //     "Ingroup[goto_first] {}, Ingroup[goto_next] {}\n",
            //     Ingroup[goto_first], Ingroup[goto_next]);
            (*current)->remove(next);
            (*cnew)->add(next);
            /* indexed by state number, evaluates to in which group. put all the
             * elements that different with the first to a new and same group */
            Ingroup[next] = NumGroups;
            break;
          }
        }
      }

      if ((*cnew)->is_empty()) {
        delete *cnew;
      } else {
        // (*cnew)->psets();
        // std::cout << "NumGroups add" << NumGroups << std::endl;
        ++NumGroups;
      }
    }
  } while (old_numgroups != NumGroups);
  std::cout << "the NumGroups is " << NumGroups << std::endl;

  if (cmd.Verbose > 1) {
    printf("\n States grouped as follows after minimization:\n");
    pgroups();
  }
  fix_dtran();
}

void DfaStates::fix_dtran() {
  /*   Reduce the size of the Dtran, using the group set mode by minimize().
   * Return the state number of the start state. The original dtran and arrays
   * are destroyed and replaced with the smaller versions.
   *   Consider the first element of each group (current) to be a
   * "representative" state. Copy that state to the new transition table,
   * modifying all the transition in the representative state so that they'll go
   * to the group within the old state is found
   */
  Set **current;
  int state;
  int *src, *dest;
  Set **end = &Groups[NumGroups];

  row *newdtran = new row[NumGroups];
  Accept *newaccept = new Accept[NumGroups];

  for (current = Groups; current < end; ++current) {
    dest = &newdtran[current - Groups][0];
    auto sstack = (*current)->nextMember();
    state = sstack.top();
    sstack.pop();
    src = &DfaDtran[state][0];

    /* assign accpting string with the first element's accepting string */
    newaccept[current - Groups] = accepting_states[state];

    for (int j{127}; j >= 0; j--) {
      /* assign new values to newdtran */
      *dest = (*src == F) ? F : Ingroup[*src];
    }
  }

  delete[] accepting_states;
  delete[] DfaDtran;

  accepting_states = newaccept;
  DfaDtran = newdtran;
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
       * group. Note that since all noaccepting states have NULL accept
       * strings, this loop puts all of these together into a single group.
       * Also note that the test in the for loop always fails for group 0,
       * which can't be an accpting state.
       */

      if (accepting_states[j].str == accepting_states[i].str) {
        /* add the new state to the group */
        Groups[Ingroup[j]]->add(i);
        /* change another array for state i. because both are same, so */
        Ingroup[i] = Ingroup[j];
        /* skip the assignment phase */
        goto match;
      }
    }

    /* create a new group and put the current state into it. Note that add()
     * has side effects, so "last" can't be incremented in the add invocation
     */
    *last = new Set;
    (*last)->add(i);

    /* indexed by state number and evaluates to the group
     * in which the state is found */
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
    // std::cout << accepting_states[i].str << std::endl;
    ;
    printf("}\n");
  }
  printf("\n");

  size_t nstates = DfaStatesNum;
  // while (--nstates)
  //   printf("\tstate %zd is in group %2d\n", nstates, Ingroup[nstates]);
}
