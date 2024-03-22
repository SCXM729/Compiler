#include "prnt.h"
#include "debug.h"
#include <cstdio>
// glue formatting workspace functions to various environment.
static FILE *Tmp_file = NULL;
static char *Tmp_name;
void prnt(int (*prnt_t)(int, FILE*), FILE *funct_arg, char *format,
          va_list args) {
  char buf[256], *p;
  vsprintf(buf, format, args);
  for (p = buf; *p; ++p) {
    (*prnt_t)(*p, funct_arg);
  }
}

void stop_prnt() {}
