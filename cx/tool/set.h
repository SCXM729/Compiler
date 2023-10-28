/*SET.H  Macros and function prototypes for the set functions */
typedef unsigned short _SETTYPE; /*one cell in bit map(two bytes())*/
#define _BITS_IN_WORD 16
#define _BYTES_IN_ARRAY(x)                                                     \
  (x << 1) /*#of bytes in bit map,left shift 2 more efficient than multipy 2*/
#define _DIV_WSIZE(x)                                                          \
  ((unsigned)(x) >> 4) /*determine the position of a particular bit in the map \
_DIV_WSIZE(35) evaluates to 35/16 = 2 --- 3 .so the value is 2*/
#define _MOD_WSIZE(x)                                                          \
  ((x)&0x0f)        /*evaluates to x/16 (the position of the bit in the word) */
#define _DEFWORDS 8 /*elements in default set*/
#define _DEFBITS (_DEFWORDS * _BITS_IN_WORD) /* 128 bits in default set*/
#define _ROUND(bit)                                                            \
  (((_DIV_WSIZE(bit) + 8) >> 3)                                                \
   << 3) /*expand the size of array,grows in _DEFWORDS-sized chunks*/

typedef struct _set_ {
  unsigned char nwords;       /*Number of words in map*/
  unsigned char comple;       /*is set to true if a negative-true set*/
  unsigned nbits;             /*Number of bits in map*/
  _SETTYPE *map;              /*Pointer to the map*/
  _SETTYPE defmap[_DEFWORDS]; /*The map itself.when the map grows,a new array is
                                 allocated and point to it . malloc() for
                                 run-time efficiency*/
} SET;

extern int _addset(SET *, int);
extern void delset(SET *);
extern SET *dupset(SET *);
extern void invert(SET *);
extern SET *newset(void);
extern int next_number(SET *);
extern int num_ele(SET *);
extern void pset(SET *, int (*)(), void *);
extern void _set_op(int, SET *, SET *);
extern int _set_test(SET *, SET *);
extern int setcmp(SET *, SET *);
extern int sethash(SET *);
extern int subset(SET *, SET *);
extern void truncate(SET *);

/*Op argument passed to _set_op*/
#define _UNION 0      /*x is in s1 or s2*/
#define _INTERSECT 1  /*x is in s1 and s2*/
#define _DIFFERENCE 2 /*(x in s1) and (x not in s2)*/
#define _ASSIGN 4     /*s1 = s2*/

#define UNION(d, s) _set_op(_UNION, d, s)
#define INTERSECT(d, s) _set_op(_INTERSECT, d, s)
#define DIFFERENCE(d, s) _set_op(_DIFFERENCE, d, s)
#define ASSIGN(d, s) _set_op(_ASSIGN, d, s)

#define CLEAR(s) memset((s)->map, 0, (s)->nwords * sizeof(_SETTYPE))
/*Binary One's Complement Operator is unary and has the effect of 'flipping'
 * bits*/
#define FILL(s) memset((s)->map, ~0, (s)->nwords * sizeof(_SETTYPE))
#define COMPLEMET(s) ((s)->compl = ~(s)->compl )
#define INVERT(s) invert(s)

#define _SET_EQUIV 0 /*Value returned from _set_test,equivalent*/
#define _SET_DISJ 1  /*                              disjoint*/
#define _SET_INTER 2 /*                              intersecting*/

#define IS_DISJOINT(s1, s2) (_set_test(s1, s2) == _SET_DISJ)
#define IS_INTERSECTING(s1, s2) (_set_test(s1, s2) == _SET_INTER)
#define is_EQUIVALENT(a, b) (setcmp((a), (b)) == 0)
#define is_EMPTY(s) (num_ele(s) == 0)

/*All of the following have heavy-duty side-effects. Be careful.*/
#define _GBIT(s, x, op) (((s)->map)[_DIV_WSIZE(x)] op(1 << _MOD_WSIZE(x)))

#define REMOVE(s, x)                                                           \
  (((x) >= (s)->nbits)                                                         \
       ? 0                                                                     \
       : _GBIT(s, x, &= ~)) /*Zero the corresponding position*/
#define ADD(s, x)                                                              \
  (((x) >= (s)->nbits) ? _addset(s, x)                                         \
                       : _GBIT(s, x, |=)) /*put the corresponding position 1*/
#define MEMBER(s, x) (((x) >= (s)->nbits) ? 0 : _GBIT(s, x, &))
#define TEST(s, x) ((MENBER(s, x)) ? !(s)->compl : (s)->compl )
