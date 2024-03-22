/*just for test*/                                                                              

                                                                            
                                                                             
                                                                         
                                      
   

#include <stdio.h>
#include <string.h>
#include <search.h>	 /* Function prototype for bsearch(). 	*/
#include <tools/debug.h> /* Needed by symtab.h.			*/
#include <tools/hash.h>	 /* Needed by symtab.h.			*/
#include <tools/l.h>
#include <tools/compiler.h>
#include "yyout.h"	/* Token defs. created by occs. Yacc uses y.tab.h. */
#include "symtab.h"	/* Needed to pass attributes to parser.		   */
#include "value.h"	/* ditto					   */

extern union {		      /* This definition must duplicate the %union */
    char	*p_char;      /* in c.y.				     */
    symbol	*p_sym;
    link	*p_link;
    structdef	*p_sdef;
    specifier	*p_spec;
    value	*p_value;
    int		integer;
    int		ascii;
}
yylval;				/* Declared by occs in yyout.c. */

extern FILE *yycodeout;		/* Declared by occs in yyout.c	*/

static int id_or_keyword P(( char *lex ));	/* declared, below */
/*----------------------------------------------------------------------*/

#define YYERROR	yyerror	/* Forces LeX-generated error messages to be     */
			/* output in an occs window if debugging mode    */
			/* is enabled.  Does nothing in UNIX lex. Remove */
			/* this definition if you aren't using occs -d.  */

/*----------------------------------------------------------------------*/










