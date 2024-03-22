#include "compiler.h"
#include "nfa.h"
#include "set.h"
#include <stack>
#include <string>

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

#ifdef DEBUG
int Lev = 0;
#define ENTER()                                                                \
  std::cout << std::format("{} enter {} on {} \"{}\"\n",                       \
                           std::string(Lev++ * 4, ' '), __func__,              \
                           tokenToStr(xCurrent_tok), bin_to_ascii(xLexeme, 0))
#define LEAVE()                                                                \
  std::cout << std::format("{} leave {} on {} \"{}\"\n",                       \
                           std::string(--Lev * 4, ' '), __func__,              \
                           tokenToStr(xCurrent_tok), bin_to_ascii(xLexeme, 0))
#else
#define ENTER()
#define LEAVE()
#endif
/*------------------------------------------------------------------------*/

static const char *tokenToStr(Token t) {
  switch (t) {
  [[likely]] case L:
    return "L";
    break;
  case AT_EOL:
    return "AT_EOL";
    break;
  case AT_BOL:
    return "AT_BOL";
    break;
  case OPEN_PAREN:
    return "OPEN_PAREN";
    break;
  case CLOSE_PAREN:
    return "CLOSE_PAREN";
    break;
  case CLOSURE:
    return "CLOSURE";
    break;
  case PLUS_CLOSE:
    return "PLUS_CLOSE";
    break;
  case DASH:
    return "DASH";
    break;
  case ANY:
    return "ANY";
    break;
  case OPTIONAL:
    return "OPTINAL";
    break;
  case CCL_START:
    return "CCL_START";
    break;
  case CCL_END:
    return "CCL_END";
    break;
  case OPEN_CURLY:
    return "OPEN_CURLY";
    break;
  case CLOSE_CURLY:
    return "CLOSE_CURLY";
    break;
  case OR:
    return "OR";
    break;
  case EOS:
    return "EOS";
    break;
  default:
    return nullptr;
  }

  return nullptr;
}
static Token Current_tok; /* Current Token  */
static int Lexeme;        /* Value associated with LITERAL  */

int curRegSize;
static const char *curPos;
static const char *endPos;
std::pair<Token, int> NfaStates::regToToken() {

  static int inquote = 0; /* Processing quoted string */
  int saw_esc;            /* Saw a backslash */

  if (*curPos == '"') {
    inquote = ~inquote;

    /* when encounter " curPos auto increment */
    /* the first " auto increment, and the second "
     * auto increment too */
    if (++curPos >= endPos) {
      Current_tok = EOS;
    }
  }

  saw_esc = (*curPos == '\\');

  /* if not in quoted string */
  if (!inquote) {
    /* eos which marks the end of the regular
     * expression, is returned if any white space in
     * encountered, and escape sequences are expanded
     * by the esc() */
    if (curPos == endPos) {
      Current_tok = EOS;
      Lexeme = '\0';
      goto exit;
    }
    Lexeme = esc(curPos);
    // D(std::cout << "esc execute" << std::endl;)
  } else { /* in quoted string */
    if (saw_esc && curPos[1] == '"') {
      /* this step handling escape character ( \" )
       * within the string */
      curPos += 2;
      Lexeme = '"';
    } else {
      Lexeme = *curPos++;
    }
  }

  /* inquote and saw_esc take away special meaning */
  Current_tok = (inquote || saw_esc) ? L : Tokmap[Lexeme];
exit:
  return std::make_pair(Current_tok, Lexeme);
}

void NfaStates::makeTokenStream() {
  /* for (auto it = curTokenStream.begin(); it != curTokenStream.end();)
  {
    it = curTokenStream.erase(it);
  } */
  std::vector<std::pair<Token, char>> newVec;
  curTokenStream.swap(newVec);

  Current_tok = BEFORE;
  curPos = curReg.c_str();
  endPos = curReg.c_str() + curReg.size();

  while (Current_tok != EOS)
    curTokenStream.push_back(regToToken());
}
void NfaStates::advance() {
  tokenLexeme = curTokenStream.at(tokenPos++);
  xCurrent_tok = tokenLexeme.first;
  xLexeme = tokenLexeme.second;
}
void NfaStates::thompson() {
  std::string regStr = RAs.at(0).first;
  /* clear the stack */
  while (!used.empty())
    used.pop();

  nfaStatesNum = 0;
  nextAlloc = 0;

  Nfa *start = machine();

  if (cmd.Verbose > 1) {
    std::cout << std::format("{}/{} Nfa states used \n", nextAlloc, nfaMax);
  }
}
Nfa *NfaStates::machine() {
  /* collect a series of rules and chains them together using epsilon
  transition and dummy states. */
  Nfa *start = nullptr;
  Nfa *p = nullptr;

  p = start = getNewNfa();

  int counter = 0;
  p->next = rule(RAs.at(0).first, RAs.at(0).second);

  while (++counter < RAs.size()) {
    std::cout << std::format("handle {} regular expression\n", counter);
    p->next2 = getNewNfa();
    p = p->next2;
    /* pay attention to the node may have epsilon edge and
     * only one next (CLOSURE, OPTINAL, PLUS_CLOSE) */
    p->next = rule(RAs.at(counter).first, RAs.at(counter).second);
  }

  return start;

  // int counter = 0;
  /* while (++counter < RAs.size()) {
    makeTokenStream();
    std::cout << std::endl;
    for (auto i : curTokenStream)
      std::cout << std::format(" {}({})", tokenToStr(i.first),
                               bin_to_ascii(i.second, 0));
    std::cout << std::endl;
    curReg = RAs.at(counter).first;
  } */
}

