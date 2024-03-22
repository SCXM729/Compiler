#include "set.h"
#include <bitset>
#include <cstddef>
#include <cstring>
#include <stack>
#include <utility>
Set::Set() {
  // std::cout << "Set()" << std::endl;
  map = new std::byte[nbytes];
  for (size_t i{0}; i < nbytes; i++) {
    map[i] = std::byte(0b00000000);
  }
}
Set::Set(std::initializer_list<size_t> elements) : Set{} {
  for (auto i : elements) {
    // std::cout << i << " ";
    add(i);
  }
}
Set::~Set() {
  // std::cout << "~Set()" << std::endl;
  delete[] map;
}
Set::Set(const Set &src)
    : ccompl{src.ccompl}, nbytes{src.nbytes}, nbits{src.nbits} {
  map = new std::byte[nbytes];
  memcpy(map, src.map, nbytes * sizeof(std::byte));
}
void Set::swap(Set &other) noexcept {
  std::swap(map, other.map);
  std::swap(nbytes, other.nbytes);
  std::swap(nbits, other.nbits);
  std::swap(ccompl, other.ccompl);
  std::swap(capacity, other.capacity);
}
Set &Set::operator=(const Set &rhs) {
  Set temp{rhs};
  swap(temp);
  return *this;
}

void Set::cleanup() noexcept {
  delete[] map;
  map = nullptr;
}
void Set::moveFrom(Set &src) noexcept {
  nbits = std::exchange(src.nbits, 0);
  nbytes = std::exchange(src.nbytes, 0);
  capacity = std::exchange(src.capacity, 0);
  ccompl = std::exchange(src.ccompl, false);
  map = std::exchange(src.map, nullptr);
}
Set::Set(Set &&src) noexcept {
  // std::cout << "move constructor" << std::endl;
  moveFrom(src);
}
Set &Set::operator=(Set &&rhs) noexcept {
  // std::cout << "move assignment operator" << std::endl;
  if (this == &rhs)
    return *this;
  cleanup();
  moveFrom(rhs);
  return *this;
}
void Set::enlarge(size_t needBytes) {
  if (nbytes > needBytes)
    return;
  else {
    std::byte *newmap = new std::byte[needBytes];
    memcpy(newmap, map, nbytes * sizeof(std ::byte));

    delete map;
    map = newmap;
  }
}
[[nodiscard]] bool Set::operator==(const Set &rhs) const {
  size_t minBytes = std::min(nbytes, rhs.nbytes);
  size_t maxBytes = std::max(nbytes, rhs.nbytes);

  // enlarge(maxBytes);
  // rhs.enlarge(maxBytes);

  for (size_t i{0}; i < minBytes; i++)
    if ((map[i] ^ rhs.map[i]) != std::byte{0})
      return false;

  for (size_t i{minBytes}; i < maxBytes; i++) {

    if (nbytes < rhs.nbytes) {
      while (i++ < maxBytes) {
        if (rhs.map[i] != std::byte{0b00000000})
          return false;
      }
    } else {
      while (i++ < maxBytes) {
        if (map[i] != std::byte(0b00000000))
          return false;
      }
    }
  }
  return true;
}

// // a naive impletation
// Set &Set::operator=(const Set &rhs) {
//   if (this == &rhs)
//     return *this;
//
//   delete[] map;
//   map = new std::byte[rhs.nbytes];
//   memcpy(map, rhs.map, rhs.nbytes * sizeof(std::byte));
//
//   nbytes = rhs.nbytes;
//   capacity = rhs.capacity;
//   nbits = rhs.nbits;
//   ccompl = rhs.ccompl;
//
//   return *this;
// }

/*-------------------------------------------------------------------*/

void Set::clear() { memset(map, 0, nbytes * sizeof(std::byte)); }
void Set::fill() { memset(map, ~0, nbytes * sizeof(std::byte)); }
void Set::invert() {
  for (std::byte *imap = map; imap <= &map[nbytes - 1]; imap++)
    *imap = ~*imap;
}

