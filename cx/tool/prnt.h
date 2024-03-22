#ifndef _PRNT_H
#define _PRNT_H
#include "../tool/debug.h"
#include <cstdarg>
#include<cstdio>
void prnt(int (*prnt_t)(int, FILE*), FILE *funct_arg, char *format, va_list args);
void stop_prnt(void);
#endif
