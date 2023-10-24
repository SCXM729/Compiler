#define FCON 1
#define ICON 2
#ifdef __NEVER__
/*--------------------------
 * DFA (start state is 0) is:
 * State 0 [noaccepting]
 *   goto 3 on .
 *   goto 1 on 0123456789
 *State 1 [accepting,line 7 <return ICON;>]
 *   goto 2 on.
 *   goto 1 on 0123456789
 *   goto 5 on e
 *State 2 [accepting,line 8 <return\tFCON;>]
 *   goto 2 on 0123456789
 *   goto 5 on e
 *State 3 [nonaccepting]
 *   goto 2 on 0123456789
 *State 4 [accepting,line 8 <return\tFCON;>]
 *   goto 4 on 0123456789
 *State 5 [nonaccepting]
 *   goto 4 on 0123456789
 */
#endif

#ifndef YYPRIVATE
#define YYPRIVATE static
#endif

#ifndef NULL
#include <stdio.h>
#endif

#ifdef YYDEBUG
int yydebug = 0;
#define YY_D(x)    \
    if (yydebug) { \
        x;         \
    } else
#else
#define YY_D(x)
#endif

typedef char YY_TTYPE;
#define YYF ((YY_TTYPE)(-1))

unsigned char* ii_text();

// clang-format off
YYPRIVATE YY_TTYPE Yy_cmap[128]={
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,
    2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
// clang-format on
static char Yy_rmap[6] = {
    0, 1, 2, 3, 4, 4
};
static char Yy_nxt[5][4] = {
    { -1, 3, 1, -1 },
    { -1, 2, 1, 5 },
    { -1, -1, 2, 5 },
    { -1, -1, 2, -1 },
    { -1, -1, 4, -1 },
};
#define yy_next(state, c) (Yy_nxt[Yy_rmap[state]][Yy_cmap[c]])

