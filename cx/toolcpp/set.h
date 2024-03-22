
#pragma once
#include <format>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>
class TestSet {};
class Set {
public:
  /* all data member are in-class initializer */
  Set();
  Set(std::initializer_list<size_t> elements);
  Set(const Set &src);
  Set &operator=(const Set &set);

  void swap(Set &other) noexcept;
  Set(Set &&src) noexcept;
  Set &operator=(Set &&rhs) noexcept;
  ~Set();

  friend std::ostream &operator<<(std::ostream &os, const Set &set);

public:
  [[nodiscard]] bool operator==(const Set &rhs) const;
  [[nodiscard]] std::strong_ordering operator<=>(const Set &rhs) const;

private:
  void cleanup() noexcept;
  void moveFrom(Set &src) noexcept;

public:
  /* == (dupset) */
  /* << (pset) */
  void cunion(const Set &src);
  void intersect(const Set &src);
  void difference(const Set &src);

  void clear();
  void fill();
  void ccomplement() { ccompl = !ccompl; }
  void invert();

  bool is_disjoint(const Set &src) const;
  bool is_intersecting(const Set &src) const;
  bool is_equivalent(const Set &src) const;
  bool is_empty() const;

  void add(size_t position);
  void remove(size_t position);

  /* logical test that determine whether the bit is in the set */
  bool test(size_t position) const {
    return isMember(position) ? !ccompl : ccompl;
  }
  size_t numEle() const;

  void psets() const;

  bool getCcompl() const { return ccompl; }
  size_t getNbytes() const { return nbytes; }
  size_t getCapacity() const { return capacity; }
  size_t getNbits() const { return nbits; }

  std::stack<size_t> nextMember() const;

  bool isMember(size_t position) const;

private:
  void enlarge(size_t size);
  bool ccompl{false};
  size_t nbytes{16};
  size_t capacity{nbytes * 8}; // default 128
  size_t nbits{0};
  std::byte *map{};
};

std::ostream &operator<<(std::ostream &os, const Set &set);
//
// template <> class std::formatter<Set> {
// public:
//   template <typename ParseContext>
//   constexpr ParseContext::iterator parse(ParseContext &context) {
//     std::string positionFormat, bitFormat;
//     auto iter{std::begin(context)};
//     for (; iter != std::end(context); ++iter) {
//       if (*iter == '}')
//         break;
//
//       switch (*iter) {
//       case 'p':
//         positions = true;
//         break;
//       case 'b':
//         bits = true;
//         break;
//       default:
//         throw std::format_error{"Invalid argument"};
//       }
//     }
//
//     return iter;
//   }
//
//   template <typename FmtContext>
//   FmtContext::iterator format(const Set &set, FmtContext &context) const {
//     if (positions == true)
//       context.advance_to(
//           m_positionFormatter.format(set.getAllMembers(), context));
//     if (bits == true) {
//       context.advance_to(m_bitsFormatter.format(set.getAllBits(), context));
//     }
//
//     return context.out();
//   }
//
// private:
//   std::formatter<std::string> m_positionFormatter;
//   std::formatter<std::string> m_bitsFormatter;
//   bool positions{false};
//   bool bits{false};
// };