Nfa *NfaStates::rule(const std::string &regStr, const std::string &action) {
  /* gets a single regular expression and associated
   * action rule   -->  expr  EOS action ^expr EOS
   * action expr$ EOS action
   *
   * action  --> <tabs> <string of character>
   *             epsilon
   */
  curReg = regStr;
  std::cout << std::endl;
  std::cout << "the regular expression is " << curReg << std::endl;
  makeTokenStream();
  for (auto i : curTokenStream)
    std::cout << std::format("{}({})  ", tokenToStr(i.first),
                             bin_to_ascii(i.second, 0));
  std::cout << std::endl;

  advance();
  // std::cout << "the first token is " << tokenToStr(xCurrent_tok) <<
  // std::endl;
  ENTER();
  Nfa *start = nullptr;
  Nfa *end = nullptr;
  int anchor = NONE;

  /* while (xCurrent_tok != EOS) {
    std::cout << std::format("{}({}) ",
  tokenToStr(xCurrent_tok), (char)xLexeme); advance();
  }
  exit(1); */

  if (match(AT_BOL)) {
    /* an extra node is created with an outgoing
    transition on a newline so that
     * ^ is treated as if it were a regular expression
    that matched a newline
     */
    start = getNewNfa();
    start->edge = '\n';
    anchor |= START;
    advance();
    expr(&start, &end);
  } else
    expr(&start, &end);

  if (match(AT_EOL)) {
    /* pattern followed by a carriage-return or
    linefeed (use a character class)
     */
    advance();
    end->next = getNewNfa();
    end->edge = Edge::ccl;

    // if (!(end->bitset = new Set))
    //   std::cout << "out of memory" << std::endl;
    end->bitset = new Set;

    end->bitset->add('\n');
    end = end->next;
    anchor |= END;
  }

  end->accept = action;
  end->anchor = anchor;

  tokenPos = 0;

  LEAVE();
  // printNfa(start);

  return start;
}

void NfaStates::expr(Nfa **startp, Nfa **endp) {
  /* Because a recursive descent compiler can't handle
   * left recursion, the productions:
   *
   *  expr  -> expr OR cat_expr
   *         | cat_expr
   *
   * must be translated into:
   *
   *  expr  -> cat_expr exor'
   *  expr' -> OR cat_expr expr'
   *         | epsilon
   *
   * which can be implemented with this loop:
   *
   *  cat_expr
   *  while(match(OR))
   *         cat_expr
   *         do the OR
   */
  ENTER();

  Nfa *e2_start = nullptr;
  Nfa *e2_end = nullptr;
  Nfa *p;

  /* create a machine before OR token and the input is advanced to the
   * OR operator */
  cat_expr(startp, endp);

  while (match(OR)) {
    advance();
    /* create another machine after OR token */
    cat_expr(&e2_start, &e2_end);

    /* except for startp and enp, all other variables are only used to
    created
     * nodes, which are unnamed */

    p = getNewNfa();
    p->next2 = e2_start;
    p->next = *startp;
    *startp = p;

    p = getNewNfa();
    (*endp)->next = p;
    e2_end->next = p;
    *endp = p;
  }

  LEAVE();
}

