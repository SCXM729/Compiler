#pragma once
#include <cstdio>
double mean(int reset, double sample, double *dev);
int esc(const char *&s);
void testfunc();
char *bin_to_ascii(int c, int use_hex);

void pchar(int c, FILE *stream);
