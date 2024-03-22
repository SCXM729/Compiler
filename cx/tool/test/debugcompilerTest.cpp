#include "../../tool/compiler.h"
#include <cstdio>
#include <iostream>
int main() {
  // char ap[100];
  // concate(9, ap, "color", "blue", "red", "black", "white", NULL);
  // std::cout << ap << std::endl;
  // const char *filename = "t.js";
  // const char *envname = "TESTENV";
  // char pathname[129];
  // char *pathname2;
  // searchenv(filename, envname, pathname);
  FILE *output = fopen("output.txt", "a");
  if (output == NULL) {
    printf("file doesn't exist\n");
  }
  const char *filename = "input.txt";
  driver_1(output, 1, filename);

  // char *arr = (char *)"-0x12";
  // double r = cstol(&arr);
  // std::cout << r << std::endl;

  // char *arr2 = (char *)"-0x12";
  // double r2 = cstoul(&arr2);
  // std::cout << "the r2 value is " << r2 << std::endl;

  // char *arr3 = (char *)"\f";
  // int r3 = esc(&arr3);
  // char *arr4 = (char *)"aals";
  // char *arr5 = (char *)"bb";

  // FILE *f = fopen("test.txt", "w");
  // char **arr6;
  // arr6 = new char *[10];
  // for (int i = 0; i < 10; i++) {
  //   arr6[i] = new char[10];
  //   strcpy(arr6[i], "a");
  // }
  // if (f == NULL) {
  //   std::cout << "exit" << std::endl;
  //   exit(1);
  // }
  // comment(f, arr6);
  // fprintf(f, "this");
  // fputc(42, f);
  // fclose(f);
  // ferr("%s", arr4, arr5);
  // ferr("%s", arr4);
  // ferr(nullptr);
  // std::cout << "ls" << std::endl;
}
