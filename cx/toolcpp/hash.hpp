#pragma once
#include "compiler.h"
#include <cstring>
#include <format>
#include <optional>
#include <utility>

#include <iostream>
#include <string>
#include <unistd.h>

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

/* restirct it to have key member that is std::string type */
template <typename dataType>
concept hasNameMember = requires(dataType t) {
  { t.name + "a" } -> std::same_as<std::string>;
  t.name;
};

/* restirct it to have name member that is std::string type */
template <typename dataType>
concept hasKeyMember = requires(dataType t) {
  { t.key + "a" } -> std::same_as<std::string>;
  t.name;
};

/* restirct it to have key or name member that is std::string type */
template <typename T>
  requires hasNameMember<T> || hasKeyMember<T>
struct Bucket {
  Bucket *next = nullptr;
  Bucket *prev = nullptr;
  T data;
};

template <typename Data> class Hash {
public:
  Hash() = default;
  Hash(size_t maxsym, unsigned (*hash)(const Data &),
       int (*cmp)(const Data &, const Data &));

  Hash(const Hash<Data> &src);
  void swap(Hash<Data> &other) noexcept;
  Hash &operator=(const Hash<Data> &rhs);

  Hash(Hash &&src) noexcept;
  Hash &operator=(Hash &&rhs) noexcept;

  ~Hash();

private:
  void moveFrom(Hash &src) noexcept;
  void cleanup() noexcept;

public:
  void addsym(const Data &);
  void delsym(const Data &data);
  std::optional<const Data *> findsym(const Data &data);
  std::optional<const Data *> nextsym(const Data &isym);
  bool isEmpty() const;

  template <typename paramType>
  int ptab(void (*print)(Data *data, paramType), paramType param,
           bool sort) const;

  int ptab(void (*print)(Data *data)) const;
  void pstatics() const;

  size_t getSize() const { return size; }
  size_t getNumSyms() const { return numsyms; }

  // private:
  size_t size{}; /* maxnumber of elements in table */
  size_t numsyms = 127;
  unsigned (*hash)(const Data &);
  int (*cmp)(const Data &, const Data &);
  Bucket<Data> **table{};
  Bucket<Data> *nextTmp;
};

/*---------------------------------------------------------------------------------*/

template <typename Data>
Hash<Data>::Hash(size_t size, unsigned (*hash)(const Data &),
                 int (*cmp)(const Data &, const Data &))
    : size{size}, numsyms{0}, hash{hash}, cmp{cmp} {
  table = new Bucket<Data> *[size];

  for (size_t i = 0; i < size; i++)
    table[i] = nullptr;
}

template <typename Data>
Hash<Data>::Hash(const Hash<Data> &src) : Hash{src.size, src.hash, src.cmp} {
  numsyms = src.numsyms;

  /* travser hash table */
  for (size_t i = 0; i < size; i++) {

    /* skip when table[i] is nullptr */
    if (src.table[i] != nullptr) {

      Bucket<Data> **head = &table[i];
      *head = new Bucket<Data>{}; /* alloc memory for sentry */

      Bucket<Data> *tmp{(*head)};
      /* copy  list at table[i] that have contents */
      for (Bucket<Data> *p = src.table[i]->next; p; p = p->next) {
        Bucket<Data> *toStore = new Bucket<Data>{.data = p->data};

        tmp->next = toStore;

        toStore->prev = tmp;
        toStore->next = nullptr;

        tmp = tmp->next;
      }
    }
  }
}

