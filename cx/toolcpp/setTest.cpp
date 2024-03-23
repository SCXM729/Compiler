#include "set.h"
#include "gtest/gtest.h"
TEST(set, defaultconstructor) {
  Set mySet1;
  EXPECT_EQ(mySet1.getCapacity(), (size_t)128);
  EXPECT_EQ(mySet1.getCcompl(), false);
  EXPECT_EQ(mySet1.getNbytes(), (size_t)16);
  mySet1.add(1);
  mySet1.add(10);
  mySet1.add(100);
  mySet1.add(150);
  mySet1.add(151);
  mySet1.add(152);
  // std::cout << mySet1;
  mySet1.invert();
  EXPECT_EQ(mySet1.test(1), false);
  mySet1.invert();
  // std::cout << mySet1 << std::endl;
  EXPECT_EQ(mySet1.test(1), true);
  EXPECT_EQ(mySet1.numEle(), size_t(6));
  mySet1.remove(100);
  // std::cout << mySet1;
  mySet1.remove(100);
  mySet1.remove(101);
  mySet1.remove(151);
  EXPECT_EQ(mySet1.numEle(), size_t(4));
  mySet1.psets();
  Set mySet2;
  mySet2.add(256);
  Set mySet3{1, 2, 190, 255};
  EXPECT_EQ(mySet3.numEle(), size_t(4));
  mySet3.remove(255);
  EXPECT_EQ(mySet3.numEle(), size_t(3));
  std::vector<Set> vec;
  for (size_t i = 0; i < 10; i++)
    mySet3 = Set{};
  std::cout << std::endl;
  // std::cout << std::format("{}", mySet3);
  bool rs{mySet1 == mySet2};
  EXPECT_EQ(rs, false);
  Set mySet4{mySet2};
  mySet4.add(230);
  EXPECT_EQ(mySet4 == mySet2, false);
  mySet4.remove(230);
  EXPECT_EQ(mySet4 == mySet2, true);
  Set mysetc1{5, 7, 8, 9, 11};
  mysetc1.add(3);
  Set mysetc2{3, 5, 7};
  mysetc2.add(8);
  mysetc2.add(9);
  mysetc2.add(11);
  mysetc1.psets();
  mysetc2.psets();
  if (mysetc1 == mysetc2)
    std::cout << "equal" << std::endl;

  EXPECT_EQ(mysetc2 == mysetc2, true);
  mysetc2.psets();
  std::cout << mysetc2.getNbits() << std::endl;
  EXPECT_EQ(mysetc2.is_empty(), false);
}
