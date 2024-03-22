#include "nfa.h"
#include "../tool/compiler.h"
#include "../tool/debug.h"
#include "../tool/hash.h"
#include "../tool/set.h"
#include "../tool/stack.h"
#include "globals.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef DEBUG
/* int Lev = 0;
#define ENTER(f) printf("%*senter %s [%c][%1.10s]\n", */
#define ENTER(f)
#define LEAVE(f)
// Lev++ * 4,"",f,Lexeme,Input)
// #define LEAVE(f) printf("%*sleave %s [%c][%1.10s]\n",
// Lev-- * 4,"",f,)
#else
#define ENTER(f)
#define LEAVE(f)
#endif


/*------------------------------------------------------
* Error processing stuff.Note that all errors are fatal
* -----------------------------------------------------
*/

typedef enum _err_num_{
  E_MEM,          /* Out of memory */
  E_BADEXPR,      /* Malformed regular expression */
  E_PAREN,        /* Missing close parenthesis */
  E_STACK,        /* Internal error: Discard stack full */
  E_LENGTH,       /* Too many regular expressions */
  E_BRACKET,      /* Missing [ in character class */
  E_BOL,          /* ^ must be at start of expr or ccl */
  E_CLOSE,        /* + ? or * must follow expression */
  E_STRINGS,      /* Too many characters in accept actions */
  E_NEWLINE,      /* Newline in quoted string */
  E_BADMAC,       /* Missing } in macro expansion */
  E_NOMAC,        /* Macro doesn't exist */
  E_MACDEPTH      /* Macro expansion nested too deeply */
} ERR_NUM;

static const char *Errmsgs[] = {
    /* indexed by ERR_NUM */
    "Not enough memory for NFA",
    "Malformed regular expression",
    "Missing close parenthesis",
    "Internal error: Discard stack full",
    "Too many regular expression or expression too long",
    "Missing [ in characters class",
    "^ must be at start of expression or after [",
    "+ ? or * must follow an expression or subexpression",
    "Too many characters in accept actions",
    "Newline in quoted string, use \\n to get newline into expression",
    "Missing } in marco expansion",
    "Macro doesn't exist",
    "Macro expansions nested too deeply",
};

/*------------------------------------------------------------------------*/

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
  CLOSURE,      /* * */
  DASH,         /* - */
  END_OF_INPUT, /* EOF */
  L,            /* literal character include escaped characters */
  OPEN_CURLY,   /* { */
  OPEN_PAREN,   /* ( */
  /* matches zero or one repetitions. */
  OPTIONAL, /* ? */
  OR,       /* | */
  /* matches one or more repetitions. */
  PLUS_CLOSE /* + */

} TOKEN;

// clang-format off
/* indexed by ASCII value and evaluates to the token value
 * associated with that character */
static TOKEN Tokmap[] = {
/* ^@  ^A  ^B  ^C  ^D  ^E  ^F  ^G  ^H  ^I  ^J  ^K  ^L  ^M  ^N  */
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

/* ^O  ^P  ^Q  ^R  ^S  ^T  ^U  ^V  ^W  ^X  ^Y  ^Z  ^[  ^\  ^]  */
    L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, 

/* ^^  ^_  SPACE   !   "   #   $        %   &   '  */
    L,  L, L,      L,  L,  L,  AT_EOL,  L,  L,  L,

/* (            )             *        +            ,   -      .  */
   OPEN_PAREN,  CLOSE_PAREN,  CLOSURE, PLUS_CLOSE,  L,  DASH,  ANY,

/* /   0   1   2   3   4   5   6   7   8   9   :   ;   <   =  */
   L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

/* >   ?   */
   L,  OPTIONAL,

/* @   A   B   C   D   E   F   G   H   I   J   K   L   M   N  */
   L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

/* O   P   Q   R   S   T   U   V   W   X   Y   Z   */
   L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

/* [           \   ]         ^        */
   CCL_START,  L,  CCL_END,  AT_BOL, 

/* _  `   a   b   c   d   e   f   g   h   i   j   k   l   m  */
  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, 

/* n   o   p   q   r   s   t   u   v   w   x   y   z  */
   L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,

/* {            |    }             DEL  */
   OPEN_CURLY,  OR,  CLOSE_CURLY,  L
};
// clang-format on

