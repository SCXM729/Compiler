#include "nfa.h"
#include "set.h"
#include <string>

/* Accepting string; nullptr is noaccepting */
/* Anchor point, if any. Values are defined in nfa.h */
struct accpet {
  std::string acceptStr;
  int anchor;
};

struct Dfa {
  unsigned group;     /* Group id, used by minimize() */
  bool mark{false};   /* Marked used by make_dtran() */
  std::string accept; /* accept action if accept state */
  int anchor;         /* Anchor point if an accept state */
  Set *set;           /* Set of Nfa states represented by this Dfa state */
  size_t dfaIndex{};
};

class DfaStates {
public:
  DfaStates();
  ~DfaStates();
  void makeDfa();
  void setNfaArrays(const Nfa *nfas, size_t size);
  void setCmd(const Cmd &cmd);
  Dfa *get_unmarked();
  int in_dstates(Set *);
  void free_sets();
  void make_dtran(int);
  size_t add_to_dstates(Set *, const std::string &accepting_string, int, int);

  Set *e_closure(Set *, std::string &, int *);
  void free_nfa();
  Set *move(Set *, char);

  void fix_dtran();
  void init_groups();
  void minimize();
  void pgroups() const;
  void min_dfa();

  friend void printNfa(const DfaStates &);

private:
  Nfa *NfaArrays;
  size_t nfaSize;
  /* Dfa states table, the DFA state number is the array index */
  Dfa *DfaArrays;
  /* Number of Dfa states */
  int DfaStatesNum;

  /* Maximum width of dfa transition table */
  static const int maxChar = 128;

  /* one full row of DfaDtran, which is itself an array, Dfa_max elements
   * long, of Rows. Declaring a row as distinct type helps you use pointers
   * to traverse individual columns. because incrementing a pointer to an
   * entire row will skip as many array elements as are in the row, effectively
   * moving you down to the next element of the current column
   */
  using row = int[maxChar];
  /* Dfa transition table */
  row *DfaDtran;

  /* Most-recently marked DFA state in DfaDtran */
  Dfa *Last_marked;
  /* maximum number of Dfa states. If this number >= 255, you'll
   * have to change the output routines and driver. States are numbered
   * from 0 to Dfa_max -1 */
  static const size_t Dfa_max = 254;
  /* marks failure state in the tables */
  const int F = -1;

  /* Groups of equivalent states in DfaDtran (collections of partitions */
  Set *Groups[Dfa_max]{};
  /* the Inverse of Groups */
  int Ingroup[Dfa_max]{};
  /* Number of groups in Groups */
  size_t NumGroups;

  struct Accept {
    std::string str;
    int anchor;
  };
  Accept *accepting_states;
  Cmd cmd;
};
