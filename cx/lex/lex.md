the lex.par is followed:    
    
/*-------------------------------------------------------------*/

#ifndef YYPRIVATE
#define YYPRIVATE static
#endif

#ifndef NULL
#include<stdio.h>
#endif

#ifdef YYDEBUG
int yydebug=0;
#define YY_D(x) if(yydebug)(x;)else
#else
#define YY_D(x)
#endif

typedef unsigned char YY_TYPE
#define YYF ((YY_TYPE)(-1))

unsigned char*ii_text();

/*-------------------------------------------------------------*/
this part is copied to lex output file, a different file can be specified with -m
command-line switch

an uncompressed array is generated by specifying a -f (for fast) switch on the LEX command line

pair compression is activated in LEX with a -cN command-line switch. N is the threshold beyond
which pairs are abandoned in faver of a simple array indexed by lookahead character. The example
we've been looking at had the threshold set at 11, so any state with more than 11 outgoing 
transitions are represented with character/next-state pairs. The default threshold--used when 
no N is given on the command line--is four