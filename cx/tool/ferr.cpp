#include "../tool/debug.h"
#include "../tool/l.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
int ferr(char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  if (fmt)
    prnt(fputc, stderr, fmt, args);
  else
    perror(va_arg(args,char*));
  va_end(args);
  exit(0);
}
