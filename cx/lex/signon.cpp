#include "dfa.h"
#include <stdio.h>

void signon() {
  /* Print the sign-on message. Since the console is opened explicitly, the
   * message is printed even if both stdout and stderr are redirected.
   */

  /* The ANSI __DATE__ macro yields a string of the form: "Sep 01 1989".
   * The __DATE__+7 gets the year portion of that string.
   *
   * The UNIX __DATE__ is created from the makefile with this command:
   *		echo \#define __DATE__ \"`date`\" >> date.h
   * which evaluates to:
   *		#define __DATE__ "Sun Apr 29 12:51:50 PDT 1990"
   * The year is at __DATE__+24
   */

  // fprintf(screen, "LeX %s [%s]. (c) %s, Allen I. Holub.", VERSION, __DATE__,
  //         ANSI(__DATE__ + 7) KnR(__DATE__ + 24));
  fprintf(stdout, "LeX %s [%s]. (c) %s, sheep.", "1.1", "2023.12", "");
  fprintf(stdout, " All rights reserved.\n");

}
