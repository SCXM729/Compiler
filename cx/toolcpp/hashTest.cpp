#include "hash.hpp"
#include "gtest/gtest.h"
#include <format>
#include <iostream>
#include <random>
#include <typeinfo>
#include <vector>
struct Stab {
  // Stab(std::string_view name) : name{name} {}
  // Stab(Stab &&other) noexcept {
  //   name = std::move(other.name);
  //   str = std::move(other.str);
  //   std::swap(hval, other.hval);
  //   std::swap(count, other.count);
  // }
  std::string name;  /* hash key */
  std::string str{}; /* used for error checking */
  int hval;          /* hash value for name */
  int count;         /* of times word was encountered */
};
template <typename Data> void print_Stab(Data *data) {
  std::cout << std::format("Name({}) ", data->name);
  // std::cout << "the address is " << data << std::endl;
}

std::string getword() {

  int let;
  std::string str;

  std::random_device generator;
  /* in [1,10] */
  std::uniform_int_distribution<int> distribution(2, 10);
  int num_letter = distribution(generator);

  while (--num_letter >= 0) {
    std::random_device generator;
    std::uniform_int_distribution<int> distribution(0, 25);
    int c = distribution(generator);
    let = ((int)(c * 2.5) % 26) + 'a';
    str += ((c >= 5) ? let : toupper(let));
  }

  return str;
}

TEST(hash, con) {

  Hash<Stab> myhash{10, hash_add, simpleCmp};
  Stab s1{"W"};
  myhash.addsym(s1);
  myhash.addsym(s1);
  myhash.addsym(s1);
  myhash.ptab(print_Stab);
  std::cout << "/*---------------------------------------------*/" << std::endl;

  /* copy constructor */
  Hash<Stab> myhash2{myhash};
  myhash2.addsym(s1);
  myhash2.ptab(print_Stab);
  std::cout << "/*---------------------------------------------*/" << std::endl;
  std::vector<std::string> vec;
  for (int i = 0; i < 10; i++) { /* test addsym */
    std::string str = getword();
    vec.push_back(str);
    std::cout << std::format(" add {}", str);
    Stab s1{.name = str};
    myhash2.addsym(s1);
  }
  std::cout << std::endl;
  myhash2.ptab(print_Stab);
  std::cout << "/*---------------------------------------------*/" << std::endl;
  Stab f1{.name = vec.at(5)};
  auto r1 = myhash2.findsym(f1);
  if (r1.has_value())
    std::cout << "find it " << r1.value()->name << std::endl;

  Stab f2{.name = "W"};
  auto r2 = myhash2.findsym(f2);
  if (r2.has_value())
    std::cout << "find it " << r2.value()->name << std::endl;

  EXPECT_EQ(myhash2.getNumSyms(), (size_t)14);
  Hash myhash3{std::move(myhash2)};
  std::cout << "/*-----------------------------------------------*/ "
            << std::endl;

  myhash3.ptab(print_Stab);
  std::cout << "/*-----------------------------------------------*/ "
            << std::endl;
  myhash3.delsym(s1);
  myhash3.ptab(print_Stab);
  EXPECT_EQ(myhash3.isEmpty(), false);

  std::cout << "/*-----------------------------------------------*/ "
            << std::endl;
  auto n1 = myhash3.findsym(s1);
  if (n1.has_value())
    std::cout << "find it " << n1.value()->name << std::endl;
  for (int i = 0; i < 10; i++) {
    auto n2 = myhash3.nextsym(s1);
    if (n2.has_value())
      std::cout << "find it " << n2.value()->name << std::endl;
  }
  myhash3.pstatics();
}
