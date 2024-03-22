#include "set.h"
#include "debug.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
SET *newset() {
  /*Create a new set and return a pointer to it.Print an error message
   * and exit if there's insufficient memory.
   */

  SET *p;

  if (!(p = (SET *)malloc(sizeof(SET)))) {
    fprintf(stderr, "Can't get memory to create set\n");
    exit(1);
  }

  memset(p, 0, sizeof(SET));
  p->map = p->defmap; /* doesn't allocate memory for p->map */
  p->nwords = _DEFWORDS;
  p->nbits = _DEFBITS;
  return p;
}

void delset(SET *set) {
  /* Delete a set created with a previous newset() call.*/

  if (set->map != set->defmap)
    free(set->map);
  free(set);
}

SET *dupset(SET *set) {
  /* Create a new set that has the same members as the input set */

  SET *cnew;

  if (!(cnew = (SET *)malloc(sizeof(SET)))) {
    fprintf(stderr, "Can't get memory to duplicate set");
    exit(1);
  }

  memset(cnew, 0, sizeof(SET));
  cnew->ccompl = set->ccompl;
  cnew->nwords = set->nwords;
  cnew->nbits = set->nbits;

  if (set->map == set->defmap) { /* default bit map is used */
    cnew->map = cnew->defmap;
    memcpy(cnew->defmap, set->defmap, sizeof(_SETTYPE) * _DEFWORDS);
  } else { /* map is used */
    cnew->map = (_SETTYPE *)malloc(set->nwords * sizeof(_SETTYPE));
    if (!cnew->map) {
      fprintf(stderr, "Can't get memory to duplicate set bit map\n");
      exit(1);
    }
    memcpy(cnew->map, set->map, set->nwords * sizeof(_SETTYPE));
  }
  return cnew;
}

int _addset(SET *set, int bit) {
  /* Addset is called by the ADD() macro when the set isn't big enough.
   * It expands the set to the necessary size and sets the indicated bit
   */

  void enlarge(unsigned, SET *); /* immediately following */

  enlarge(_ROUND(bit), set);
  return _GBIT(set, bit, |=);
}

void enlarge(unsigned need, SET *set) { /* Remove PRIVATE declaration */
  /*Enlarge the set to "need" words,filling in the extra words with zeros.
   * Print an error message and abort by raising SIGABRT if there's not
   * enough memory.NULL is returned if raise() returns.Since this routine
   * calls malloc,it's rather slow and should be avoided if possible
   */

  _SETTYPE *cnew;

  if (!set || need <= set->nwords) {
    return;
  }

  // D(printf("enlarging %d word map to %d words\n", set->nwords, need);)

  if (!(cnew = (_SETTYPE *)malloc(need * sizeof(_SETTYPE)))) {
    fprintf(stderr, "Can't get memory to expand set\n");
    exit(1);
  }
  memcpy(cnew, set->map, set->nwords * sizeof(_SETTYPE));
  memset(cnew + set->nwords, 0, (need - set->nwords) * sizeof(_SETTYPE));

  // release the original memory.
  if (set->map != set->defmap)
    free(set->map);

  set->map = cnew;
  set->nwords = need;
  set->nbits = need * _BITS_IN_WORD;
}

int num_ele(SET *set) {
  /* Return the number of elements (nonzero bits) in the set. NULL Set are
   * considered empty. The table-lookup approach used here was suggested to
   * me by DM. Nbits[] is indexed by any number in the range 0-255,
   * and it evaluates to the number of bits in the number
   */

  /* clang-format off */
  static unsigned char nbits[]={
    /*   0-15  */	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    /*  16-31  */	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    /*  32-47  */	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    /*  48-63  */	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    /*  64-79  */	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    /*  80-95  */	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    /*  96-111 */	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    /* 112-127 */	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    /* 128-143 */	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    /* 144-159 */	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    /* 160-175 */	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    /* 176-191 */	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    /* 192-207 */	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    /* 208-223 */	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    /* 224-239 */	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    /* 240-255 */	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };
  /* clang-format on */

  int i;
  unsigned int count = 0;
  unsigned char *p;

  if (!set)
    return 0;

  p = (unsigned char *)set->map;
  /* the sum of number of 1 in every array element. */
  for (i = _BYTES_IN_ARRAY(set->nwords); --i >= 0;)
    count += nbits[*p++];

  return count;
}