/* Input function pointer like gets(), returning either the next input line or
 * NULL at end of file. */
static char *(*Ifunct)();
static char *Input = (char *)""; /* Current position in input string  */
static char *S_input;            /* Beginning of input string */
static TOKEN Current_tok;        /* Current Token  */
static int Lexeme;               /* Value associated with LITERAL  */

#define MATCH(t) (Current_tok == (t))

/*-----------------------------------------------------------------------*/

static void parse_err(ERR_NUM type) {
  fprintf(stderr, "ERROR (line %d) %s \n%s\n", Actual_lineno, Errmsgs[type],
          S_input);
  while (++S_input <= Input)
    putc('_', stderr);
  fprintf(stderr, "^\n");
  exit(1);
}

static NFA *Nfa_states; /* State-machine array */
static int Nstates = 0; /* # of NFA states in machine */
static int Next_alloc;  /* Index of next element of the array */

#define SSIZE 32

static NFA *Sstack[SSIZE];     /* Stack used by cnew() */
static NFA **Sp = &Sstack[-1]; /* Stack pointer */

#define STACK_OK()                                                             \
  (INBOUNDS(Sstack, Sp)) /* true if stack not full or empty                    \
                          */

// could use second pointer subtract the first level pointer, and get the
// quantity between the two fo them
#define STACK_USED() ((Sp - Stack) + 1)) /* slots used */
#define CLEAR_STACK() (Sp = Sstack - 1)  /* reset the stack */
#define PUSH(x) (*++Sp = (x))            /* put x on stack */
#define POP() (*Sp--)                    /* get x from stack */

/* uses a node from the stack if one is available*/
static NFA *cnew() {
  NFA *p;
  static int first_time = 1;

  // this alloc executed only once
  if (first_time) {
    if (!(Nfa_states = (NFA *)calloc(NFA_MAX, sizeof(NFA))))
      parse_err(E_MEM);
    first_time = 0;
    Sp = &Sstack[-1];
  }
  if (++Nstates >= NFA_MAX)
    parse_err(E_LENGTH);
  /* If the stack is not ok,it's empty */
  // the Next_alloc varibale is initialized to zero
  p = !STACK_OK() ? &Nfa_states[Next_alloc++] : POP();
  p->edge = EPSILON;
  return p;
}

// pushes a pointer to the discarded node onto a stack
static void discard(NFA *nfa_to_discard) {
  --Nstates;
  memset(nfa_to_discard, 0, sizeof(NFA));
  nfa_to_discard->edge = EMPTY;
  PUSH(nfa_to_discard);
  if (!STACK_OK())
    parse_err(E_STACK);
}

/* Making Strings an int pointer takes care of the alignment problem at the cost
 * of a little wasted space-the size of the region used to store the string
 * itself must be rounded up to an even multiple of the int size */
static int *Strings; /* Place to save accepting string */
static int *Savep;   /* Current position in Strings array */

/* String-management function, passed a pointer to a string, and return a
 * pointer to a copy--in static memory--of that string. */
/* textp is used for copying the string component */
static char *save(char *str) {
  // textp is used to copy the string component
  char *textp, *startp;
  int len;
  static int first_time = 1;

  // initialized only once
  if (first_time) {
    /* STR_MAX: Total space that can be used by the accept string */
    if (!(Savep = Strings = (int *)malloc(STR_MAX)))
      parse_err(E_MEM);
    first_time = 0;
  }

  /* the test is for lines starting with a vertical bar, which say that the
   * action for the next line should be used for the current rule. No strings
   * are copied in this situation.
   */
  // the Savep[0] save line number
  if (*str == '|')
    return (char *)(Savep + 1);

  // the line number is put into the string
  *Savep++ = Lineno;

  for (textp = (char *)Savep; *str; *textp++ = *str++) {
    if (textp >= (char *)(Strings) + (STR_MAX - 1)) {
      parse_err(E_STRINGS);
    }
  }
  *textp++ = '\0';

  /* Increment Savep past the text."len" is initialized to the string
   * length. The "len/sizeof(int)" truncates the size down to an
   * even multiple of the current int size. The "+(len % sizeof(int) != 0)"
   * add 1 to the truncated size if the string length isn't an even multiple of
   * the int size (the !=operator evaluates to 1 or 0). Return a pointer to
   * string itself
   */
  startp = (char *)Savep;
  len = textp - startp;

  // move Savep
  Savep += (len / sizeof(int)) + (len % sizeof(int) != 0);
  return startp;
}

