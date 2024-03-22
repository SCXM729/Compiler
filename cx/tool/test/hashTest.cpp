#include "../hash.h"
#include "../compiler.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <random>
/* Used by pstat, max number */
#define MAXLEN 128

/*----------------------------------------------------------------------
 * The following test routines exercise the hash functions by building a table
 * consisting of either 500 words comprised of random letters (if RANDOM is
 * defined) or by collecting keywords and variable names from standard input.
 * Statistics are then printed showing the execution time, the various collision
 * chain lengths and the mean chain length (and deviation from the mean). If
 * you're using real words, the usage is:
 *		cat file ... | hash  	or 	hash < file
 */

typedef struct {
  char name[32]; /* hash key */
  char str[16];  /* used for error checking */
  unsigned hval; // Hash value of name, also " */
  int count;     /* # of times word was encoutered */
} STAB;

void pstabs(HASH_TAB *tabp) {
  /* print out various statistics showing the lengths of the chains (number of
   * collections) along with the mean depth of non-empty chains, standard
   * deviation
   * */
  BUCKET *p;           // pointer to current hash element
  int i;               /* counter */
  int chain_len;       /* length to current hash element */
  int maxlen = 0;      /* maxmum chain length */
  int minlen = MAXLEN; /* minium chain length */
  int lengths[MAXLEN]; /* indexed by chain length, holds the # of chains of that
                         length */

  int longer = 0; /* # of chains longer than MAXLEN */

  /* mean chain length and standard deviation from same */
  double m, d;

  if (tabp->numsyms == 0) {
    printf("Table is empty");
    return;
  }
  mean(1, 0.0, &d);

  memset(lengths, 0, sizeof(lengths));

  for (i = 0; i < tabp->size; i++) {
    chain_len = 0;

    for (p = tabp->table[i]; p; p = p->next)
      chain_len++;

    if (chain_len >= MAXLEN)
      ++longer;
    else
      ++lengths[chain_len];

    minlen = std::min(minlen, chain_len);
    maxlen = std::max(maxlen, chain_len);

    if (chain_len != 0)
      m = mean(0, (double)chain_len, &d);
  }

  printf("%d entries in %d element hash table, ", tabp->numsyms, tabp->size);
  printf("%d (%1.0f%%) empty.\n", lengths[0],
         ((double)lengths[0] / tabp->size) * 100);

  printf("Mean chain length (excluding zero-length chains): %g\n", m);
  printf("\t\tmax=%d, min=%d,standard deviation=%g\n", maxlen, minlen, d);

  for (i = 0; i < MAXLEN; i++)
    if (lengths[i])
      printf("%3d chains of length %d\n", lengths[i], i);

  if (longer)
    printf("%3d chains of length %d or longer\n", longer, MAXLEN);
}

void dptab(HASH_TAB *addr) {
  /* using %x to print pointers. Change this to %p in the compact of large
   * models
   */

  BUCKET **p, *bukp;
  int i;

  printf("HASH_TAB at %p (%d element table, %d symbols)\n", addr, addr->size,
         addr->numsyms);

  for (p = addr->table, i = 0; i < addr->size; ++p, ++i) {
    if (!*p)
      continue;
    printf("Htab[%3d] @ %p:\n", i, p);

    for (bukp = *p; bukp; bukp = bukp->next) {
      printf("\t\t%p prev=%p next=%p user=%p", bukp, bukp->prev, bukp->next,
             bukp + 1);

      printf(" (%s)\n", ((STAB *)(bukp + 1))->name);
    }
    putchar('\r');
  }
}

int getword(char *buf) {
#ifndef MSTDOUT
  /* generate 500 random words */
  static int wordnum = 500;
  int num_letters, let;

  if (--wordnum < 0)
    return 0;

  /* make sure num_letters < 48 */
  // while ((num_letters = rand()) > 48)
  // ;
  std::random_device generator;
  std::uniform_int_distribution<int> distribution(1, 40);
  int num_letter = distribution(generator);

  while (--num_letter >= 0) {
    let = (rand() % 26) + 'a';
    *buf++ = (rand() % 10) ? let : toupper(let);
  }

#else
  int c;

  while ((c = getchar()) != EOF) {
    /* skip up to the beginning of the next word, ignoring the contents of all
     * comments.
     * */
    if (isalpha(c) || c == '_')
      break;

    if (c == '/') {
      if ((c = getchar()) != '*') {
        if (c == EOF)
          break;

        ungetc(c, stdin);
        continue;
      } else {
        while ((c = getchar()) != EOF)
          if (c == '*' && getchar() == '/')
            break;
      }
    }
  }

  if (c == EOF)
    return 0;
  else {
    *buf++ = c;
    while ((c = getchar()) != EOF && (isalnum(c) || c == '_'))
      *buf++ = c;
  }
#endif
  *buf = '\0';
  return 1;
}

int mstrcmp(void *p1, void *p2) {
  char *a1 = (char *)p1;
  char *a2 = (char *)p2;
  return strcmp(a1, a2);
}
int main(int argc, char *argv[]) {
  // char word[80];
  // STAB *sp;
  HASH_TAB *tabp;
  //
  // clock_t start = clock();
  //
  // double time1, time2;
  int c;
  //
  // /* default use hash_add (p for hash_pjw) */
  c = (argc > 1) ? argv[1][0] : 'a';
  //
  printf("\n Using %s\n", c == 'a' ? "hash_add" : "hash_pjw");
  tabp = maketab(127, c == 'a' ? hash_add : hash_pjw, mstrcmp);
  //
  // while (getword(word)) {
  //   if ((sp = (STAB *)findsym(tabp, word))) {
  //     if (strcmp(sp->str, "123456789abcdef") || (sp->hval != hash_add(word)))
  //     {
  //       printf("node has been adulterated\n");
  //       exit(1);
  //     }
  //     sp->count++;
  //   } else {
  //     sp = (STAB *)newsym(sizeof(STAB));
  //     strncpy(sp->name, word, 32);
  //     strcpy(sp->str, "123456789abcdef");
  //     sp->hval = hash_add(word);
  //     addsym(tabp, sp);
  //     sp->count = 1;
  //   }
  // }
  //
  // clock_t end = clock();
  // printf("Elapsed time =%lf seconds\n", (double)(end - start) /
  // CLOCKS_PER_SEC);
  //
  // pstabs(tabp);

  const char *str1 = "asdf";
  const char *str2 = "asda";
  STAB *sp1 = (STAB *)newsym(sizeof(STAB));
  strncpy(sp1->name, str1, 32);
  addsym(tabp, sp1);

  printf("1");
  STAB *sp2 = (STAB *)newsym(sizeof(STAB));
  strncpy(sp2->name, str2, 32);
  addsym(tabp, sp2);

  // pstabs(tabp);

  const char*str3="aaaa";
  STAB *test = (STAB *)findsym(tabp, (void *)str2);
  if (test == NULL) {
    printf("couldn't find");
    printf("end");
  }
  printf("%p",test);
}
