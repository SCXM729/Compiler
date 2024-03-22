#include "../../tool/compiler.h"
#include "gtest/gtest.h"
#include <cstring>

TEST(cstol, decimalNumber) {
  char *arr = (char *)"-123";
  char *arr2 = (char *)"123";
  char *arr3 = (char *)" 1.2";
  char *arr4 = (char *)"-1.2";
  EXPECT_EQ(cstol(&arr), -123);
  EXPECT_EQ(cstol(&arr2), 123);
  EXPECT_EQ(cstol(&arr3), 1);
  EXPECT_EQ(cstol(&arr4), -1);
}

TEST(cstol, hexNumber) {
  char *arr = (char *)"-0x12";
  EXPECT_EQ(cstol(&arr), -18);

  char *arr2 = (char *)"-0x23";
  EXPECT_EQ(cstol(&arr2), -35);
}

TEST(cstol, octNumber) {
  char *arr = (char *)"023";
  EXPECT_EQ(cstol(&arr), 19);
}

TEST(esc, normalEsc) {
  char *arr = (char *)"\\f";
  EXPECT_EQ(esc(&arr), 12);
  /* the *arr point at the character after "f" **/
  // std::cout << "esc(&arr) " << esc(&arr) << std::endl;
  char *arrc = (char *)"\\F";
  EXPECT_EQ(esc(&arrc), '\f');

  char *arrcc = (char *)"\f";
  EXPECT_EQ(esc(&arrcc), '\f');
}
TEST(esc, controlCode) {
  char *arr = (char *)"\\^C";
  EXPECT_EQ(esc(&arr), 3);

  char *arr2 = (char *)"\\^E";
  EXPECT_EQ(esc(&arr2), 5);
}
TEST(esc, hexNumber) {
  char *arr = (char *)"\\xa7a";
  int c = 10 * 16 * 16 + 7 * 16 + 10;
  EXPECT_EQ(esc(&arr), c);
  // std::cout << "arr" << *arr << std::endl;
}

TEST(bin_to_ascii, all) {
  char *arr = bin_to_ascii(69, 1);
  // std::cout << arr << std::endl;
  EXPECT_NE(std::strcmp(arr, "B"), 0);

  char *arr2 = bin_to_ascii(10, 1);
  EXPECT_EQ(std::strcmp(arr2, "\\n"), 0);

  char *arr3 = bin_to_ascii(39, 1);
  EXPECT_EQ(std::strcmp(arr3, "\\'"), 0);

  // default in switch
  char *arr4 = bin_to_ascii(150, 1);
  // std::cout << arr4 << std::endl;
  EXPECT_EQ(std::strcmp(arr4, "\\x096"), 0);
}
int cmp(void *s1, void *s2) {
  int a1 = *(int *)s1;
  int a2 = *(int *)s2;
  if (a1 == a2)
    return 0;
  else if (a1 <= a2)
    return -1;
  else
    return 1;
}
int cmp(void **s1, void **s2) {
  int a1 = *(*((int **)s1));
  int a2 = *(*((int **)s2));
  // printf("%d, %d", a1, a2);
  if (a1 == a2)
    return 0;
  else if (a1 < a2)
    return -1;
  else
    return 1;
}
TEST(sort, ssort) {
  int arr[10] = {1, 200, 3, 4, 5, 55, 34, 10};
  ssort((char *)arr, std::size(arr), 4, cmp);
  EXPECT_EQ(arr[0], 0);
  EXPECT_EQ(arr[9], 200);

  int **arr2;
  arr2 = new int *[10];
  for (int i = 0; i < 10; i++) {
    arr2[i] = (int *)malloc(sizeof(int));
    *arr2[i] = 10 - i;
  }

  assort((void **)arr2, 10, 8, cmp);
  EXPECT_EQ(*arr2[0], 1);
  EXPECT_EQ(*arr2[9], 10);

  for (int i = 0; i < 10; i++) {
    free(arr2[i]);
  }
  delete[] arr2;
}

TEST(miscellaneous, copyfile) {
  // EXPECT_EQ(copyfile("noneExit.txt", "src.txt", "w"), -1);
  // EXPECT_EQ(copyfile("dst.txt", "noneExit.txt", "w"), -2);
  // EXPECT_EQ(copyfile("dst.txt", "src.txt", "w"), 0);
  // EXPECT_EQ(copyfile("dst.txt", "src.txt", "a"), 0);

  // EXPECT_EQ(movefile("noneExit.txt", "src.txt", "w"), -1);
  // EXPECT_EQ(movefile("dst.txt", "noneExit.txt", "w"), -2);
  // copyfile("dst.txt", "../compilerTest.cpp", "w");
  // EXPECT_EQ(movefile("dst.txt", "src.txt", "w"), 0);
  int *dst = new int[10];
  for (int i = 0; i < 10; i++) {
    dst[i] = i;
  }
  memiset(dst, 0, 10);
  EXPECT_EQ(dst[0], 0);
  EXPECT_EQ(dst[9], 0);
  delete[] dst;

  char str[100];
  int r = concate(100, str, "as", "as", "ssss", "color", NULL);
  EXPECT_EQ(strcmp(str, "asassssscolor"), 0);
  EXPECT_EQ(r, 87);

  char str2[100];
  int r2 = concate(10, str2, "as", "as", "ssss", "color", NULL);
  EXPECT_EQ(strcmp(str2, "asasssssc"), 0);
  EXPECT_EQ(r2, -1);

  /* subtle differences in return values */
  char str3[100];
  int r3 = concate(8, str3, "as", "as", "ssss", "color", NULL);
  EXPECT_EQ(strcmp(str3, "asassss"), 0);
  EXPECT_EQ(r3, -1);

  const char *filename = "t.js";
  const char *envname = "TESTENV";
  char pathname[129];
  // char *pathname2;
  searchenv(filename, envname, pathname);
  // std::cout << filename << std::endl;
  // std::cout << pathname << std::endl;
  EXPECT_EQ(strcmp(pathname, "/home/sheep/test/t.js"), 0);

  FILE *output = fopen("output.txt", "w");
  if (output == NULL) {
    printf("file doesn't exist\n");
  }
  const char *filename2 = "input.txt";
  driver_1(output, 1, filename2);
}