/*--------------------------------------------------
 * MACRO support
 */

#define MAC_NAME_MAX 34 /* Maximum name length */
#define MAC_TEXT_MAX 80 /* Maxium amount of expansion text */

typedef struct {
  char name[MAC_NAME_MAX];
  char text[MAC_TEXT_MAX];
} MACRO;

static HASH_TAB *Macros; /* Symbol table for macro definitions */

void new_macro(char *def) {
  /* Add a new macro to the table. If two macros have the same name, the
   * Second one takes precedence. A definitions takes the form:
   *                 name <whitespace> text [<whitespace>]
   * whitespace at the end of the line is ignored.
   */

  char *name; /* Name component of macro definition */
  char *text; /* text part of macro definition */
  char *edef; /* pointer of end of text part */
  MACRO *p;
  static int first_time = 1;

  if (first_time) {
    first_time = 0;
    /* use sample_cmp to substitute strcmp. */
    Macros = maketab(31, hash_add, sample_cmp);
  }

  for (name = def; *def && !std::isspace(*def); def++)
    ; /* Isolate name */

  if (*def)
    *def++ = '\0';

  /* Isolate the definition text. This process is complicated because you need
   * to display any trailing whitespace on the line. The first while loop
   * skips the preceding whitespace. The for loop is looking for end of string .
   * If you find a white character (and the \n at the end of string is white),
   * remember the position as a potential end of string
   */

  while (std::isspace(*def)) /* Skip up to macro body */
    ++def;

  text = def; /* Remember start of replacement text */

  edef = NULL;
  while (*def) {
    if (!isspace(*def))
      ++def;
    else
      for (edef = def++; std::isspace(*def); ++def)
        ;
  }

  if (edef)
    *edef = '\0';

  /* Add the macro to the symbol table */
  p = (MACRO *)newsym(sizeof(MACRO));
  std::strncpy(p->name, name, MAC_NAME_MAX);
  std::strncpy(p->text, text, MAC_TEXT_MAX);
  addsym(Macros, p);
}

static char *expand_macro(char **namep) {
  /* Return a pointer to the contents of a macro having the indicated
   * name. Abort with a message if no macro exists. The macro name includes
   * the brackets, which are destroyed by the expansion process. *namep
   * is modified to point past the close brace
   */

  char *p;
  MACRO *mac;

  /* find the position when a character first show in the string. */
  if (!(p = std::strchr(++(*namep), '}'))) /* skip { and find } */
    parse_err(E_BADMAC);                   /* print msg & abort */
  else {
    *p = '\0'; /* Overwrite close brace. */

    if (!(mac = (MACRO *)findsym(Macros, *namep))) {
      // printf("%s\n",*namep);
      // printf("%p\n",mac);
      parse_err(E_NOMAC);
    }
    *p++ = '}'; /* put the brace back */

    *namep = p; /* Update name pointer */
    return mac->text;
  }

  return (char *)"ERROR"; /* If you get here, it's a bug */
}

/* Workspace function needed by ptab() call in printmacs(), below*/
static void print_a_macro(void *mac, void *ignore) {
  MACRO *a1 = (MACRO *)mac;
  printf("%-16s--[%s]--\n", a1->name, a1->text);
}

void printmacs() { /* Print all the macros to stuout */
  if (!Macros)
    printf("\tThere are no macros\n");
  else {
    printf("\nMACROS:\n");
    ptab(Macros, print_a_macro, NULL, 1);
  }
}

/*-------------------------------------------------------------------
 * Lexical analyzer:
 *
 * Lexical analysis is trivial because all lexemes are single-character values.
 * The only complications are escape sequences and quoted strings, both of which
 * are handled by advance(), below. This routine advances past the current
 * token, putting the new token into Current_tok and the equivalent Lexeme into
 * Lexeme. If the character was escaped, Lexeme holds the actual value. For
 * example, if a "\s" is encountered, Lexeme will hold a space character. The
 * MATCH(x) macro return true if x matches the current token. Advance both
 * modifies Current_tok to the current token and returns it
 */

