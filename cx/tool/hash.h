#ifndef __HASH_H
#define __HASH_H
#include "debug.h"

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
  int size;                   /*Maxnumber of elements in table */
  int numsyms;                /*number of elements currently in table*/
  unsigned (*hash)(void *);   /*hash function*/
  int (*cmp)(void *, void *); /*comparsion funct,cmp(name,bucket_p);*/
  BUCKET *table[1];           /*First element of actual hash table*/
} HASH_TAB;

typedef void (*ptab_t)(void *, ...); /*print argument to ptab*/

extern HASH_TAB *maketab P((unsigned maxsym, unsigned (*hash)(), int (*cmp)()));
extern void *newsym P((int size));
extern void freesym P((void *sym));
extern void *addsym P((HASH_TAB * tabp, void *sym));
extern void *findsym P((HASH_TAB * tabp, void *sym));
extern void *nextsym P((HASH_TAB * tabp, void *last));
extern void delsym P((HASH_TAB * tabp, void *sym));
extern int ptab P((HASH_TAB * tabp, void *sym));
unsigned hash_ass P((unsigned char *name)); /*in hashadd.c*/
unsigned hash_pjw P((unsigned char *name)); /*in hashpjw.c*/

#endif
