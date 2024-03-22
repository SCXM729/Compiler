#include "hash.h"
#include "compiler.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void *newsym(int size) {
  /* Allocate space for new symbol,return a pointer to the user space */

  BUCKET *sym;

  if (!(sym = (BUCKET *)calloc(sizeof(BUCKET) + size, 1))) {
    fprintf(stderr, "Can't get memory for BUCKET\n");
    exit(1);
  }
  return (void *)(sym + 1); /* return pointer to user space */
}

void freesym(void *sym) {
  /* backs up the sym pointer to its original position */
  free((BUCKET *)sym - 1);
}

HASH_TAB *maketab(unsigned maxsym, unsigned (*hash_function)(void *),
                  int (*cmp_function)(void *, void *)) {
  /* Make a hash table of the indicated size */

  HASH_TAB *p;

  if (!maxsym)
    maxsym = 127;
  // clang-format off
                              /* |<---- header ---->||<---space for table ---->|*/
  if ((p = (HASH_TAB *)calloc(1,   sizeof(HASH_TAB) + maxsym * sizeof(BUCKET *)))){
    /* take advantage of cpp doesn't check array-boundary */
    // clang-format on
    p->size = maxsym;
    p->numsyms = 0;
    p->hash = hash_function;
    p->cmp = cmp_function;
  } else {
    fprintf(stderr, "insufficent memory for symbol table\n");
    exit(1);
  }
  return p;
}

void *addsym(HASH_TAB *tabp, void *isym) {
  /* Add a symbol to the hash table */

  BUCKET **p, *tmp;
  BUCKET *sym = (BUCKET *)(isym);

  // calculate the hash value and then take the remainder
  // suffix --
  p = &(tabp->table)[(*tabp->hash)(sym--) % tabp->size]; /* sym decrement */

  /* link the new node to the head of a chain found at the previously computed
   * array element. */
  tmp = *p;
  *p = sym;

  /* forward point to itself (sym==*sym->prev) */
  sym->prev = p; // because it is first element.
  sym->next = tmp;

  if (tmp) // skip when tmp is NULL
    tmp->prev = &sym->next;

  tabp->numsyms++;
  return (void *)(sym + 1);
}

void delsym(HASH_TAB *tabp, void *isym) {
  /* Remove a symbol from the hash table."sym" is pointer returned
   * from a previous findsym() call.It points initially at the user
   * space,but is decrement to get at the BUCKET header
   */

  BUCKET *sym = (BUCKET *)isym;

  if (tabp && sym) {
    --tabp->numsyms;
    --sym;

    /* The if is required because the next pointer is NULL on the last node in
    the chain. */
    if ((*(sym->prev) = sym->next))
      sym->next->prev = sym->prev;
    //  freesym(sym+1); test it later.
  }
}

void *findsym(HASH_TAB *tabp, void *sym) {
  /* Return a pointer to the hash table element having a
   * particular name or NULL if the name isn't in the table
   */
  BUCKET *p;
  if (!tabp) /*table empty*/
    return NULL;
  p = (tabp->table)[(*tabp->hash)(sym) % tabp->size];

  while (p && (*tabp->cmp)(sym, p + 1)) // p+1 point to content
    p = p->next;

  return (void *)(p ? p + 1 : NULL); // if p is NULL, return NULL
}

void *nextsym(HASH_TAB *tabp, void *i_last) {
  /* Return a pointer the next node in the current chain that has the same key
   * as the last node found (or NULL if there is no such node). "last" is a
   * pointer returned from a previous findsym() of nextsym() call
   */
  BUCKET *last = (BUCKET *)i_last;
  for (--last; last->next; last = last->next)
    if (tabp->cmp(last + 1, last->next + 1) == 0) /* key match */
      return (char *)(last->next + 1);
  return NULL;
}

static int (*User_cmp)(void *, void *);