/* swap all data members */
template <typename Data> void Hash<Data>::swap(Hash<Data> &other) noexcept {
  std::swap(size, other.size);
  std::swap(numsyms, other.numsyms);
  std::swap(hash, other.hash);
  std::swap(cmp, other.cmp);
  std::swap(table, other.table);
}
/* assignment operator (use copy-and-swap idiom) */
template <typename Data>
Hash<Data> &Hash<Data>::operator=(const Hash<Data> &other) {
  Hash<Data> tmp{other};
  swap(tmp);
  return *this;
}
/* move src's resource for move senmantics */
template <typename Data> void Hash<Data>::moveFrom(Hash<Data> &src) noexcept {
  size = std::exchange(src.size, 0);
  numsyms = std::exchange(src.numsyms, 0);
  hash = std::exchange(src.hash, nullptr);
  cmp = std::exchange(src.cmp, nullptr);
  table = std::exchange(src.table, nullptr);
}
/* move constructor */
template <typename Data> Hash<Data>::Hash(Hash &&src) noexcept {
  moveFrom(src);
}
/* clean up all resource */
template <typename Data> void Hash<Data>::cleanup() noexcept {
  for (size_t i = 0; i < size; i++) {
    Bucket<Data> *current = nullptr;
    Bucket<Data> *tmp = nullptr;

    if (table[i] == nullptr) {
      continue;
    }

    for (current = table[i]; current;) {
      tmp = current->next;
      delete current;
      numsyms--;
      current = tmp;
    }
  }
  delete[] table;
}
/* move assignment operator */
template <typename Data>
Hash<Data> &Hash<Data>::operator=(Hash &&rhs) noexcept {
  if (this == &rhs)
    return *this;
  cleanup();
  moveFrom(rhs);
  return *this;
}
template <typename Data> Hash<Data>::~Hash() { cleanup(); }

/*-------------------------------------------------------------------------------*/

/* insert at the first position behind the sentry (head pointer) */
template <typename Data> void Hash<Data>::addsym(const Data &data) {
  Bucket<Data> *isym = new Bucket<Data>{.data = data};

  // Bucket<Data> *tmp;
  // Bucket<Data> *p;
  /* head sentry */
  Bucket<Data> **head{&(table)[hash(isym->data) % size]};

  /* first alloc for head sentry */
  if (*head == nullptr) {
    *head = new Bucket<Data>{};
  }

  /* store the first element after sentry */
  Bucket<Data> *tmp = (*head)->next;
  (*head)->next = isym; /* change the head direction to isym */

  isym->prev = *head;
  isym->next = tmp;

  if (tmp != nullptr) /* change the item's prev */
    tmp->prev = isym;

  numsyms++;
}

template <typename Data> void Hash<Data>::delsym(const Data &data) {
  /* remove a symbol from the hash table. "isym" is pointer returned from
   * a previous findsym() call
   */
  if (numsyms == 0) {
    throw "the table is empty, delete something is wrong";
  }

  auto p = table[hash(data) % size];
  if (p == nullptr)
    throw "not found";
  else
    p = p->next;

  /* find the element in the table list */
  while (p && (cmp(data, p->data) != 0))
    p = p->next;

  if (p == nullptr)
    throw "could find the element in the table";

  std::cout << std::format("find at index{}", hash(data) % size) << std::endl;
  if (p) {                    /* find it */
    if (p->next == nullptr) { /* the tail */
      p->prev->next = nullptr;
      delete p;
    } else { /* the middle */
      p->next->prev = p->prev;
      p->prev->next = p->next;
      delete p;
    }
  }
  --numsyms;
}
template <typename Data> bool Hash<Data>::isEmpty() const {
  return numsyms ? false : true;
}
template <typename Data>
std::optional<const Data *> Hash<Data>::findsym(const Data &data) {
  /* Return a pointer-to-const Data to the hash table element having a
   * particular name or std::nullopt if the name isn't in the table
   */
  Bucket<Data> isym{.data = data};
  Bucket<Data> *p;

  if (numsyms == 0) {
    return std::nullopt;
  }

  p = table[hash(data) % size];

  D(int inchain{-1});
  while (p && (cmp(data, p->data) != 0)) {
    D(inchain++);
    p = p->next;
  }

  if (p == nullptr)
    return std::nullopt;

  D(std::cout << std::format("find it at index {}, in_chain {}\n",
                             (hash(data) % size), inchain));
  nextTmp = p; /* for nextsym() function */
  return &p->data;
}

template <typename Data>
std::optional<const Data *> Hash<Data>::nextsym(const Data &data) {
  /*Return a std::optional.  return value is the next node (the data) in the
    current chain that has the same key as the last node found (or nullopt if
    there is no such node). "data" is an argument used by a previous
    findsym call
    */
  if (nextTmp == nullptr)
    return std::nullopt;

  nextTmp = nextTmp->next;
  while (nextTmp && (cmp(data, nextTmp->data) != 0)) {
    nextTmp = nextTmp->next;
  }

  if (nextTmp == nullptr)
    return std::nullopt;
  return &nextTmp->data;
}

