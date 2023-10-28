#ifndef __DEBUG_H
#define __EDBUG_H

#ifdef DEBUG
#define PRIVATE
#define D(x) x /*expand ony when debugging*/
#define ND(x)  /*expand only when not debugging*/
#else
#define PRIVATE static
#define D(x)
#define ND(x) x
#endif

#define PUBLIC
#ifdef __TURBOC__ /*Compiling for Trubo/Botland C/C++*/
#define BCC(x) x  /*BXX(x) expands to its argument*/
#define MSC(x)    /*these expands to empty string*/
#define MSC5(x)
#define MSC6(x)
#define MSC7(x)
#define UNIX(x)
#define ANSI(x)
#define _8086
#define FARPTR far *
#define VA_LIST
#define MSDOS
#else
#if (defined(MSDOS) || defined(_MSDOS))
#ifndef MSDOS
#define MSDOS 1
#endif

#if (defined(_MSC_VER) && _MSC_VER == 700)
#progma message("/*Compiling for Microsoft C,Ver.7.x*/")
#define FARPTR _far *
#define MSC5(x)
#define MSC6(x)
#define MSC7(x)
#elif (defined(MSC_VER) && (MSC_VER == 600))
#pragma message("/*Compiling for Microsoft C,Ver.6.x*/")
#define FARPTR *far
#define MSC5(x)
#define MSC6(x)
#define MSC7(x)
#else
#pragma message("/*Compiling for Microsoft C.Ver.5.x*/")
#define FARPTR far *
#define MSC5(x)
#define MSC6(x)
#define MSC7(x)
#endif

#define BCC(x) /*all version*/
#define MSC(x) x
#define UNIX(x)
#define ANSI(x)
#define _8086
#define VA_LIST
#else /*non-ansi(ie.UNIX)compiler*/
#define BCC(x)
#define MSC(x)
#define MSC5(x)
#define MSC6(x)
#define MSC7(x)
#define UNIX(x) x
#define ANSI(x)
#define FARPTR *
#define VA_LIST
#define O_BINARY 0 /*No binary input mode in UNIX open()*/
#define far        /*Microsoft/Turbo keyword for an 8086*/
/*32-bit,far pointer.Ignore in UNIX.*/
// #define const      /*Ignore ANSI const and volatile keyword*/
#define volatile
// #define memmove(d, s, n) bcopy(s, d, n)
// #define memcpy(d, s, n) bcopy(s, d, n)
// extern long getpid();
// #define raise(sig) kill((int)getpid(), sig)
#include <cstdio>
#define vfprintf(stream, fmt, argp) _doprnt(fmt, argp, stream)
#define vprintf(fmt, argp) _doprnt(fmt, argp, stdout)
typedef long time_t;   /*for the VAX,may have to change this*/
extern char *strdup(); /*You need to supply one*/
                       // typedef int void;
#endif /*MSODS*/
#endif /*__TURBOC__*/

#if (0 ANSI(+1))
#define KnR(x)
#define P(x) x /*function prototypes supported*/
#else
#define KnR(x) x
#define P(x) x /*Otherwise,discard argument lists and*/
// #define void char /*translate void keyword to int*/
#endif

#if (0 MSC(+1) BCC(+1)) /*Microsoft or Borland Compiler*/
#define MS(x) x
#else
#define MS(x)
#endif

/*SEG(p) Evaluates to the segment portion of an 8086 address
 * OFF(p) Evaluates to the offset portion of an 8086 address
 * PHYS(P) Evaluates to a long holding a physical address
 */
#ifdef _8086
#define SEG(p) (((unsigned *)&(p))[1])
#define OFF(p) (((unsigned *)&(p))[0])
#define PHYS(p) (((unsigned long)OFF(p)) + ((unsigned long)SEG(p) << 4))
#else
#define PHYS(p) p
#endif
#endif

/*NUMELE(array) Evaluates to thhe array size in elements
 * LASTELE(array) Evaluates to a pointer to the last element
 * INBOUNDS(array,p) Evaluates to pointer to last element
 * RANGE(a,b,c)  Evaluates to true if a<=b<=c
 * max(a,b) Evalates to a or b,which ever is larger
 * min(a,b) Evalates to a or b,which ever is smaller
 * associated with a pointer
 * NBITS(type) Return number of bits in a variable of the indicated type
 * MAXINT Evaluates to the value of the largest signed integer
 */

#define NUMELE(a) (sizeof(a) / sizeof(*(a)))
#define LASTELE(a) ((a) + (NUMELE(a) - 1))
#define TOOHIGH(a, p) ((p) - (a) > (NUMELE(a) - 1))
#define TOOLOW(a, p) ((p) - (a) < 0)
#define INBOUNDS(a, p) (!(TOOHIGH(a, p) || TOOLOW(a, p)))
/*Portability note:Some systems won't allow UL for unsigned long in the _IS
 * macro.You can use the following if so:
 *
 *(unsigned long) 1
 *
 *Bob Muller,who suggested the foregoing,also reports:
 *"The also seems to be an issue with the notion of shifting;the DEC Ultrix"
 *compiler,for example ,says that (unsigned long)((unsigned long)1<<32)==1
 *while the SunOs 4 compiler says that it is 0
 */
