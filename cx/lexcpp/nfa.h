#pragma once
#include "lexio.h"
#include "set.h"
#include <stack>

/*   epsilon: Non-character values of NFA.edge.
 *   ccl: if a transition is made on a character class, all character in the
 * classes are elements of the SET pointed to by bitset,and edge holds the value
 *   empty: if the state has no outgoing transitions, edge is set to EMPTY. */
enum Edge { empty = -3, ccl, epsilon };

#define NONE 0             /* Values of the anchor field:Not anchored */
#define START 1            /* Anchored at start of line */
#define END 2              /* Anchored at end of liine */
#define BOTH (START | END) /* Anchored in both places */

struct Nfa {
  int edge; /* Label for edge:character,CCL,EMPTY,or EPSILON. CCL represent
               character class */

  Set *bitset = nullptr; /* Set to stroe character classes */

  Nfa *next = nullptr;  /* Next state (or NULL if there is no outgoing edge) */
  Nfa *next2 = nullptr; /* Another next state if edge==EPSILON. used only for
                         states with two outgoing epsilon edges. */

  std::string accept{}; /* NULL if not an accapting state,else a pointer to
                   action  string. variable length(line number and string). */

  int anchor; /* Says whether pattern is anchored and,if
   so,where (uses #define above) */

  int stateNumber{}; /* index in arrays */
};

/* only seven characters have special meaning inside a characters class.
 * {  start of line
 * }  end of macro name
 * ]  end of character class
 * -  range of characters
 * ^  indicates negative character class
 * "  takes away special meaning characters up to next quote mark
 * \  takes away special meaning of next meaning
 */
typedef enum token {
  /* is returned at end of the regular expression, but the input buffer holds
   * the entire rule, including a multiple-line accepting action. The parser
   * uses this fact to pick up the accepting action when an EOS is encountered.
   */
  BEFORE,  /* used before makeTokenStream */
  EOS = 1, /* end of string */
  /* match any character except a newline. */
  ANY, /* . */
  /* an up arrow anchors the pattern to the start of line. the pattern ^and
   * matches string and only if it comprises the first three characters on the
   * line(no preceding white space). the newline is not part of the lexeme. */
  AT_BOL, /* ^ */
  /* a dollar sign anchors the pattern to end of line. The pattern and$ matches
   * the string only if it is the last three characters on the line(no following
   * whitespace). */
  AT_EOL, /* $ */
  /* brackets match any of the characters enclosed in the brackets. */
  CCL_END,     /* ] */
  CCL_START,   /* [ */
  CLOSE_CURLY, /* } */
  CLOSE_PAREN, /* ) */
  /* repeated zero or more times. */
  CLOSURE,    /* * */
  DASH,       /* - */
  L,          /* literal character include escaped characters */
  OPEN_CURLY, /* { */
  OPEN_PAREN, /* ( */
  /* matches zero or one repetitions. */
  OPTIONAL, /* ? */
  OR,       /* | */
  /* matches one or more repetitions. */
  PLUS_CLOSE /* + */

} Token;

class NfaStates {
public:
  NfaStates(const InOut &io) : RAs{io.getRAs()}, cmd{io.getCmd()} {
    nfaArrays = new Nfa[nfaMax]{};
    for (size_t i{0}; i < nfaMax; i++)
      nfaArrays[i].stateNumber = i;
  }
  NfaStates(const NfaStates &src) = delete;
  NfaStates &operator=(const NfaStates &rhs) = delete;
  NfaStates(NfaStates &&src) noexcept = delete;
  NfaStates &operator=(NfaStates &&rhs) noexcept = delete;
  ~NfaStates() { cleanup(); }
  void cleanup() noexcept;
  void getRuleAction();
  void handleSingleExpr();
  void handleAllExprs();
  friend void printNfa(const NfaStates &);
  void printRAs() const;
  void thompson();
  const Nfa *getNfaArrays() const { return nfaArrays; }
  size_t getNfaMaxSize() const { return nfaMax; }
  size_t getNfaSize() const { return nextAlloc; }
  Cmd getCmd() const { return cmd; }

  Nfa *getNewNfa();
  void discardNfa(Nfa *nfa_to_discard);

public:
  void forMemoryCopy(Nfa *des, Nfa *src); /* cat_expr */

private:
  std::pair<Token, int> regToToken();
  void makeTokenStream();
  void advance();
  bool match(Token tok) { return xCurrent_tok == tok; }
  // void clearSingleNfa(Nfa *nfa_to_discard) {
  //   if (nfa_to_discard->bitset != nullptr) {
  //     delete nfa_to_discard->bitset;
  //   }
  //   memset(nfa_to_discard, 0, sizeof(Nfa));
  //   nfa_to_discard->next = nullptr;
  //   nfa_to_discard->next2 = nullptr;
  // }
  Nfa *machine();
  Nfa *rule(const std::string &regStr, const std::string &action);
  void expr(Nfa **, Nfa **);
  void cat_expr(Nfa **, Nfa **);
  int first_in_cat(Token tok);
  void factor(Nfa **, Nfa **);
  void term(Nfa **, Nfa **);
  void dodash(Set *);

private:
  /* Maxium number of NFA states in a single machine. NFA_MAX*sizeof(NFA)
   * can't exceed 64K */
  const size_t nfaMax = 768;

  int nfaStatesNum = 0; /* nfa sates in machine */
  int nextAlloc = 0;    /* index of next element of the array */
  Nfa *nfaArrays;
  /* for discarded Nfa */
  std::stack<Nfa *> used;
  const std::vector<std::pair<std::string, std::string>> &RAs;
  const Cmd &cmd;
  std::string curReg;
  std::vector<std::pair<Token, char>> curTokenStream;
  std::pair<Token, int> tokenLexeme;
  Token xCurrent_tok;
  int xLexeme;
  int tokenPos{};
};

// void printNfa(Nfa *);
// clang-format off
/* indexed by ASCII value and evaluates to the token value
 * associated with that character */
static Token Tokmap[] = {
    /* ^@  ^A  ^B  ^C  ^D  ^E  ^F  ^G  ^H  ^I  ^J  ^K  ^L  ^M  ^N  */
    L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,

    /* ^O  ^P  ^Q  ^R  ^S  ^T  ^U  ^V  ^W  ^X  ^Y  ^Z  ^[  ^\  ^]  */
    L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,

    /* ^^  ^_  SPACE   !   "   #   $        %   &   '  */
    L, L, L, L, L, L, AT_EOL, L, L, L,

    /* (            )             *        +            ,   -      .  */
    OPEN_PAREN, CLOSE_PAREN, CLOSURE, PLUS_CLOSE, L, DASH, ANY,

    /* /   0   1   2   3   4   5   6   7   8   9   :   ;   <   =  */
    L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,

    /* >   ?   */
    L, OPTIONAL,

    /* @   A   B   C   D   E   F   G   H   I   J   K   L   M   N  */
    L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,

    /* O   P   Q   R   S   T   U   V   W   X   Y   Z   */
    L, L, L, L, L, L, L, L, L, L, L, L,

    /* [           \   ]         ^        */
    CCL_START, L, CCL_END, AT_BOL,

    /* _  `   a   b   c   d   e   f   g   h   i   j   k   l   m  */
    L, L, L, L, L, L, L, L, L, L, L, L, L, L, L,

    /* n   o   p   q   r   s   t   u   v   w   x   y   z  */
    L, L, L, L, L, L, L, L, L, L, L, L, L,

    /* {            |    }             DEL  */
    OPEN_CURLY, OR, CLOSE_CURLY, L};
// clang-format on
