/*GLOBALS.H:Global variables shared between modules*/
#pragma once
#include <cstdio>
#ifdef ALLOC
#define CLASS
#define I(x) x
#else
#define CLASS extern
#define I(x)
#endif

#define MAXINP 2048 /*Maxium rule size*/

/* Verbose, No_lines, Unix, Public, Template are set by  command-line switches.
 */
extern int Verbose;          /* Print statistics */
extern int No_lines;         /* Suppress #line directives */
extern int Unix;             /* Use UNIX-style newlines */
extern int Public;           /* Make static symbols public */
extern const char *Template; /* State-machine driver template */
extern int Actual_lineno;    /* Current input line number */
extern int Lineno; /* Line number of first line of a multiple-line rule */

extern char Input_buf[MAXINP]; /* Line buffer for input */
extern const char *Input_file_name;  /* Input file name */
extern FILE *Ifile;            /* Input stream */
extern FILE *Ofile;            /* Output stream */
#undef CLASS
#undef I