#ifdef _NEVER_
/*---------------------------------------*/* DFA (start state is 0) is:
*
 * State 0 [nonaccepting]
 * goto 25 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t
 * goto 26 on \n
 * goto 25 on \x00b
 * goto  1 on \f\r\x00e\x00f\x010\x011\x012\x013\x014\x015\x016\x017\x018
 *           \x019\x01a\x01b\x01c\x01d\x01e\x01f
 * goto 25 on  
 * goto 40 on !
 * goto 48 on "
 * goto  1 on #$
 * goto 43 on %
 * goto 24 on &
 * goto 39 on \'
 * goto 23 on (
 * goto 22 on )
 * goto 21 on *
 * goto 20 on +
 * goto 19 on ,
 * goto 18 on -
 * goto 17 on .
 * goto 16 on /
 * goto 42 on 0
 * goto 15 on 123456789
 * goto 14 on :
 * goto 13 on ;
 * goto 41 on <
 * goto 12 on =
 * goto 11 on >
 * goto 10 on ?
 * goto  1 on @
 * goto  6 on ABCDEFGHIJKLMNOPQRSTUVWXYZ
 * goto  9 on [
 * goto  1 on \\
 * goto  8 on ]
 * goto  7 on ^
 * goto  6 on _
 * goto  1 on `
 * goto  6 on abcdefghijklmnopqrstuvwxyz
 * goto  5 on {
 * goto  4 on |
 * goto  3 on }
 * goto  2 on ~
 * goto  1 on \x07f
 * State 1 [accepting, line 145] <yyerror( "Illegal ch>]
 * State 2 [accepting, line 104] <yylval.ascii = *yyte>]
 * State 3 [accepting, line 92] <return RC;>]
 * State 4 [accepting, line 132] <return OR;>]
 * goto 28 on =
 * goto 27 on |
 * State 5 [accepting, line 91] <return LC;>]
 * State 6 [accepting, line 141] <return id_or_keyword>]
 * goto  6 on 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrst
 *           uvwxyz
 * State 7 [accepting, line 131] <return XOR;>]
 * goto 28 on =
 * State 8 [accepting, line 94] <return RB;\n>]
 * State 9 [accepting, line 93] <return LB;>]
 * State 10 [accepting, line 135] <return QUEST;>]
 * State 11 [accepting, line 118] <yylval.ascii = yytex>]
 * goto 49 on =
 * goto 29 on >
 * State 12 [accepting, line 129] <return EQUAL;>]
 * goto 30 on =
 * State 13 [accepting, line 138] <return SEMI;>]
 * State 14 [accepting, line 136] <return COLON;>]
 * State 15 [accepting, line 85] <return ICON ;\n>]
 * goto 45 on .
 * goto 15 on 0123456789
 * goto 38 on E
 * goto 31 on F
 * goto 50 on LU
 * goto 38 on e
 * goto 31 on f
 * goto 50 on lu
 * State 16 [accepting, line 109] <yylval.ascii = *yyte>]
 * goto 32 on *
 * goto 28 on =
 * State 17 [accepting, line 97] <yylval.ascii = *yyte>]
 * goto 52 on .
 * goto 45 on 0123456789
 * State 18 [accepting, line 113] <return MINUS;\n>]
 * goto 33 on -
 * goto 28 on =
 * goto 44 on >
 * State 19 [accepting, line 137] <return COMMA;>]
 * State 20 [accepting, line 112] <return PLUS;>]
 * goto 33 on +
 * goto 28 on =
 * State 21 [accepting, line 107] <return STAR;\n>]
 * goto 28 on =
 * State 22 [accepting, line 90] <return RP;>]
 * State 23 [accepting, line 89] <return LP;>]
 * State 24 [accepting, line 130] <return AND;>]
 * goto 34 on &
 * goto 28 on =
 * State 25 [accepting, line 144] <;\t/* ignore other w>]
 * goto 25 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t\x00b 
 * State 26 [accepting, line 143] <fprintf(yycodeout, ">]
 * State 27 [accepting, line 134] <return OROR;>]
 * State 28 [accepting, line 126] <yylval.ascii = *yyte>]
 * State 29 [accepting, line 115] <yylval.ascii = *yyte>]
 * goto 28 on =
 * State 30 [accepting, line 122] <yylval.ascii = *yyte>]
 * State 31 [accepting, line 87] <return FCON ;\n>]
 * State 32 [accepting, line 55] <{\n                 >]
 * State 33 [accepting, line 101] <yylval.ascii = *yyte>]
 * State 34 [accepting, line 133] <return ANDAND;>]
 * State 35 [accepting, line 74] <{ return STRING; }\n>]
 * State 36 [accepting, line 76] <yyerror("Adding miss>]
 * State 37 [accepting, line 139] <return ELLIPSIS;\n>]
 * State 38 [nonaccepting]
 * goto 61 on +-
 * goto 54 on 0123456789
 * State 39 [accepting, line 145] <yyerror( "Illegal ch>]
 * goto 55 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t\x00b\f\r\x00e
 *           \x00f\x010\x011\x012\x013\x014\x015\x016\x017\x018\x019\x01a
 *           \x01b\x01c\x01d\x01e\x01f !"#$%&\'()*+,-./0123456789:;<=>
 *           ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[
 * goto 57 on \\
 * goto 55 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x07f
 * State 40 [accepting, line 104] <yylval.ascii = *yyte>]
 * goto 30 on =
 * State 41 [accepting, line 118] <yylval.ascii = yytex>]
 * goto 29 on <
 * goto 49 on =
 * State 42 [accepting, line 85] <return ICON ;\n>]
 * goto 45 on .
 * goto 53 on 01234567
 * goto 51 on 89
 * goto 38 on E
 * goto 31 on F
 * goto 50 on LU
 * goto 38 on e
 * goto 31 on f
 * goto 50 on lu
 * goto 47 on x
 * State 43 [accepting, line 109] <yylval.ascii = *yyte>]
 * goto 28 on =
 * State 44 [accepting, line 97] <yylval.ascii = *yyte>]
 * State 45 [accepting, line 87] <return FCON ;\n>]
 * goto 45 on 0123456789
 * goto 38 on E
 * goto 31 on F
 * goto 38 on e
 * goto 31 on f
 * State 46 [accepting, line 74] <{ return STRING; }\n>]
 * goto 59 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t
 * goto 36 on \n
 * goto 59 on \x00b\f\r\x00e\x00f\x010\x011\x012\x013\x014\x015\x016\x017
 *           \x018\x019\x01a\x01b\x01c\x01d\x01e\x01f !
 * goto 35 on "
 * goto 59 on #$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
 *           [
 * goto 60 on \\
 * goto 59 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x07f
 * State 47 [nonaccepting]
 * goto 56 on 0123456789ABCDEFabcdef
 * State 48 [accepting, line 145] <yyerror( "Illegal ch>]
 * goto 59 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t
 * goto 36 on \n
 * goto 59 on \x00b\f\r\x00e\x00f\x010\x011\x012\x013\x014\x015\x016\x017
 *           \x018\x019\x01a\x01b\x01c\x01d\x01e\x01f !
 * goto 35 on "
 * goto 59 on #$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
 *           [
 * goto 60 on \\
 * goto 59 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x07f
 * State 49 [accepting, line 118] <yylval.ascii = yytex>]
 * State 50 [accepting, line 85] <return ICON ;\n>]
 * State 51 [accepting, line 87] <return FCON ;\n>]
 * goto 45 on .
 * goto 51 on 0123456789
 * goto 38 on E
 * goto 31 on F
 * goto 38 on e
 * goto 31 on f
 * State 52 [nonaccepting]
 * goto 37 on .
 * State 53 [accepting, line 85] <return ICON ;\n>]
 * goto 45 on .
 * goto 53 on 01234567
 * goto 51 on 89
 * goto 38 on E
 * goto 31 on F
 * goto 50 on LU
 * goto 38 on e
 * goto 31 on f
 * goto 50 on lu
 * State 54 [accepting, line 87] <return FCON ;\n>]
 * goto 54 on 0123456789
 * goto 31 on Ff
 * State 55 [nonaccepting]
 * goto 50 on \'
 * State 56 [accepting, line 85] <return ICON ;\n>]
 * goto 56 on 0123456789ABCDEF
 * goto 50 on LU
 * goto 56 on abcdef
 * goto 50 on lu
 * State 57 [nonaccepting]
 * goto 55 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t\x00b\f\r\x00e
 *           \x00f\x010\x011\x012\x013\x014\x015\x016\x017\x018\x019\x01a
 *           \x01b\x01c\x01d\x01e\x01f !"#$%&
 * goto 58 on \'
 * goto 55 on ()*+,-./
 * goto 63 on 01234567
 * goto 55 on 89:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmno
 *           pqrstuvw
 * goto 62 on x
 * goto 55 on yz{|}~\x07f
 * State 58 [accepting, line 85] <return ICON ;\n>]
 * goto 50 on \'
 * State 59 [nonaccepting]
 * goto 59 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t
 * goto 36 on \n
 * goto 59 on \x00b\f\r\x00e\x00f\x010\x011\x012\x013\x014\x015\x016\x017
 *           \x018\x019\x01a\x01b\x01c\x01d\x01e\x01f !
 * goto 35 on "
 * goto 59 on #$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
 *           [
 * goto 60 on \\
 * goto 59 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x07f
 * State 60 [nonaccepting]
 * goto 59 on \x000\x001\x002\x003\x004\x005\x006\x007\b\t
 * goto 36 on \n
 * goto 59 on \x00b\f\r\x00e\x00f\x010\x011\x012\x013\x014\x015\x016\x017
 *           \x018\x019\x01a\x01b\x01c\x01d\x01e\x01f !
 * goto 46 on "
 * goto 59 on #$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
 *           [
 * goto 60 on \\
 * goto 59 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x07f
 * State 61 [nonaccepting]
 * goto 54 on 0123456789
 * State 62 [nonaccepting]
 * goto 50 on \'
 * goto 66 on 0123456789ABCDEFabcdef
 * State 63 [nonaccepting]
 * goto 50 on \'
 * goto 64 on 01234567
 * State 64 [nonaccepting]
 * goto 50 on \'
 * goto 55 on 01234567
 * State 65 [nonaccepting]
 * goto 50 on \'
 * goto 55 on 0123456789ABCDEFabcdef
 * State 66 [nonaccepting]
 * goto 50 on \'
 * goto 65 on 0123456789ABCDEFabcdef
 */

#endif
/*@A (C) 1992 Allen I. Holub                                                */




/* YY_TTYPE is used for the DFA transition table: Yy_nxt[], declared below.
 * YYF marks failure transitions in the DFA transition table. There's no failure
 * state in the table itself, these transitions must be handled by the driver
 * program. The DFA start state is State 0. YYPRIVATE is only defined here only
 * if it hasn't be #defined earlier. I'm assuming that if NULL is undefined,
 * <stdio.h> hasn't been included.
 */

#ifndef YYPRIVATE
#       define YYPRIVATE static
#endif

#ifndef NULL
#       include <stdio.h>
#endif

#include <tools/debug.h> /* For P() macro used in l.h and below		*/
#include <tools/l.h>	 /* Prototoyptes for the ii_ functions.		*/

#ifdef __TURBOC__	 /* Turn off the "unreachable code" messages	*/
#pragma warn -rch	 /* generated by break statements that follow	*/
#endif		 /* user-supplied returns in lex actions.	*/

#ifdef YYDEBUG
	int	yydebug = 0;
#	define YY_D(x) if( yydebug ){ x; }else
#else
#	define YY_D(x)
#endif

typedef unsigned char	YY_TTYPE;

#define YYF		(( YY_TTYPE )(-1))

unsigned char	*ii_text();

/*----------------------------------------------------
 * The Yy_cmap[] and Yy_rmap arrays are used as follows:
 * 
 *  next_state= Yydtran[ Yy_rmap[current_state] ][ Yy_cmap[input_char] ];
 * 
 * Character positions in the Yy_cmap array are:
 * 
 *    ^@  ^A  ^B  ^C  ^D  ^E  ^F  ^G  ^H  ^I  ^J  ^K  ^L  ^M  ^N  ^O
 *    ^P  ^Q  ^R  ^S  ^T  ^U  ^V  ^W  ^X  ^Y  ^Z  ^[  ^\  ^]  ^^  ^_
 *         !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
 *     0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
 *     @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
 *     P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
 *     `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
 *     p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~   DEL
 */

