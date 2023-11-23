#include "../tool/compiler.h"
#include "../tool/debug.h"
PUBLIC void defnext(FILE *fp, char *name) {
  /* Print the default yy_next(s,c) subroutines for an uncompressed table */
  static char *comment_text[] = {
      "yy_next(state,c) is given the current state and input character and",
      "evaluates to the next state", NULL};

  comment(fp, comment_text);
  fprintf(fp, "#define yy_next(state,c) %s[state][c]\n", name);
}

