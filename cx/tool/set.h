#pragma once
/*SET.H  Macros and function prototypes for the set functions */

typedef unsigned short _SETTYPE; /*one cell in bit map(two bytes or 16 bits*/
#define _BITS_IN_WORD 16

/* #of bytes in bit map,left shift 2 more efficient than multipy 2 */
#define _BYTES_IN_ARRAY(x) (x << 1)

/*determine the position(the array index) of a particular bit in the map \
_DIV_WSIZE(35) evaluates to 35/16 = 2 --- 3 .so the value is 2*/
#define _DIV_WSIZE(x) ((unsigned)(x) >> 4)

/* evaluate to the position of the bit within the word-the offset in bits from
 * the fromt the least-significant bit in the word. a bitwise AND with 0x0f(15)
 * more efficent than a % with 16 */
#define _MOD_WSIZE(x) ((x) & 0x0f)
#define _DEFWORDS 8                          /*elements in default set*/
#define _DEFBITS (_DEFWORDS * _BITS_IN_WORD) /* 128 bits in default set*/

/*expand the size of array,grows in _DEFWORDS-sized chunks.(巧妙的位运算)*/
#define _ROUND(bit) (((_DIV_WSIZE(bit) + 8) >> 3) << 3)

typedef struct _set_ {
  unsigned nwords;     /* Number of words in map*/
  unsigned nbits;      /* Number of bits in map*/
  bool ccompl;         /* is set to true if a negative-true set, default 0 */
  unsigned short *map; /* Pointer to the map */
  _SETTYPE defmap[_DEFWORDS]; /* The map itself.when the map grows,a new array
                                 is allocated and point to it . malloc() for
                                 run-time efficiency */
} SET;

int _addset(SET *set, int bit);
void delset(SET *set);
SET *dupset(SET *set);
void invert(SET *set);
SET *newset(void);
int next_member(SET *set);
int num_ele(SET *set);
void pset(SET *set);
void _set_op(int op, SET *dest, SET *src);
int _set_test(SET *set1, SET *set2);
int setcmp(SET *set1, SET *set2);
unsigned sethash(SET *);
int subset(SET *set, SET *possible_subset);
void truncate(SET *);

/* Op argument passed to _set_op */
#define _UNION 0      /* x is in s1 or s2 */
#define _INTERSECT 1  /* x is in s1 and s2 */
#define _DIFFERENCE 2 /* (x in s1) and (x not in s2) */
#define _ASSIGN 4     /* s1 = s2 */

#define UNION(d, s) _set_op(_UNION, d, s)
#define INTERSECT(d, s) _set_op(_INTERSECT, d, s)
#define DIFFERENCE(d, s) _set_op(_DIFFERENCE, d, s)
#define ASSIGN(d, s) _set_op(_ASSIGN, d, s)

#define CLEAR(s) memset((s)->map, 0, (s)->nwords * sizeof(_SETTYPE))
/*Binary One's Complement Operator is unary and has the effect of
 * 'flipping(翻转)' bits*/

/* negative number is represented in binary. First reverse the corresponding
 * bits, then add 1. So all ~0(-1)'s bits are 1 */
#define FILL(s) memset((s)->map, ~0, (s)->nwords * sizeof(_SETTYPE))
#define CCOMPLEMENT(s) ((s)->ccompl = ~((s)->ccompl))
#define INVERT(s) invert(s)

#define _SET_EQUIV 0 /*Value returned from _set_test,equivalent*/
#define _SET_DISJ 1  /*                              disjoint*/
#define _SET_INTER 2 /*                              intersecting*/

#define IS_DISJOINT(s1, s2) (_set_test(s1, s2) == _SET_DISJ)
#define IS_INTERSECTING(s1, s2) (_set_test(s1, s2) == _SET_INTER)
#define IS_EQUIVALENT(a, b) (setcmp((a), (b)) == 0)
#define IS_EMPTY(s) (num_ele(s) == 0)

/* All of the following have heavy-duty side-effects. Be careful. */

/* (s)->map(_DIV_WSIZE(x) indicates the array element,
 * 1<<_MOD_WSIZE(x) indicates the offset the bit from that array element. */
#define _GBIT(s, x, op) (((s)->map)[_DIV_WSIZE(x)] op(1 << _MOD_WSIZE(x)))

/* Zero the corresponding position */
#define REMOVE(s, x) (((x) >= (s)->nbits) ? 0 : _GBIT(s, x, &= ~))

/* put the corresponding position 1 */
#define ADD(s, x) (((x) >= (s)->nbits) ? _addset(s, x) : _GBIT(s, x, |=))
#define MEMBER(s, x) (((x) >= (s)->nbits) ? 0 : _GBIT(s, x, &))

/* handle complemented sets, works by first determining whether x is in the set,
 * and then evaluating to the complement flag ot its inverse, as appropriate.
 * For example, the complemented flag will be 1 if x is in a negative true set,
 * and MEMBER tests true if the bit is in the map. */
#define MTEST(s, x) ((MEMBER(s, x)) ? !(s)->ccompl : (s)->ccompl)