/*--------------------------------------------------------------------*/

int _set_test(SET *set1, SET *set2) {
  /* Compares two sets. Returns as follows:
   *
   * _SET_EQUIV    Sets are equivalent
   * _SET_INTER    Sets intersect but aren't equivalent
   * _SET_DISJ     Sets are disjoint
   *
   * The smaller set is made larger if the two sets are different sizes
   */

  int i, rval = _SET_EQUIV;
  _SETTYPE *p1, *p2;

  i = std::max(set1->nwords, set2->nwords);

  enlarge(i, set1); /* Make the sets the same size */
  enlarge(i, set2);

  p1 = set1->map;
  p2 = set2->map;

  int equ_flag = 0;
  for (; --i >= 0; p1++, p2++) {
    if (*p1 != *p2) {
      /* You get here if the sets aren't equivalent. You can return
       * immediately if the sets intersect but have to keep going in the
       * case of disjoint sets (because the sets might actually intersect
       * at some byte, as yet unseen)
       */
      equ_flag++;
      if (*p1 & *p2) {

        return _SET_INTER;
      } else if (!*p1 || !*p2) {
        continue;
      } else {
        rval = _SET_DISJ;
      }
    }
  }

  return rval; /* They're equivalent */
}

int setcmp(SET *set1, SET *set2) {
  /* Yet another comparison function. This one works like strcmp(),
   * returning 0 if the sets are equivalent. <0 if set1 < set2 and >0
   * if set1 > set2
   */

  int i, j;
  _SETTYPE *p1, *p2;

  i = j = std::min(set1->nwords, set2->nwords);

  for (p1 = set1->map, p2 = set2->map; --j >= 0; p1++, p2++)
    if (*p1 != *p2)
      return *p1 - *p2;
  /* You get here only if all words that exist in both sets are same.
   * Check the tail end of the larger array for all zeros
   */

  if ((j = set1->nwords - i) > 0) { /* Set 1 is the larger */
    while (--j >= 0)
      if (*p1++)
        return 1;
  } else if ((j = set2->nwords - i) > 0) /* Set 2 is larger */
    while (--j >= 0)
      if (*p2++)
        return -1;

  return 0; /* They're equivalent */
}

unsigned sethash(SET *set1) {
  /* hash the set by summing together the words in the bit map */

  _SETTYPE *p;
  unsigned total;
  int j;

  total = 0;
  j = set1->nwords;
  p = set1->map;

  while (--j >= 0)
    total += *p++;

  return total;
}

int subset(SET *set, SET *possible_subset) {
  /* Return 1 if "possible_subet" is a subset of "set". One is returned if
   * it's subset, zero otherwise. Empty sets are subets of everything.
   * The routine silently malfunctions(程序会无声地发生故障) if given a NULL
   * set, howerver. If the "possible_subset" is larger than the "set", then the
   * extra bytes must be all zeros
   */

  _SETTYPE *subsetp, *setp;
  int lengthCommon; /* This many bytes in potential subset */
  int lengthTail;   /* This many implied 0 bytes in b */

  if (possible_subset->nwords > set->nwords) {
    lengthCommon = set->nwords;
    lengthTail = possible_subset->nwords - lengthCommon;
  } else {
    lengthCommon = possible_subset->nwords;
    lengthTail = 0;
  }

  subsetp = possible_subset->map;
  setp = set->map;

  for (; --lengthCommon >= 0; subsetp++, setp++)
    if ((*subsetp & *setp) != *subsetp)
      return 0;

  // make sure the rest of possible_subset is all zero, if , return 0
  while (--lengthTail >= 0)
    if (*subsetp++)
      return 0;

  return 1; // other situations excluded.
}