static int advance();
static void cat_expr(NFA **, NFA **);
static void discard(NFA *);
static void dodash(SET *);
static void expr(NFA **, NFA **);
static void factor(NFA **, NFA **);
static int first_in_cat(TOKEN);
static NFA *machine();
static NFA *cnew();
// PRIVATE NFA parse_err(ERR_NUM);
static NFA *rule();
static void term(NFA **, NFA **);

static int advance() {
  /* advance the input by one character */
  static int inquote = 0;    /* Processing quoted string */
  int saw_esc;               /* Saw a backslash */
  static char *stack[SSIZE], /* Input-source stack */
      **sp = NULL;           /* and stack pointer */

  if (!sp)          /* Initialized sp. */
    sp = stack - 1; /* Necessary for large model  */

  if (Current_tok == EOS) /* if the end of the previous line has been reached.
                             Get another line */
  {
    if (inquote)
      parse_err(E_NEWLINE);
    do {
      /* get a complete rule and action */
      if (!(Input = (*Ifunct)())) /* End of file */
      {
        Current_tok = END_OF_INPUT;
        goto exit;
      }
      while (std::isspace(*Input)) /* Ignored leading */
        Input++;                   /* white space... */
    } while (!*Input);

    S_input = Input; /* Remember start of line */
  }                  /* for error messages. */

  while (*Input == '\0') /* macro expansion */
  {
    /* is activated when you get to the end of the replacement text */
    if (INBOUNDS(stack, sp)) /* Restore previous input source  */
    {
      Input = *sp--;
      continue;
    }

    /* get here only if the stack is empty and no more characters are found in
     * the current input source, in which case end of string has been reached.
     */
    Current_tok = EOS; /* No more input sources to restore  */
    Lexeme = '\0';     /* ie. you're at the real end of */
    goto exit;         /* string */
  }

  if (!inquote) {
    while (*Input == '{') /* Macro expansion required  */
    {
      /* nested macros are handled with a stack. When an inner macro is
       * encounter, the current input buffer is pushed onto a stack, and Input
       * is modified to point at the macro-replacement text */
      *++sp = Input;            /* Stack current input string */
      Input = expand_macro(sp); /* and replace it with the macro body*/

      if (TOOHIGH(stack, sp))
        parse_err(E_MACDEPTH); /* Stack overflow */
    }
  }

  if (*Input == '"') {
    /* recognize quotes and inquote is set to true when precessing a quoted
     * string */
    inquote = ~inquote;
    /* when encounter '"' Input + 1 */
    if (!*++Input) {     /* At either start and end of a quoted */
      Current_tok = EOS; /* string. All characters are treated as literals */
      Lexeme = '\0';     /* while inquote is true */
      goto exit;
    }
  }

  saw_esc = (*Input == '\\');

  /* normal text which doesn't in double quotes */
  if (!inquote) {
    /* eos, which marks the end of the regular expression, is returned if any
     * white space is encountered, and escape sequences are expanded by the
     * esc(). */
    if (std::isspace(*Input)) {
      Current_tok = EOS;
      Lexeme = '\0';
      goto exit;
    }
    Lexeme = esc(&Input);
  } else {
    if (saw_esc && Input[1] == '"') {
      Input += 2;
      Lexeme = '"';
    } else {
      Lexeme = *Input++;
    }
  }

  Current_tok = (inquote || saw_esc ? L : Tokmap[Lexeme]);
exit:
  return Current_tok;
}

/*---------------------------------------------------------------------
 * The Parser: A simple recursive descent parser that creates a Thompson NFA
 * for a regular expression. The access routine [thompson()] ia at the bottom.
 * The NFA is created as a directed graph, with each node containing pointer's
 * to the next node. Since the structures are allocated from an array, the
 * machine can also be considered as an array where the state number is the
 * arrray index
 */

