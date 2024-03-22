#ifndef __HASH_H
#define __HASH_H

typedef struct BUCKET { /* two pointer total 16 bytes */
  struct BUCKET *next;
  /* the head of chain is a simple pointer, not an entire BUCKET structure.The
   * backwards pointer for the leftmost node in the chain points at the
   * head-of-chain pointer. All other backwards pointer hold the address of the
   * next field from the previous node
   */

  struct BUCKET **prev;
} BUCKET;

typedef struct hash_tab_ {
  int size;                   /* Maxnumber of elements in table */
  int numsyms;                /* number of elements currently in table */
  unsigned (*hash)(void *);   /* hash function */
  int (*cmp)(void *, void *); /* comparsion funct,cmp(name,bucket_p); */
  BUCKET *table[1];           /* First element of actual hash table */
} HASH_TAB;

typedef void (*ptab_t)(void *, ...); /*print argument to ptab*/

HASH_TAB *maketab(unsigned maxsym, unsigned (*hash_function)(void *),
                  int (*cmp_function)(void *, void *));
void *newsym(int size);
void freesym(void *sym);
void *addsym(HASH_TAB *tabp, void *isym);
void *findsym(HASH_TAB *tabp, void *sym);
void *nextsym(HASH_TAB *tabp, void *last);
void delsym(HASH_TAB *tabp, void *isym);
int ptab(HASH_TAB *tabp, void (*print)(void *, void *), void *param, int sort);
unsigned hash_add(void *name);
unsigned hash_pjw(void *name);
int sample_cmp(void *a1, void *a2);
#endif