void _set_op(int op, SET *dest, SET *src) {
  /* Performs binary operations depending on op:
   *
   * _UNION:      dest = union of src and dest
   * _INTERSECT   dest = intersection(交集) of src and dest
   * _DIFFERENCE  dest = symmetric difference(对称差) of src and dest
   * _ASSIGN      dest = src
   *
   * The sizes of the destination set is adjusted so that it's the same
   * size as the source set
   */

  _SETTYPE *destp; /* Pointer to destination map */
  _SETTYPE *srcp;  /* Pointer to map in src */
  int sizeSrc;     /* Number of words in src set */
  int tailDest;    /* dest set is much bigger */

  sizeSrc = src->nwords;
  /* Make sure dest set is at least as big as the src dest */
  if (dest->nwords < sizeSrc)
    enlarge(sizeSrc, dest);

  tailDest = dest->nwords - sizeSrc;
  destp = dest->map;
  srcp = src->map;

  switch (op) {
  case _UNION:
    while (--sizeSrc >= 0)
      *destp++ |= *srcp++;
    break;
  case _INTERSECT:
    while (--sizeSrc >= 0)
      *destp++ &= *srcp++;
    while (--tailDest >= 0)
      *destp++ = 0;
    break;
  case _DIFFERENCE:
    while (--sizeSrc >= 0)
      *destp++ ^= *srcp++; // XOR: 1 if different, 0 if same
    break;
  case _ASSIGN:
    while (--sizeSrc >= 0)
      *destp++ = *srcp++;
    while (--tailDest >= 0)
      *destp++ = 0;
    break;
  }
}

void invert(SET *set) {
  /* Physically invert the bits in the set. Compare with the COMPLEMENT()
   * macro, which just modifies the complement bit
   */

  _SETTYPE *p, *end;

  for (p = set->map, end = p + set->nwords; p < end; p++)
    *p = ~*p; // all bits are reversed
}

void truncate(SET *set) {
  /* Clears the set but also set it back to the original,default size.
   * Compare this routine to the CLEAR() macro which clears all the bits
   * int the map but doesn't modify the size.
   */

  if (set->map != set->defmap) {
    free(set->map);
    set->map = set->defmap;
  }
  set->nwords = _DEFWORDS;
  set->nbits = _DEFBITS;
  memset(set->defmap, 0, sizeof(set->defmap));
}

int next_member(SET *set) {
  /* access all element of a set sequentially*/
  /* -1 if there are no more elements */
  /* set == NULL                      Reset
   * set changed from last call       Reset and return first element
   * otherwise                        return next element or -1 if none
   */
  static SET *lastCallSet = NULL; /* "set" arg in last call */
  static int current_member = 0;  /* last-accessed member of cur set */
  _SETTYPE *mapp;

  if (!set)
    /* assignment statement return the value of left operand
     * address 8 bytes and int 4 bytes */
    return ((long)(lastCallSet = NULL));

  if (lastCallSet != set) {
    lastCallSet = set;
    current_member = 0;

    /*skipping initial words in the bit map that are all zeros*/
    for (mapp = set->map; *mapp == 0 && current_member < set->nbits; ++mapp)
      current_member += _BITS_IN_WORD;
  }
  /* The increment must be put into the test because, if the TEST() invocation
   * evaluates true, then an increment on the right of a for() statement would
   * never be executed
   */

  while (current_member++ < set->nbits)
    /* determine whether the corresponding bit is zero */
    if (MTEST(set, current_member - 1))
      return current_member - 1;

  return -1;
}

void pset(SET *set) {
  /* Print the contents of the set bit map in human-readable form.
   * The output routine is called for each element of the set with
   * the following arguments:
   *
   * (*out)(param,"null",-1);    Null set ("set" arg == NULL)
   * (*out)(param,"empty",-2);   Empty set (no elements)
   * (*out)(param,"%d ",N);      N is an element of the set
   */

  int i, did_something = 0;

  if (!set)
    fprintf(stdout, "null");
  else {
    next_member(NULL);
    while ((i = next_member(set)) >= 0) {
      did_something++;
      fprintf(stdout, "%d ", i);
    }
    next_member(NULL);

    if (!did_something)
      fprintf(stdout, "empty");
  }
}