static NFA *machine() {
  /* collect a series of rules and chains them together using epsilon transition
   * and dummy states. */
  NFA *start;
  NFA *p;

  ENTER("Machine");

  p = start = cnew();
  p->next = rule();

  while (!MATCH(END_OF_INPUT)) {
    p->next2 = cnew();
    p = p->next2;
    p->next = rule();
  }

  LEAVE("Machine");
  return start;
}

static NFA *rule() {
  /* gets a single regular expression and associated action
   * rule   -->  expr  EOS action
   *             ^expr EOS action
   *             expr$ EOS action
   *
   * action  --> <tabs> <string of character>
   *             epsilon
   */

  NFA *start = NULL;
  NFA *end = NULL;
  int anchor = NONE;

  ENTER("rule");

  if (MATCH(AT_BOL)) {
    /* an extra node is created with an outgoing transition an a newline so that
     * ^ is treated as if it were a regular expression that matched a newline */
    start = cnew(); // Beginning and end-of-line anchors are processed
    start->edge = '\n';
    anchor |= START;
    advance();
    expr(&start->next, &end);
  } else
    expr(&start, &end);

  if (MATCH(AT_EOL)) {
    /* pattern followed by a carriage-return or linefeed (use a character class)
     */

    advance();
    end->next = cnew();
    end->edge = CCL;

    if (!(end->bitset = newset()))
      parse_err(E_MEM);

    ADD(end->bitset, '\n');

    if (!Unix)
      ADD(end->bitset, '\r');

    end = end->next;
    anchor |= END;
  }

  while (std::isspace(*Input))
    Input++;

  /* the Input is positioned at the start of the accepting action (remember, an
   * entire multiple-line action is collected into the input string) */
  end->accept = save(Input);
  end->anchor = anchor;
  advance(); /* skip past EOS */

  LEAVE("rule");
  return start;
}

static void expr(NFA **startp, NFA **endp) {
  /* Because a recursive descent compiler can't handle left recursion,
   * the productions:
   *
   *  expr  -> expr OR cat_expr
   *         | cat_expr
   *
   * must be translated into:
   *
   *  expr  -> cat_expr exor'
   *  expr' -> OR cat_expr expr'
   *           epsilon
   *
   * which can be implemented with this loop:
   *
   *  cat_expr
   *  while(match(OR))
   *         cat_expr
   *         do the OR
   */

  NFA *e2_start = NULL; /* expression to right of \ */
  NFA *e2_end = NULL;
  NFA *p;

  ENTER("expr");

  /* create a machine and the input is advanced to the OR operator */
  cat_expr(startp, endp);

  while (MATCH(OR)) {
    advance();
    /* create another machine */
    cat_expr(&e2_start, &e2_end);

    p = cnew();
    p->next2 = e2_start;
    p->next = *startp;
    *startp = p;

    p = cnew();
    (*endp)->next = p;
    e2_end->next = p;
    *endp = p;
  }
  LEAVE("expr");
}

/*----------------------------------------------------------*/

static void cat_expr(NFA **startp, NFA **endp) {
  /* The same translations that were needed in the expr rules are needed again
   * here:
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

  NFA *e2_start, *e2_end;

  ENTER("cat_expr");

  if (first_in_cat(Current_tok))
    /* concatenation start. The first operand is fetched with this factor call()
     */
    factor(startp, endp);

  while (first_in_cat(Current_tok)) {
    /* The second and subsequent operand are fetched with this operand call() */
    factor(&e2_start, &e2_end);
    /* The two are concatenated by overwriting the contents of the end node of
     * the first operand with the contents of the starting node of the second
     * operand. */
    std::memcpy(*endp, e2_start, sizeof(NFA));
    /* The discarded node is picked up again in the next iteration of the loop
     * because it will be at the top of the pushback stack. */
    discard(e2_start);

    /* updated the endp */
    *endp = e2_end;
  }

  LEAVE("cat_expr");
}

