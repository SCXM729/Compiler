#include "../tool/debug.h"
#include "globals.h"
#include <cctype>
#include <cstdio>

/* INPUT.CPP Lowesr-level input functions */

PUBLIC char *get_expr() {
  /* Input routine for nfa(). Gets a regular expression and the associated
   * string form the input stream. Returns a pointer to the input string
   * normally. Return NULL on end of file or if a line beginning with % is
   * encounter. All blank line are discarded and all lines that start with
   * whitespace are concatenated to the previous line. The global variable
   * Lineno is set to the line number of the top line of a multiple-line
   * block. Actual_lineno holds the real line number
   */

  static int lookahead = 0;
  int space_left;
  char *p;

  p = Input_buf;
  space_left = MAXINP;
  if (Verbose > 1) {
    printf("b%d: ", Actual_lineno);
  }

  if (lookahead == '%') /* next line starts with a % sign */
    return NULL;        /* return End-of-input marker  */

  Lineno = Actual_lineno;

  while ((lookahead = getline(&p, space_left - 1), Ifile) != EOF) {
    if (lookahead == 0)
      lerror(1, "Rule too long");

    Actual_lineno++;

    if (!Input_buf[0]) /* Ignore blank lines */
      continue;

    space_left = MAXINP - (p - Input_buf);

    if (!isspace(lookahead))
      break;

    *p++ = '\n';
  }

  if (Verbose > 1)
    printf("%s\n", lookahead ? Input_buf : "---EOF---");

  return lookahead ? Input_buf : NULL;
}

/*---------------------------------------------------------------------*/

PRIVATE int getline(char **stringp, FILE *stream) {
  /* Gets a line of input, Gets at most n-1 characters. Updates *stringp
   * to point at the '\0' at the end of the string. Return a lookahead
   * character (the character that follows the \n in the input). The '\n'
   * is not put into the string.
   *
   * Return the character following the \n normally
   *   EOF at end of file
   *   0   if the line is too long
   */

  static int lookahead = 0;
  char *str, *startstr;

  startstr = str = *stringp;

  if (lookahead == 0) /* initialize */
    lookahead = getc(stream);

  if (n > 0 && lookahead != EOF) {
    while (--n > 0) {
      *str = lookahead;
      lookahead = getc(stream);

      if (*str == '\n' || *str == EOF)
        break;
      ++str;
    }
    *str = '\0';
    *stringp = str;
  }
  return (n <= 0) ? 0 : lookahead;
}