int ptab(HASH_TAB *tabp, void (*print)(void *, void *), void *param, int sort) {
  /* tabp: pointer to the Table
   * print: print function used for output
   * param: parameter passed to print cmp_function
   * sort: Sort the table if ture.
   */
  /* Return 0 if a sorted table can't be printed because of insufficient
   * memory, else return 1 if the table was printed or the table is empty. The
   * print function is called with two arguments:
   *   (*print)(sym,param)
   *
   *   Sym is a pointer to a BUCKET user area and param is the third
   *   argument to ptab.
   */

  BUCKET **outtab, **outp, *sym, **symtab;
  int internal_cmp(BUCKET **, BUCKET **);
  int i;

  if (!tabp || tabp->size == 0) /* Table is empty. */
    return 1;

  if (!sort) {
    for (symtab = tabp->table, i = tabp->size; --i >= 0; symtab++) {
      /* Print all symbols in the current chain. The +1 in the print call
       * increments the pointer to the applications area of the bucket.
       */
      for (sym = *symtab; sym; sym = sym->next)
        (*print)(sym + 1, param);
    }
  } else {
    /* handle sorted arrays. Allocate memory for the outtab, an array of
     * pointers to BUCKETS, and initialize it. The outtab is different from the
     * actual hash table in that every outtab element points to a single BUCKET
     * structure, rather than to a linked list of them.
     */

    if (!(outtab = (BUCKET **)malloc(tabp->numsyms * sizeof(BUCKET *))))
      return 0;

    // outtab array point at every BUCKET in the table with the loop
    outp = outtab;

    for (symtab = tabp->table, i = tabp->size; --i >= 0; symtab++)
      for (sym = *symtab; sym; sym = sym->next) {
        if (outp > outtab + tabp->numsyms) {
          fprintf(stderr, "Internal error [ptab], table overfolw");
          exit(1);
        }
        *outp++ = sym;
      }
    /* Sort the outtab and then print it. The (*outp)+1 in the print call
     * increments the pointer past the header part of the BUCKET structure.
     * During sorting, the increment is done in internal_cmp.
     */

    User_cmp = tabp->cmp;
    assort((void **)outtab, (int)tabp->numsyms, (int)sizeof(BUCKET *),
           (int (*)(void **, void **))internal_cmp);

    for (outp = outtab, i = tabp->numsyms; --i >= 0; outp++)
      (*print)((*outp) + 1, param);

    free(outtab);
  }
  return 1;
}

int internal_cmp(BUCKET **p1, BUCKET **p2) {
  return (*User_cmp)(*p1 + 1, *p2 + 1);
}

/* hash function: just add together characters in the name */
unsigned hash_add(void *name) {
  unsigned char *bname = (unsigned char *)name;
  unsigned h;
  for (h = 0; *bname; h += *bname++)
    ;
  return h;
}

/* The algorithm used a shift-and-XOR strategy to randomize the input key. The
 * main iteration of loop shifts the accumulated hash value to the left by a few
 * bits and adds in the current character. When the number gets too large, it is
 * randomized by XORing it with a shifted version of itself.
 */
unsigned hash_pjw(void *name) {
  unsigned char *bname = (unsigned char *)name;
  unsigned h = 0; /* hash value. */
  unsigned g;

  int high_bits = ~((unsigned)(~0) >> 4);
  for (; *bname; ++bname) {
    /* because of upward compatibility of data currency, h's lowest 8 bits was
     * changed first then left shift advance 4 bits every loop. */
    h = (h << 4) + *bname;
    /* If the highest 4 bits was used, then use XORing to randomize the h(no
     * rule just random) and keep the highest 4 bits is unused.*/
    if ((g = h & high_bits))
      h = (h ^ (g >> 24)) & ~high_bits;
  }
  return h;
}

int sample_cmp(void *a1, void *a2) {
  char *b1 = (char *)a1;
  char *b2 = (char *)a2;
  return strcmp(b1,b2);
}