void NfaStates::cat_expr(Nfa **startp, Nfa **endp) {
  /* The same translations that were needed in the expr
   * rules are needed again here:
   *
   *  cat_expr  -> cat_expr | factor
   *               factor
   *
   * is translated to:
   *
   *  cat_expr  -> factor cat_expr'
   *  cat_expr  -> | factor cat_expr'
   *               epsilon
   */
  ENTER();
  Nfa *e2_start = nullptr;
  Nfa *e2_end = nullptr;

  if (first_in_cat(xCurrent_tok))
    /* concatenation start. The first operand is
     * fetched with this factor call
     * ()
     */
    factor(startp, endp);

  while (first_in_cat(xCurrent_tok)) {
    /* The second and subsequent operand are fetched
     * with this factor call()
     */
    factor(&e2_start, &e2_end);
    /* The two are concatenated by overwriting the
     * contents of the end nodes of the first operand
     * with the contents of the starting node of the
     * second operand */
    // std::memcpy(*endp, e2_start, sizeof(Nfa));
    forMemoryCopy(*endp, e2_start);
    /* the discarded node is picked up again in the
     * next iteration of the loop because it will be at
     * the top of the pushback stack */
    discardNfa(e2_start);

    /* updated the endp */
    *endp = e2_end;
  }
  LEAVE();
}

int NfaStates::first_in_cat(Token tok) {
  /* concatenation is a somewhat harder problem because
   * there's no operator to look for The problem is
   * solved by this routine which looks to see if the
   * next input token can reasonably be concatenated to
   * the current one.
   */
  switch (tok) {
  case CLOSE_PAREN:
  case AT_EOL:
  case OR:
  case EOS:
    return 0;

  case CLOSURE:
  case PLUS_CLOSE:
  case OPTIONAL:
    std::cout << "+ ? or * must follow expression " << std::endl;
    exit(1);
    return 0;
  case CCL_END:
    std::cout << "Missing [ in character class " << std::endl;
    exit(1);
    return 0;
  case AT_BOL:
    std::cout << "^ must be at start of expr or ccl " << std::endl;
    exit(1);
    return 0;
  }

  return 1;
}

void NfaStates::factor(Nfa **startp, Nfa **endp) {
  /* factor --> term* | term+ | term?
   */

  ENTER();

  Nfa *start = nullptr;
  Nfa *end = nullptr;

  term(startp, endp);

  if (match(CLOSURE) || match(PLUS_CLOSE) || match(OPTIONAL)) {
    start = getNewNfa();
    end = getNewNfa();
    start->next = *startp;
    (*endp)->next = end;

    if (match(CLOSURE) || match(OPTIONAL)) /*  * or ? */
      start->next2 = end;

    if (match(CLOSURE) || match(PLUS_CLOSE)) /*  * or + */
      (*endp)->next2 = *startp;

    *startp = start;
    *endp = end;
    advance();
  }

  LEAVE();
}

void NfaStates::term(Nfa **startp, Nfa **endp) {
  /* process the term productions:
   *
   * term --> [...] | [^...] | [] | [^] | . | (expr) |
   * <character>
   *
   * the [] is nonstandard. It matches a space, tab,
   * formfeed, or newline, but not a carriage return
   * (\r). All of these are single nodes in the Nfa
   */
  ENTER();

  Nfa *start;
  int c;

  if (match(OPEN_PAREN)) {
    advance();
    expr(startp, endp);
    if (match(CLOSE_PAREN))
      advance();
    else {
      std::cout << " Missing close parenthesis " << std::endl;
      exit(1);
    }
  } else {
    /* The actual Nfa structure are allocated and
     * connected. */
    *startp = start = getNewNfa();
    *endp = start->next = getNewNfa();

    if (!(match(ANY) || match(CCL_START))) {
      /* doesn't encounter . and [ */
      start->edge = xLexeme;
      advance();
    } else {
      start->edge = ccl;

      start->bitset = new Set;

      if (match(ANY)) { /* dot(.) doesn't include '\n' */
        start->bitset->add('\n');

        start->bitset->ccomplement();
      } else { /* ccl */
        advance();
        if (match(AT_BOL)) /* Negative character class */
        {
          advance();

          start->bitset->add('\n'); /* don't include \n in class */

          start->bitset->ccomplement();
        }

        if (!match(CCL_END))
          dodash(start->bitset);
        else /* [] or [^] */
          for (c = 0; c <= ' '; ++c)
            start->bitset->add(c);
      }
      advance();
    }
  }

  LEAVE();
}

void NfaStates::dodash(Set *set) {
  /* convert the input tokens representing the class
   * into a Set. A set can have one element ([x]) or
   * servel elements [a-zA-Z] */
  int first;

  for (; !match(EOS) && !match(CCL_END); advance()) {
    if (!match(DASH)) {
      first = xLexeme;
      set->add(xLexeme);
      // std::cout << "add " << xLexeme << std::endl;
    } else {
      advance();
      for (; first <= xLexeme; first++) {
        // std::cout << "add " << first << std::endl;
        set->add(first);
      }
    }
  }
}
