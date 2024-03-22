#ifndef _COMPILER_H
#define _COMPILER_H
#include <cstdarg>
#include <cstdio>

void defnext(FILE *fp, const char *name);

// conversion function
int hex2bin(int c);
int oct2bin(int c);
int esc(char **s);
char *bin_to_ascii(int c, int use_hex);

// print function
int ferr(const char *fmt, ...);
int on_ferr();
void fputstr(char *str, int maxlen, FILE *stream);
void pchar(int c, FILE *stream);
void printv(FILE *fp, const char *argv[]);
void comment(FILE *fp, const char **argv);
void prnt(int (*ofunct)(int, FILE *), FILE *funct_arg, const char *format,
          va_list args);
void stop_prnt();

int copyfile(const char *dst, const char *src, const char *mode);
int movefile(const char *dst, const char *src, const char *mode);
int *memiset(int *dst, int with_what, int count);
int concate(int size, char *dst, ...);
void searchenv(const char *filename, const char *envname, char *pathname);
FILE *driver_1(FILE *ouput, int lines, const char *file_name);
int driver_2(FILE *output, int lines);

// \src\tool\ssort.cpp
void ssort(char *base, int numele, int elsize, int (*cmp)(void *, void *));
void assort(void **base, int numele, int elsize, int (*cmp)(void **, void **));

// \src\tool\mean.cpp
double mean(int reset, double sample, double *dev);

// pairs.cpp
int pairs(FILE *fp, int *array, int nrows, int ncols, const char *name,
          int threshold, int numbers);
void pnext(FILE *fp, const char *name);

// print_ar.cpp
void print_array(FILE *fp, int *array, int nrows, int ncols);

void unixfy(char *str);

// \src\tool\stol.cpp
unsigned long cstoul(char **instr);
long cstol(char **instr);

#endif