YYPRIVATE YY_TTYPE  Yy_cmap[128] =
{
       0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  2,  2,  2,  2,
       2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
       0,  3,  4,  2,  2,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 19, 20, 21, 22, 23, 24,
       2, 25, 25, 25, 25, 26, 27, 28, 28, 28, 28, 28, 29, 28, 28, 28,
      28, 28, 28, 28, 28, 29, 28, 28, 28, 28, 28, 30, 31, 32, 33, 28,
       2, 25, 25, 25, 25, 26, 27, 28, 28, 28, 28, 28, 29, 28, 28, 28,
      28, 28, 28, 28, 28, 29, 28, 28, 34, 28, 28, 35, 36, 37, 38,  2
};

YYPRIVATE YY_TTYPE  Yy_rmap[67] =
{
       0,  1,  1,  1,  2,  1,  3,  4,  1,  1,  1,  5,  6,  1,  1,  7,
       8,  9, 10,  1, 11,  4,  1,  1, 12, 13,  1,  1,  1,  4,  1,  1,
       1,  1,  1,  1,  1,  1, 14, 15,  6, 16, 17,  4,  1, 18, 19, 20,
      19,  1,  1, 21, 22, 23, 24, 25, 26, 27, 25, 19, 28, 29, 30, 31,
      32, 33, 34
};

