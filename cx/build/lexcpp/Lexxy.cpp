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
