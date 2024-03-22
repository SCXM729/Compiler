#include "compiler.h"
#include "gtest/gtest.h"
TEST(compiler, esc) {
  const char *str = "\\x1110";
  // str ="\\xxx";
  std::cout << str << std::endl;
  EXPECT_EQ(esc(str), 273);
  std::cout << str << std::endl;
}
