#include "hash.h"
#include "debug.h"
#include <cctype>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

PUBLIC void *newsym(int size) {
  /*Allocate space for new symbol,return a pointer to the user space*/

  BUCKET *sym;

  if (!(sym = (BUCKET *)calloc(size + sizeof(BUCKET), 1))) {
    fprintf(stderr, "Can't get memory for BUCKET\n");
    raise(SIGABRT);
    return NULL;
  }
  return (void *)(sym + 1); /*return pointer to user space*/
}

/*--------------------------------------------------------*/
PUBLIC void freesym(void *sym) {
  /* backs up the sym pointer to its original position*/
  free((BUCKET *)sym - 1);
}

PUBLIC HASH_TAB *maketab(unsigned maxsym, unsigned (*hash_function)(void *),
                         int (*cmp_function)(void *, void *)) {
  /*Make a hash table of the indicated size*/

  HASH_TAB *p;

  if (!maxsym)
    maxsym = 127;
  // clang-format off
                              /* |<---space for table ---->|<---- and header ---->|*/
  if ((p = (HASH_TAB *)calloc(1, (maxsym * sizeof(BUCKET *)) + sizeof(HASH_TAB)))) {
    /* take advantage of cpp doesn't check array-boundary */
    // clang-format on
    p->size = maxsym;
    p->numsyms = 0;
    p->hash = hash_function;
    p->cmp = cmp_function;
  } else {
    fprintf(stderr, "insufficent memory for symbol table\n");
    raise(SIGABRT);
    return NULL;
  }
  return p;
}

PUBLIC void *addsym(HASH_TAB *tabp, void *isym) {
  /*Add a symbol to the hash table*/
  
  BUCKET **p, *tmp;
  BUCKET *sym = (BUCKET *)(isym);
  
  p = &(tabp->table)[(*tabp->hash)(sym--) % tabp->size]; /* sym decrement */
  
  tmp = *p;
  *p = sym;
  sym->prev = p;
  sym->next = tmp;
  
  if (tmp)
    tmp->prev = &sym->next;
  
  tabp->numsyms++;
  return (void *)(sym + 1);
}

PUBLIC void delsym(HASH_TAB *tabp, void *isym) {
  /*Remove a symbol from the hash table."sym" is pointer returned
   * from a previous findsym() call.It points initially at the user
   * space,but is decrement to get at the BUCKET header
   */
  
  BUCKET *sym = (BUCKET *)isym;
  
  if (tabp && sym) {
    --tabp->numsyms;
    --sym;
  
    if (*(sym->prev) == sym->next)
      sym->next->prev = sym->prev;
  }
}

PUBLIC void *findsym(HASH_TAB *tabp, void *sym) {
  /*Return a pointer to the hash table element having a
   * particular name or NULL if the name isn't in the table
   */
  BUCKET *p;
  if (!tabp) /*table empty*/
    return NULL;
  p = (tabp->table)[(*tabp->hash)(sym) % tabp->size];
  while (p && (*tabp->cmp)(sym, p + 1))
    p = p->next;
  return (void *)(p ? p + 1 : NULL);
}

PUBLIC void *nextsym(HASH_TAB *tabp, void *i_last) {
  /*Return a pointer the next node in the current chain that
   * has the same key as the last node found (or NULL if there)
   * "last" is a pointer returned from a previous findsym()
   * of nextsym() call
   */
  BUCKET *last = (BUCKET *)i_last;
  for (--last; last->next; last = last->next)
    if (tabp->cmp)
}