YYPRIVATE YY_TTYPE Yy_nxt[ 35 ][ 39 ]=
{
/* 00 */ (  25,  26,   1,  40,  48,  43,  24,  39,  23,  22, 
     21,  20,  19,  18,  17,  16,  42,  15,  15,  14, 
     13,  41,  12,  11,  10,   6,   6,   6,   6,   6, 
      9,   1,   8,   7,   6,   5,   4,   3,   2
     ),
/* 01 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 02 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  27,  -1,  -1
     ),
/* 03 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,   6,   6,   6,  -1, 
     -1,  -1,  -1,  -1,  -1,   6,   6,   6,   6,   6, 
     -1,  -1,  -1,  -1,   6,  -1,  -1,  -1,  -1
     ),
/* 04 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 05 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  49,  29,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 06 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  30,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 07 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  45,  -1,  15,  15,  15,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  38,  31,  -1,  50, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 08 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     32,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 09 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  52,  -1,  45,  45,  45,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 10 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  33,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  28,  44,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 11 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  33,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 12 */ (  -1,  -1,  -1,  -1,  -1,  -1,  34,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  28,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 13 */ (  25,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 14 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  61,  -1,  61,  -1,  -1,  54,  54,  54,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 15 */ (  55,  -1,  55,  55,  55,  55,  55,  55,  55,  55, 
     55,  55,  55,  55,  55,  55,  55,  55,  55,  55, 
     55,  55,  55,  55,  55,  55,  55,  55,  55,  55, 
     55,  57,  55,  55,  55,  55,  55,  55,  55
     ),
/* 16 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  29,  49,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 17 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  45,  -1,  53,  53,  51,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  38,  31,  -1,  50, 
     -1,  -1,  -1,  -1,  47,  -1,  -1,  -1,  -1
     ),
/* 18 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  45,  45,  45,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  38,  31,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 19 */ (  59,  36,  59,  59,  35,  59,  59,  59,  59,  59, 
     59,  59,  59,  59,  59,  59,  59,  59,  59,  59, 
     59,  59,  59,  59,  59,  59,  59,  59,  59,  59, 
     59,  60,  59,  59,  59,  59,  59,  59,  59
     ),
/* 20 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  56,  56,  56,  -1, 
     -1,  -1,  -1,  -1,  -1,  56,  56,  56,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 21 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  45,  -1,  51,  51,  51,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  38,  31,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 22 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  37,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 23 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  45,  -1,  53,  53,  51,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  38,  31,  -1,  50, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 24 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  54,  54,  54,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  31,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 25 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 26 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  56,  56,  56,  -1, 
     -1,  -1,  -1,  -1,  -1,  56,  56,  56,  -1,  50, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 27 */ (  55,  -1,  55,  55,  55,  55,  55,  58,  55,  55, 
     55,  55,  55,  55,  55,  55,  63,  63,  55,  55, 
     55,  55,  55,  55,  55,  55,  55,  55,  55,  55, 
     55,  55,  55,  55,  62,  55,  55,  55,  55
     ),
/* 28 */ (  59,  36,  59,  59,  46,  59,  59,  59,  59,  59, 
     59,  59,  59,  59,  59,  59,  59,  59,  59,  59, 
     59,  59,  59,  59,  59,  59,  59,  59,  59,  59, 
     59,  60,  59,  59,  59,  59,  59,  59,  59
     ),
/* 29 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  54,  54,  54,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 30 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  66,  66,  66,  -1, 
     -1,  -1,  -1,  -1,  -1,  66,  66,  66,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 31 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  64,  64,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 32 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  55,  55,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 33 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  55,  55,  55,  -1, 
     -1,  -1,  -1,  -1,  -1,  55,  55,  55,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ),
/* 34 */ (  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  65,  65,  65,  -1, 
     -1,  -1,  -1,  -1,  -1,  65,  65,  65,  -1,  -1, 
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1
     ) 
};

/*----------------------------------------------------
 * yy_next(state,c) is given the current state number and input
 * character and evaluates to the next state.
 */

#define yy_next(state,c) (Yy_nxt[ Yy_rmap[state] ][ Yy_cmap[c] ])

/*----------------------------------------------------
 * The Yyaccept array has two purposes, If Yyaccept[i] is 0 then state
 * i is nonaccepting. If it's nonzero then the number determines whether
 * the string is anchored, 1=anchored at start of line,2=at end of line,
 * 3=both,4=line not anchored
 */

YYPRIVATE YY_TTYPE Yyaccept[] =
{
	0 ,    /* State 0   */
	4  ,    /* State 1   */
	4  ,    /* State 2   */
	4  ,    /* State 3   */
	4  ,    /* State 4   */
	4  ,    /* State 5   */
	4  ,    /* State 6   */
	4  ,    /* State 7   */
	4  ,    /* State 8   */
	4  ,    /* State 9   */
	4  ,    /* State 10  */
	4  ,    /* State 11  */
	4  ,    /* State 12  */
	4  ,    /* State 13  */
	4  ,    /* State 14  */
	4  ,    /* State 15  */
	4  ,    /* State 16  */
	4  ,    /* State 17  */
	4  ,    /* State 18  */
	4  ,    /* State 19  */
	4  ,    /* State 20  */
	4  ,    /* State 21  */
	4  ,    /* State 22  */
	4  ,    /* State 23  */
	4  ,    /* State 24  */
	4  ,    /* State 25  */
	4  ,    /* State 26  */
	4  ,    /* State 27  */
	4  ,    /* State 28  */
	4  ,    /* State 29  */
	4  ,    /* State 30  */
	4  ,    /* State 31  */
	4  ,    /* State 32  */
	4  ,    /* State 33  */
	4  ,    /* State 34  */
	4  ,    /* State 35  */
	4  ,    /* State 36  */
	4  ,    /* State 37  */
	0 ,    /* State 38  */
	4  ,    /* State 39  */
	4  ,    /* State 40  */
	4  ,    /* State 41  */
	4  ,    /* State 42  */
	4  ,    /* State 43  */
	4  ,    /* State 44  */
	4  ,    /* State 45  */
	4  ,    /* State 46  */
	0 ,    /* State 47  */
	4  ,    /* State 48  */
	4  ,    /* State 49  */
	4  ,    /* State 50  */
	4  ,    /* State 51  */
	0 ,    /* State 52  */
	4  ,    /* State 53  */
	4  ,    /* State 54  */
	0 ,    /* State 55  */
	4  ,    /* State 56  */
	0 ,    /* State 57  */
	4  ,    /* State 58  */
	0 ,    /* State 59  */
	0 ,    /* State 60  */
	0 ,    /* State 61  */
	0 ,    /* State 62  */
	0 ,    /* State 63  */
	0 ,    /* State 64  */
	0 ,    /* State 65  */
	0      /* State 66  */
};




/*-----------------------------------------------------------------
 * Global variables used by the parser.
 */

char   *yytext;		/* Pointer to lexeme.           */
int    	yyleng;		/* Length of lexeme.            */
int    	yylineno;	/* Input line number.           */
FILE	*yyout = stdout;

/*-----------------------------------------------------------------
 * Macros that duplicate functions in UNIX lex:
 */

#define output(c)   putc(c,yyout)
#define ECHO        fprintf(yyout, "%s", yytext )

#ifndef YYERROR
#    define YYERROR printf
#endif

#define yymore()    yymoreflg = 1

#define unput(c)  (ii_unput(c), --yyleng )
#define yyless(n) (    ii_unterm(), \
		     ( yyleng -= ii_pushback(n) ? n : yyleng ), \
		       ii_term() \
		  )

int  input P(( void ))		/* This is a macro in UNIX lex 	    */
{				/* P() is defined in tools/debug.h  */
    int c;

    if( (c = ii_input()) && (c != -1))
    {
	yytext   = (char *) ii_text();
	yylineno = ii_lineno();
	++yyleng;
    }
    return c;
}

/*----------------------------------------------------------------------*/

yylex()
{
    int        yymoreflg;         /* Set when yymore() is executed       */
    static int yystate   = -1;    /* Current state.                      */
    int        yylastaccept;      /* Most recently seen accept state     */
    int        yyprev;            /* State before yylastaccept           */
    int        yynstate;          /* Next state, given lookahead.        */
    int        yylook;            /* Lookahead character	         */
    int	       yyanchor;	  /* Anchor point for most recently seen */
				  /* accepting state.			 */
    if( yystate == -1 )
    {
	yy_init_lex();		/* One-time initializations */
        ii_advance();
        ii_pushback(1);
    }

    yystate      = 0;		/* Top-of-loop initializations */
    yylastaccept = 0;
    yymoreflg    = 0;
    ii_unterm();
    ii_mark_start();

    while( 1 )
    {
	/* Check end of file. If there's an unprocessed accepting state,
	 * yylastaccept will be nonzero. In this case, ignore EOF for now so
	 * that you can do the accepting action; otherwise, try to open another
	 * file and return if you can't.
	 */


	while( 1 )
	{
	    if( (yylook=ii_look(1)) != EOF )
	    {
		yynstate = yy_next( yystate, yylook );
		break;
	    }
	    else
	    {
		if( yylastaccept )		/* still something to do */
		{
		    yynstate = YYF;
		    break;
		}
		else if( yywrap() )    		/* another file?  */
		{      	            		/* no             */
		     yytext = "";
		     yyleng = 0;
		     return 0;
		}
		else
		{
		    ii_advance(); 		/* load a new buffer */
		    ii_pushback(1);
		}
	    }
	}


        if( yynstate != YYF )
        {
	    YY_D( printf("    Transition from state %d", yystate )     );
	    YY_D( printf(" to state %d on <%c>\n",   yynstate, yylook) );

            if( ii_advance() < 0 )               	    /* Buffer full */
            {
                YYERROR( "Line %d, lexeme too long. "
		         "Discarding extra characters.\n", ii_lineno() );
                ii_flush(1);
            }

            if(yyanchor = Yyaccept[ yynstate ])	   /* saw an accept state */
            {
		yyprev       = yystate  ;
		yylastaccept = yynstate ;
		ii_mark_end();  	/* Mark input at current character. */
			        	/* A subsequent ii_to_mark()        */
			        	/* returns us to this position.     */
            }

            yystate = yynstate;
        }
	else
        {
	    if( !yylastaccept )	   			  /* illegal input */
	    {
#ifdef YYBADINP
		YYERROR( "Ignoring bad input\n" );
#endif
		ii_advance();          /* Skip char that caused failure.   */
	    }
	    else
	    {
		ii_to_mark();          /* Back up to previous accept state */
		if( yyanchor & 2 )     /* If end anchor is active	   */
		    ii_pushback(1);    /* push back the CR or LF	   */

		if( yyanchor & 1 )		/* if start anchor is active */
		    ii_move_start();		/* skip the leading newline  */

		ii_term();              	/* Null-terminate the string */
		yytext   = (char *) ii_text();
		yyleng   = ii_length ();
		yylineno = ii_lineno ();

		YY_D( printf("Accepting state %d, ", yylastaccept )	);
		YY_D( printf("line %d: <%s>\n",      yylineno, yytext )	);

		switch( yylastaccept )
		{
		case 1:					/* State 1   */		    yyerror( "Illegal character <%s>\n", yytext );
		    break;
		case 2:					/* State 2   */		    yylval.ascii = *yytext;
			return UNOP;

		    break;
		case 3:					/* State 3   */		    return RC;
		    break;
		case 4:					/* State 4   */		    return OR;
		    break;
		case 5:					/* State 5   */		    return LC;
		    break;
		case 6:					/* State 6   */		    return id_or_keyword( yytext );

		    break;
		case 7:					/* State 7   */		    return XOR;
		    break;
		case 8:					/* State 8   */		    return RB;

		    break;
		case 9:					/* State 9   */		    return LB;
		    break;
		case 10:					/* State 10  */		    return QUEST;
		    break;
		case 11:					/* State 11  */		    yylval.ascii = yytext[1] ? (yytext[0]=='>' ? 'G' : 'L')
						 : (yytext[0]		      );
			return RELOP;

		    break;
		case 12:					/* State 12  */		    return EQUAL;
		    break;
		case 13:					/* State 13  */		    return SEMI;
		    break;
		case 14:					/* State 14  */		    return COLON;
		    break;
		case 15:					/* State 15  */		    return ICON ;

		    break;
		case 16:					/* State 16  */		    yylval.ascii = *yytext;
			return DIVOP;

		    break;
		case 17:					/* State 17  */		    yylval.ascii = *yytext;
			return STRUCTOP;

		    break;
		case 18:					/* State 18  */		    return MINUS;

		    break;
		case 19:					/* State 19  */		    return COMMA;
		    break;
		case 20:					/* State 20  */		    return PLUS;
		    break;
		case 21:					/* State 21  */		    return STAR;

		    break;
		case 22:					/* State 22  */		    return RP;
		    break;
		case 23:					/* State 23  */		    return LP;
		    break;
		case 24:					/* State 24  */		    return AND;
		    break;
		case 25:					/* State 25  */		    ;	/* ignore other white space */
		    break;
		case 26:					/* State 26  */		    fprintf(yycodeout, "\t\t\t\t\t\t\t\t\t/*%d*/\n", yylineno);
		    break;
		case 27:					/* State 27  */		    return OROR;
		    break;
		case 28:					/* State 28  */		    yylval.ascii = *yytext;
			return ASSIGNOP;

		    break;
		case 29:					/* State 29  */		    yylval.ascii = *yytext;
		        return SHIFTOP;

		    break;
		case 30:					/* State 30  */		    yylval.ascii = *yytext;
                   	return EQUOP;

		    break;
		case 31:					/* State 31  */		    return FCON ;

		    break;
		case 32:					/* State 32  */		    {
                        int i;

                        while( i = ii_input() )
                        {
                           if( i < 0 )
                              ii_flushbuf();   		/* Discard lexeme. */

                           else if( i == '*'  &&  ii_lookahead(1) == '/' )
                           {
                                ii_input();
                                break;          	/* Recognized comment.*/
                           }
                        }

                        if( i == 0 )
                           yyerror( "End of file in comment\n" );
                    }

		    break;
		case 33:					/* State 33  */		    yylval.ascii = *yytext;
			return INCOP;

		    break;
		case 34:					/* State 34  */		    return ANDAND;
		    break;
		case 35:					/* State 35  */		    { return STRING; }

		    break;
		case 36:					/* State 36  */		    yyerror("Adding missing \" to string constant\n");
		    yymore();

		    break;
		case 37:					/* State 37  */		    return ELLIPSIS;

		    break;
		case 39:					/* State 39  */		    yyerror( "Illegal character <%s>\n", yytext );
		    break;
		case 40:					/* State 40  */		    yylval.ascii = *yytext;
			return UNOP;

		    break;
		case 41:					/* State 41  */		    yylval.ascii = yytext[1] ? (yytext[0]=='>' ? 'G' : 'L')
						 : (yytext[0]		      );
			return RELOP;

		    break;
		case 42:					/* State 42  */		    return ICON ;

		    break;
		case 43:					/* State 43  */		    yylval.ascii = *yytext;
			return DIVOP;

		    break;
		case 44:					/* State 44  */		    yylval.ascii = *yytext;
			return STRUCTOP;

		    break;
		case 45:					/* State 45  */		    return FCON ;

		    break;
		case 46:					/* State 46  */		    { return STRING; }

		    break;
		case 48:					/* State 48  */		    yyerror( "Illegal character <%s>\n", yytext );
		    break;
		case 49:					/* State 49  */		    yylval.ascii = yytext[1] ? (yytext[0]=='>' ? 'G' : 'L')
						 : (yytext[0]		      );
			return RELOP;

		    break;
		case 50:					/* State 50  */		    return ICON ;

		    break;
		case 51:					/* State 51  */		    return FCON ;

		    break;
		case 53:					/* State 53  */		    return ICON ;

		    break;
		case 54:					/* State 54  */		    return FCON ;

		    break;
		case 56:					/* State 56  */		    return ICON ;

		    break;
		case 58:					/* State 58  */		    return ICON ;

		    break;











		default:
		    YYERROR("INTERNAL ERROR, yylex: Unknown accept state %d.\n",
								yylastaccept );
		    break;
		}
	    }

	    ii_unterm();
	    yylastaccept = 0;

	    if( !yymoreflg )
	    {
		yystate = 0;
		ii_mark_start();
	    }
	    else
	    {
		yystate   = yyprev;  	/* Back up */
		yymoreflg = 0;
	    }
        }
    }
}


/*------------------------------------------------------------------*/
typedef struct		/* Routines to recognize keywords. A table	*/
{			/* lookup is used for this purpose in order to	*/
    char  *name;	/* minimize the number of states in the FSM. A	*/
    int   val;		/* KWORD is a single table entry.		*/
}
KWORD;

KWORD  Ktab[] =			/* Alphabetic keywords	*/
{
    { "auto",     CLASS         },
    { "break",    BREAK         },
    { "case",     CASE          },
    { "char",     TYPE          },
    { "continue", CONTINUE      },
    { "default",  DEFAULT       },
    { "do",       DO            },
    { "double",   TYPE          },
    { "else",     ELSE          },
    { "enum",     ENUM          },
    { "extern",   CLASS         },
    { "float",    TYPE          },
    { "for",      FOR           },
    { "goto",     GOTO          },
    { "if",       IF            },
    { "int",      TYPE          },
    { "long",     TYPE          },
    { "register", CLASS         },
    { "return",   RETURN        },
    { "short",    TYPE          },
    { "sizeof",   SIZEOF        },
    { "static",   CLASS         },
    { "struct",   STRUCT        },
    { "switch",   SWITCH        },
    { "typedef",  CLASS         },
    { "union",    STRUCT        },
    { "unsigned", TYPE          },
    { "void",     TYPE          },
    { "while",    WHILE         }
};

static int cmp( a, b )
KWORD   *a, *b;
{
    return strcmp( a->name, b->name );
}

static int id_or_keyword( lex )	/* Do a binary search for a */
char    *lex;			/* possible keyword in Ktab */
{				/* Return the token if it's */
    KWORD         *p;		/* in the table, NAME       */
    KWORD         dummy;	/* otherwise.		    */

    dummy.name = lex;
    p = (KWORD *) bsearch( &dummy, Ktab, sizeof(Ktab)/sizeof(KWORD),
								sizeof(KWORD),
	BCC ((int _Cdecl(*)   (const void _FAR *, const void _FAR *)))
	MSC ((int  (__cdecl *)(const void *,      const void      *))) cmp );

    if( p )						/* It's a keyword. */
    {
	yylval.ascii = *yytext;
	return p->val;
    }
    else if( yylval.p_sym = (symbol *) findsym( Symbol_tab, yytext ) )
	return (yylval.p_sym->type->tdef) ? TTYPE : NAME ;
    else
	return NAME;
}
#ifdef TEST_LEX
ptok( int tok  )
{
	switch( tok )
	{
	case STRING:	printf("STRING (%s)\n",	yytext );	break;
	case ICON:	printf("ICON (%s)\n",	yytext );	break;
	case FCON:	printf("FCON (%s)\n",	yytext );	break;
	case TYPE:	printf("TYPE (%s)\n",	yytext );	break;
	case STRUCT:	printf("STRUCT (%s)\n",	yytext );	break;
	case ENUM:	printf("ENUM (%s)\n",	yytext );	break;
	case RETURN:	printf("RETURN (%s)\n",	yytext );	break;
	case GOTO:	printf("GOTO (%s)\n",	yytext );	break;
	case IF:	printf("IF (%s)\n",	yytext );	break;
	case ELSE:	printf("ELSE (%s)\n",	yytext );	break;
	case SWITCH:	printf("SWITCH (%s)\n",	yytext );	break;
	case CASE:	printf("CASE (%s)\n",	yytext );	break;
	case DEFAULT:	printf("DEFAULT (%s)\n",yytext );	break;
	case BREAK:	printf("BREAK (%s)\n",	yytext );	break;
	case CONTINUE:	printf("CONTINUE (%s)\n",yytext );	break;
	case WHILE:	printf("WHILE (%s)\n",	yytext );	break;
	case DO:	printf("DO (%s)\n",	yytext );	break;
	case FOR:	printf("FOR (%s)\n",	yytext );	break;
	case LC:	printf("LC (%s)\n",	yytext );	break;
	case RC:	printf("RC (%s)\n",	yytext );	break;
	case SEMI:	printf("SEMI (%s)\n",	yytext );	break;
	case ELLIPSIS:	printf("ELLIPSIS (%s)\n",yytext );	break;
	case COMMA:	printf("COMMA (%s)\n",	yytext );	break;
	case EQUAL:	printf("EQUAL (%s)\n",	yytext );	break;
	case ASSIGNOP:	printf("ASSIGNOP (%s)\n",yytext );	break;
	case QUEST:	printf("QUEST (%s)\n",	yytext );	break;
	case COLON:	printf("COLON (%s)\n",	yytext );	break;
	case OROR:	printf("OROR (%s)\n",	yytext );	break;
	case ANDAND:	printf("ANDAND (%s)\n",	yytext );	break;
	case OR:	printf("OR (%s)\n",	yytext );	break;
	case XOR:	printf("XOR (%s)\n",	yytext );	break;
	case AND:	printf("AND (%s)\n",	yytext );	break;
	case EQUOP:	printf("EQUOP (%s)\n",	yytext );	break;
	case RELOP:	printf("RELOP (%s)\n",	yytext );	break;
	case SHIFTOP:	printf("SHIFTOP (%s)\n",yytext );	break;
	case PLUS:	printf("PLUS (%s)\n",	yytext );	break;
	case MINUS:	printf("MINUS (%s)\n",	yytext );	break;
	case STAR:	printf("STAR (%s)\n",	yytext );	break;
	case DIVOP:	printf("DIVOP (%s)\n",	yytext );	break;
	case SIZEOF:	printf("SIZEOF (%s)\n",	yytext );	break;
	case UNOP:	printf("UNOP (%s)\n",	yytext );	break;
	case INCOP:	printf("INCOP (%s)\n",	yytext );	break;
	case LB:	printf("LB (%s)\n",	yytext );	break;
	case RB:	printf("RB (%s)\n",	yytext );	break;
	case LP:	printf("LP (%s)\n",	yytext );	break;
	case RP:	printf("RP (%s)\n",	yytext );	break;
	case STRUCTOP:	printf("STRUCTOP (%s)\n",yytext );	break;
	case TTYPE:	printf("TTYPE (%s)\n",	yytext );	break;
	case CLASS:	printf("CLASS (%s)\n",	yytext );	break;
	case NAME:	printf("NAME (%s)\n",	yytext );	break;
	}
}
#endif

