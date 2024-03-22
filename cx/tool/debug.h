#pragma once
#ifdef DEBUG
#define D(x) x
#else
#define PRIVATE static
#define D(x)
#endif
#define PUBLIC

#ifdef MSODS
#define MS(x) x
#define UX(x)
#define ANSI
#define _8086
#else
#define MS(x)
#define UX(x) x
#define O_BINARY 0 /*no binary input mode in UNIX open()*/
typedef long time_t; /*for the VAX,may have to change this*/
// typedef unsigned size_t; /*for the VAX,may have to change this*/
extern char *strdup(); /*You need to supply one*/
#endif

// #ifdef ANSI /*if ANSI is defined,put arg list into function prototype*/
// #define P(x) x
// #define VA_LIST ...
// #else
// #define P(x) ()
// #define void char
// #define VA_LIST _a_r_g_s
// #endif
#define P(x) x
#define VA_LIST ...

#ifdef _8086
#else
#define PHYS(p) p
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
// if sizeof(a)/sizeof(*(a)) there are warngins: "suspicious usage of
// 'sizeof(A*)'"
#define NUMELE(a) (sizeof(a) / sizeof(a[0]))
#define LASTELE(a) ((a) + (NUMELE(a) - 1))
#define TOOHIGH(a, p) ((p) - (a) > (NUMELE(a) - 1))
#define TOOLOW(a, p) ((p) - (a) < 0)
/* (a[0], a[9]) */
#define INBOUNDS(a, p) (!(TOOHIGH(a, p) || TOOLOW(a, p)))

#define _IS(t, x) (((t)1 << (x)) != 0)
/*Evaluates true if the width of a variable                              \
 *of type of t is < x. the !=0 assures that the answer is 1 or 0*/

#define NBITS(t) (4*(1+_IS(t,4)+_IS(t,8)+_IS(t,12)+_IS(t,16)\
+_IS(t,20) +_IS(t,24)+_IS(t,28)+_IS(t,32))))
/*NBITS(int) evaluate to the number fo bits in an int*/

#define MAXINT (((unsigned)~0) >> 1)
/*find the largest integer*/

/* #ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif */
#define RANGE(a, b, c) ((a) <= (b) && (b) <= (c))
