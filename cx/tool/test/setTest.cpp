#include "../set.h"
#include "gtest/gtest.h"
#include <bitset>
int add(int a, int b) { return a + b; }
TEST(testCase, test1) { EXPECT_EQ(add(1, 2), 3); }
TEST(SetTest, test1) {
  SET *m_Set1 = newset();
  EXPECT_NE(m_Set1, nullptr);

  // ADD(m_Set1, 1);
  ADD(m_Set1, 20);

  SET *m_set2 = dupset(m_Set1);
  ADD(m_set2,72);
  ADD(m_Set1,73);
  pset(m_Set1);

  int _ = 12;
  printf("%d",_);
  /* for (int i = 0; i < 8; i++) {
    std::bitset<16> my(m_Set1->defmap[i]);
    std::cout << my; */
  // }
  /* puts("\n");
  pset(m_set2);
  EXPECT_EQ(IS_EQUIVALENT(m_Set1, m_set2), 0);
  EXPECT_NE(m_set2, nullptr); */
}
