#include <cstdio>
#include <cstdlib>
#ifdef MSDOS
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include "../tool/debug.h"
#include "globals.h"
#include "nfa.h"
#include <cctype>
#include <cstring>

#ifdef DEBUG
int Lev = 0;
#define ENTER(f) printf("%*senter %s [%c][%1.10s]\n",
Lev++ * 4,"",f,Lexeme,Input)
#define LEAVE(f) printf("%*sleave %s [%c][%1.10s]\n",
Lev-- * 4,"",f,)
#else
#define ENTER(f)
#define LEAVE(f)
#endif


/*------------------------------------------------------
* Error processing stuff.Note that all errors are fatal
* -----------------------------------------------------
*/

typedef enum _err_num_{
  E_MEM,          /*Out of memory*/
  E_BADEXPR,      /*Malformed regular expression*/
  E_PAREN,        /*Missing close parenthesis*/
  E_STACK,        /*Internal error: Discard stack full*/
  E_LENGTH,       /*Too many regular expressions*/
  E_BRACKET,      /*Missing [ in character class*/
  E_BOL,          /*^ must be at start of expr or ccl*/
  E_CLOSE,        /* + ? or * must follow expression*/
  E_STRINGS,      /*Too many characters in accept actions*/
  E_NEWLINE,      /*Newline in quoted string*/
  E_BADMAC,       /*Missing } in macro expansion*/
  E_NOMAC,        /*Macro doesn't exist*/
  E_MACDEPTH      /*Macro expansion nested too deeply*/
} ERR_NUM;

PRIVATE const char *Errmsgs[] = {
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
    "Macro doesn't exist",
    "Macro expansions nested too deeply",
};

PRIVATE void parse_err(ERR_NUM type) {
  fprintf(stderr, "ERROR (line %d) %s \n%s\n", Actual_lineno,
          Errmsgs[(int)type], S_input);
  while (++S_input <= Input)
    putc('_', stderr);
  fprintf(stderr, "^\n");
  exit(1);
}

PRIVATE NFA *Nfa_states; /*State-machine array*/
PRIVATE int Nstates = 0; /*#of NFA states in machine*/
PRIVATE int Next_alloc;  /*Index of next element of the array*/

#define SSIZE 32

PRIVATE NFA *Sstack[SSIZE];     /*Stack used by new()*/
PRIVATE NFA **Sp = &Sstack[-1]; /*Stack pointer*/

#define STACK_OK() (INBOUND(Sstack, Sp)) /*true if stack not full or empty*/

#define STACK_USED() ((Sp - Stack) - 1)) /*slots used*/
#define CLEAR_STACK() (Sp = Sstack - 1)  /*reset the stack*/
#define PUSH(x) (*++Sp = (x))            /*put x on stack*/
#define POP() (*Sp--)                    /*get x from stack*/

/*-----------------------------------*/
/*Making Strings an int pointer takes care of the alignment problem at the cost
 * of a little wasted space-the size of the region used to store the string
 * itself must be rounded up to an even multiple of the int size*/
PRIVATE int *Strings; /*Place to save accepting string*/
PRIVATE int *Savep;   /*Current position in Strings array*/
/*----------------------------------*/

/* uses a node from the stack if one is available*/
PRIVATE NFA *new() {
  NFA *p;
  static int first_time = 1;

  if (first_time) {
    if (!(Nfa_states = (NFA *)calloc(NFA_MAX, sizeof(NFA))))
      parse_err(E_MEM);
    first_time = 0;
    Sp = &Sstack[-1];
  }
  if (++Nstates >= NFA_MAX)
    parse_err(E_LENGTH);
  /*If the stack is not ok,it's empty*/
  p = !STACK_OK() ? &Nfa_states[Next_alloc++] : POP();
  p->edge = EPSILON;
  return p;
}

// pushes a pointer to the discarded node onto a stack
PRIVATE void discard(NFA *nfa_to_discard) {
  --Nstates;
  memset(nfa_to_discard, 0, sizeof(NFA));
  nfa_to_discard->edge = EMPTY;
  PUSH(nfa_to_discard);
  if (!STACK_OK())
    parse_err(E_STACK);
}

PRIVATE char *save(char *str) { /*String-management function*/
  /*textp is used for copying the string component*/
  char *textp, *startp;
  int len;
  static int first_time = 1;

  if (first_time) {
    if (!(Savep = Strings = (int *)malloc(STR_MAX)))
      parse_err(E_MEM);
    first_time = 0;
  }

  if (*str == '|')
    return (char *)(Savep + 1);

  *Savep++ = Lineno;

  for (textp = (char *)Savep; *str; *textp++ = *str++) {
    if (textp >= (char *)(Strings + (STR_MAX - 1)))
      parse_err(E_STRINGS);
  }
  *textp++ = '\0';

  /*Increment Savep past the text."len" is initialized to the string
   * length. The "len/sizeof(int)" truncates the size down to an
   * even multiple of the current int size. The "+(len % sizeof(int) != 0)"
   * add 1 to the truncated size if the string length isn't an even multiple of
   * the int size (the !=operator evaluates to 1 or 0). Return a pointer to
   * string itself
   */
  startp = (char *)Savep;
  len = textp - startp;
  Savep += (len / sizeof(int)) + (len % sizeof(int) != 0);
  return startp;
}

/*--------------------------------------------------
 * MACRO support
 */

#define MAC_NAME_MAX 34 /*Maximum name length*/
#define MAC_TEXT_MAX 80 /*Maxium amount of expansion text*/

typedef struct {
  char name[MAC_NAME_MAX];
  char text[MAC_TEXT_MAX];
} MACRO;

PRIVATE HASH_TAB *Macros;