void Set::add(size_t position) {
  size_t index = position / 8; // the elements in which index
  std::byte offset =
      std::byte{0b10000000} >>
      position % 8; // a byte only have 1 bit that offset from array boundary
                    //
  if (position < capacity)
    map[index] |= offset;
  else {
    /* increment 16 bytes (128 bits) everytime when the position > nbits */
    size_t needBytes = 16 * (position / 128 + 1);
    // std::cout << "the needBytes is " << needBytes << std::endl;

    std::byte *newmap = new std::byte[needBytes];
    memcpy(newmap, map, nbytes * sizeof(std::byte));
    memset(newmap + nbytes, 0, (needBytes - nbytes) * sizeof(std::byte));

    // clean up original source if allocating memory for map
    delete[] map;
    map = newmap;

    map[index] |= offset;

    nbytes = needBytes;
    capacity += 128;
    nbits++;
  }
}

/* determine whether it is in the set or not */
bool Set::isMember(size_t position) const {
  /* if position greater than capacity, just return false */
  if (position >= capacity) {
    // throw std::out_of_range("position greater than nbits");
    return false;
  }

  int index = position / 8;
  // std::cout << "the index is " << index << std::endl;
  // std::cout << "the nbytes is " << nbytes << std::endl;
  // std::cout << "the capacity is " << capacity << std::endl;
  std::byte offset = std::byte{0b10000000} >> position % 8;

  return bool(map[index] & offset);
}

/* return the total bit member in the set */
size_t Set::numEle() const {
  int count = 0;
  for (std::byte *imap = map; imap <= &map[nbytes - 1]; imap++) {
    count += std::popcount(std::to_integer<unsigned>(*imap));
  }
  return count;
}

/* remove one bit that in the postion */
void Set::remove(size_t position) {
  if (position > capacity)
    throw std::out_of_range("the bit to removed is much bigger");

  size_t index = position / 8;
  std::byte offset{~(std::byte{0b10000000} >> (position % 8))};
  map[index] &= offset;
}

/* print all bits with it position */
void Set::psets() const {
  int index = 0;
  int did_something = 0;
  for (std::byte *imap = map; imap <= &map[nbytes - 1]; imap++) {
    if (*imap == std::byte{0}) {
      index++;
      continue;
    } else {
      std::byte m = *imap;
      for (int i = 0; i < 8; i++) {
        if (bool(m & std::byte{0b10000000})) {
          did_something++;
          printf("%d ", index * 8 + i);
        }
        m <<= 1;
      }
      index++;
    }
  }
  if (did_something == 0)
    std::cout << "empty" << std::endl;
}
/* print all bits with it position */
std::stack<size_t> Set::nextMember() const {
  std::stack<size_t> sstack;
  int index = 0;
  int did_something = 0;
  for (std::byte *imap = map; imap <= &map[nbytes - 1]; imap++) {
    if (*imap == std::byte{0}) {
      index++;
      continue;
    } else {
      std::byte m = *imap;
      for (int i = 0; i < 8; i++) {
        if (bool(m & std::byte{0b10000000})) {
          did_something++;
          sstack.push(index * 8 + i);
          // printf("%d ", index * 8 + i);
        }
        m <<= 1;
      }
      index++;
    }
  }
  if (did_something == 0)
    std::cout << "empty" << std::endl;
  return sstack;
}
/* print the original bits */
std::ostream &operator<<(std::ostream &os, const Set &set) {
  for (size_t i{0}; i < set.nbytes; i++) {
    os << std::bitset<sizeof(std::byte) * 8>(std::to_integer<int>(set.map[i]))
       << (((i + 1) % 4 == 0) ? "" : "-");
    if ((i + 1) % 4 == 0)
      std::cout << std::endl;
  }

  return os;
}