template <typename D> void Hash<D>::pstatics() const {
  /* print out various statistics showing the lengths of the chains
   * (number of collections) along with the mean depth of non-empty
   * chains, standard deviation
   * */
  Bucket<D> *p;                 // pointer to current hash element
  size_t i;                     /* counter */
  int chain_len;                /* length to current hash element */
  int maxlen = 0;               /* maxmum chain length */
  int minlen = 128;             /* minium chain length */
  int *lengths = new int[size]; /* indexed by chain length, holds the # of
                         chains of that length */

  int longer = 0; /* # of chains longer than 128 */

  /* mean chain length and standard deviation from same */
  double m, d;

  if (numsyms == 0) {
    std::cout << "Table is empty" << std::endl;
    return;
  }

  mean(1, 0.0, &d);

  memset(lengths, 0, size * sizeof(int));

  for (i = 0; i < size; i++) {
    chain_len = 0;

    if (table[i] != nullptr && table[i]->next != nullptr)
      for (p = table[i]->next; p; p = p->next)
        chain_len++;

    if (chain_len >= 128)
      ++longer;
    else
      ++lengths[chain_len];

    minlen = std::min(minlen, chain_len);
    maxlen = std::max(maxlen, chain_len);

    if (chain_len != 0)
      m = mean(0, (double)chain_len, &d);
  }

  printf("%zd entries in %zd element hash table, ", numsyms, size);
  printf("%d (%1.0f%%) empty.\n", lengths[0],
         ((double)lengths[0] / size) * 100);

  printf("Mean chain length (excluding zero-length chains): %g\n", m);
  printf("\t\tmax=%d, min=%d,standard deviation=%g\n", maxlen, minlen, d);

  for (i = 0; i < size; i++)
    if (lengths[i])
      printf("%3d chains of length %zd\n", lengths[i], i);

  if (longer)
    printf("%3d chains of length %d or longer\n", longer, 128);

  delete[] lengths;
}

// template <typename D>
// template <typename paramType>
// int Hash<D>::ptab(void (*print)(Bucket<D> *data, paramType param),
//                   paramType param, bool sort) const {
//   /* sort: sort the table if true */
//
//   if (isEmpty()) {
//     std::cout << "the table is empty" << std::endl;
//     return 1;
//   }
//
//   int i;
//   Bucket<D> **symtab;
//   if (!sort) {
//     for (symtab = table, i = size; --i >= 0; symtab++) {
//       /* print all symbols in the current chain. */
//       for (Bucket<D> *sym = *symtab; sym; sym = sym->next)
//         print(sym, param);
//     }
//   } else {
//   }
// }
template <typename Data> int Hash<Data>::ptab(void (*print)(Data *data)) const {
  /* sort: sort the table if true */

  if (numsyms == 0) {
    std::cout << "the table is empty" << std::endl;
    return 1;
  }

  // D(std::cout << "the size is " << size << std::endl;)
  for (size_t i{0}; i < size; i++) {
    /* print all symbols in the current chain. */
    if (table[i] == nullptr) {
      std::cout << std::format("  index {} is nullptr", i) << std::endl;
      continue;
    }
    if (table[i])
      std::cout << std::format("  index {}: ", i);
    for (auto sym = table[i]->next; sym; sym = sym->next) {
      print(&sym->data);
      // std::cout << sym->prev << "(prev) ";
      // std::cout << sym << " ";
      // std::cout << sym->next << "(next) ";
    }
    std::cout << std::endl;
  }
  return 0;
}

/* the key is a string, then add every key's character together and return it */
template <typename Data> unsigned hash_add(const Data &data) {
  const std::string str = data.name;

  unsigned h = 0;
  for (size_t i = 0; i < str.size(); i++) {
    h += str.at(i);
  }
  return h;
}

#define higt_bits (~(unsigned)(~0) >> 4)
template <typename T> unsigned hash_pjw(T *data) {
  unsigned h = 0;

  unsigned g;

  std::string name = std::string{data->name};

  for (int i = 0; i < std::size(name); i++) {
    h = (h << 4) * name.at(i);

    if ((g = h & higt_bits))
      h = (h ^ (g >> 24)) & ~higt_bits;
  }
  return h;
}

/* just list strcmp or the overloading for std::string operator = */
template <typename Data> int simpleCmp(const Data &t1, const Data &t2) {
  if (t1.name == t2.name)
    return 0;
  if (t1.name > t2.name)
    return 1;
  return -1;
}