static int first_in_cat(TOKEN tok) {
  /* Concatenation is a somewhat harder problem because there's no operator to
   * look for. The problem is solved by this routine which looks to see if the
   * next input token can reasonably be concatenated to the current one.
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
    parse_err(E_CLOSE);
    return 0;
  case CCL_END:
    parse_err(E_BRACKET);
    return 0;
  case AT_BOL:
    parse_err(E_BOL);
    return 0;
  }

  return 1;
}

static void factor(NFA **startp, NFA **endp) {
  /* factor  --> term* | term+ | term?
   */

  NFA *start, *end;

  ENTER("factor");
  term(startp, endp);

  if (MATCH(CLOSURE) || MATCH(PLUS_CLOSE) || MATCH(OPTIONAL)) {
    start = cnew();
    end = cnew();
    start->next = *startp;
    (*endp)->next = end;

    if (MATCH(CLOSURE) || MATCH(OPTIONAL)) /*   * or ?   */
      start->next2 = end;

    if (MATCH(CLOSURE) || MATCH(PLUS_CLOSE)) /*   * or +  */
      (*endp)->next2 = *startp;

    *startp = start;
    *endp = end;
    advance();
  }

  LEAVE("factor");
}

static void term(NFA **startp, NFA **endp) {
  /* Process the term productions:
   *
   * term  --> [...] | [^...] | [] | [^] | . | (expr) | <character>
   *
   * the [] is nonstandard. It matches a space, tab, formfeed, or newline,
   * but not a carriage return (\r). All of these are single nodes in the NFA
   */

  NFA *start;
  int c;

  ENTER("term");

  if (MATCH(OPEN_PAREN)) {
    advance();
    expr(startp, endp);
    if (MATCH(CLOSE_PAREN))
      advance();
    else
      parse_err(E_PAREN);
  } else {
    /* The actual NFA structure are allocated and connected. */
    *startp = start = cnew();
    *endp = start->next = cnew();

    if (!(MATCH(ANY) || MATCH(CCL_START))) {
      /* if doesn't encounter . and [ */
      start->edge = Lexeme;
      advance();
    } else {
      start->edge = CCL;

      if (!((start->bitset) = newset()))
        parse_err(E_MEM);

      if (MATCH(ANY)) /*  dot(.)  */
      {
        ADD(start->bitset, '\n');
        if (!Unix)
          ADD(start->bitset, '\r');

        CCOMPLEMENT(start->bitset);
      } else {
        advance();
        if (MATCH(AT_BOL)) /* Negative character class */
        {
          advance();

          ADD(start->bitset, '\n'); /* Don't include \n in class */
          if (!Unix)
            ADD(start->bitset, '\r');

          CCOMPLEMENT(start->bitset);
        }
        if (!MATCH(CCL_END))
          dodash(start->bitset);
        else /* [] or [^] */
          for (c = 0; c <= ' '; ++c)
            ADD(start->bitset, c);
      }
      advance();
    }
  }

  LEAVE("term");
}

static void dodash(SET *set) /*  Pointer to ccl character set */
{
  /* convert the input tokens representing the class into a SET. A set can have
   * one element ([x]) or servel elements [a-zA-Z] */
  int first;

  for (; !MATCH(EOS) && !MATCH(CCL_END); advance()) {
    if (!MATCH(DASH)) {
      first = Lexeme;
      ADD(set, Lexeme);
    } else {
      advance();
      for (; first <= Lexeme; first++)
        ADD(set, first);
    }
  }
}

NFA *thompson(char *(*input_function)(), int *max_state, NFA **start_state) {
  /* Access routine to this module. Return a pointer to NFA transition table
   * that represets the regular expression pointed to by expr or NULL if there's
   * not enough memory. Modify *max_state to reflect the largest state number
   * used. This number will probably be a larger number than the total number of
   * states. Modify *start_state to point to the start state. This pointer is
   * garbage if thompson return 0. The memory for the table is fetched from
   * malloc(); use free() to discard it.
   */
  CLEAR_STACK();

  Ifunct = input_function;

  Current_tok = EOS; /* Load first token */
  advance();

  Nstates = 0;
  Next_alloc = 0;

  *start_state = machine(); /* Manufacture the NFA */
  *max_state = Next_alloc;  /* Max state # in NFA */

  if (Verbose > 1)
    print_nfa(Nfa_states, *max_state, *start_state);

  if (Verbose) {
    printf("%d/%d NFA states used.\n", *max_state, NFA_MAX);
    printf("%ld/%d bytes used for accepting strings.\n\n", Savep - Strings,
           STR_MAX);
  }
  return Nfa_states;
}
