#include "../tool/compiler.h"
#include "dfa.h"
#include "globals.h"
#include "nfa.h"
#include <cstddef>
#include <cstdio>
#include <cstring>

/* print.cpp: This module contains miscellaneous print routines that
 * do everything except print the actual tables
 */

void pheader(FILE *fp, ROW dtran[], int rows, ACCEPT *accept);
void pdriver(FILE *output, int rows, ACCEPT *accept);

/*----------------------------------------------------------------*/

void pheader(FILE *fp, ROW dtran[], int nrows, ACCEPT *accept) {
  /* fp: output stream
   * dtran: DFA transition table
   * nrows: Number of states in dtran[]
   * accept: Set of accept states in dtran[]
   */

  /* print out a header comment that describes the uncompressed DFA */
  int i, j;
  int last_transition;
  int chars_printed;

  fprintf(fp, "#ifdef _NEVER_\n");
  fprintf(fp, "/*---------------------------------------*/");
  fprintf(fp, "* DFA (start state is 0) is:\n*\n");

  for (i = 0; i < nrows; i++) {
    if (!accept[i].string)
      fprintf(fp, " * State %d [nonaccepting]", i);
    else {
      fprintf(fp, " * State %d [accepting, line %d] <", i,
              ((int *)(accept[i].string))[-1]);

      fputstr(accept[i].string, 20, fp);
      fprintf(fp, ">]");

      if (accept[i].anchor)
        fprintf(fp, " Anchor: %s%s", accept[i].anchor & START ? "start " : "",
                accept[i].anchor & END ? "end" : "");
    }

    last_transition = -1;
    for (j = 0; j < MAX_CHARS; ++j) {
      if (dtran[i][j] != F) {
        if (dtran[i][j] != last_transition) {
          fprintf(fp, "\n * goto %2d on ", dtran[i][j]);
          chars_printed = 0;
        }

        fprintf(fp, "%s", bin_to_ascii(j, 1));

        if ((chars_printed += strlen(bin_to_ascii(j, 1))) > 56) {
          fprintf(fp, "\n *           ");
          chars_printed = 0;
        }

        last_transition = dtran[i][j];
      }
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, " */\n\n");
  fprintf(fp, "#endif\n");
}

void pdriver(FILE *output, int nrows, ACCEPT *accept) {
  /* nrows: Number of states in dtran
   * accept: Set of accept states in dtran
   */

  /* print the array of accepting states, the driver itself, and the case
   * statements for the accepting string.
   */
  int i;
  static const char *text[] = {
      "The Yyaccept array has two purposes, If Yyaccept[i] is 0 then state",
      "i is nonaccepting. If it's nonzero then the number determines whether",
      "the string is anchored, 1=anchored at start of line,2=at end of line,",
      "3=both,4=line not anchored", NULL};
  comment(output, text);
  fprintf(output, "YYPRIVATE YY_TTYPE Yyaccept[] =\n");
  fprintf(output, "{\n");

  for (i = 0; i < nrows; ++i) { /* accepting array */
    if (!accept[i].string)
      fprintf(output, "\t0 ");
    else
      fprintf(output, "\t%-3d", accept[i].anchor ? accept[i].anchor : 4);

    fprintf(output, "%c    /* State %-3d */\n", i == (nrows - 1) ? ' ' : ',',
            i);
  }
  fprintf(output, "};\n\n");

  driver_2(output, !No_lines); /* code above cases */

  for (i = 0; i < nrows; ++i) /* case statement */
  {
    if (accept[i].string) {
      fprintf(output, "\t\tcase %d:\t\t\t\t\t/* State %-3d */", i, i);
      if (!No_lines)
        fprintf(output, "#line %d \"%s\"\n", *((int *)(accept[i].string) - 1),
                Input_file_name);
      fprintf(output, "\t\t    %s\n", accept[i].string);
      fprintf(output, "\t\t    break;\n");
    }
  }
  driver_2(output, !No_lines); /* code below cases */
}