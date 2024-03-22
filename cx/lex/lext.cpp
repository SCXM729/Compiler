#include "../tool/debug.h"
#include "dfa.h"
#include "globals.h"
#include "nfa.h"
#include <iostream>
#include <typeinfo>
typedef int row[128];
using row2 = int[128];
int main() {
  int a[10];
  int *p = &a[9];
  p++;
  std::cout << INBOUNDS(a, p);

  row *dtran;
  row2 *dtran2;
  row *dtran3;
  int i[128];
  int i2;
  std::cout << typeid(i2).name() << std::endl;
  std::cout << typeid(i).name() << std::endl;
  std::cout << typeid(dtran3).name() << std::endl;
  std::cout << typeid(dtran).name() << std::endl;
  std::cout << typeid(dtran2).name() << std::endl;
  if (typeid(dtran) != typeid(dtran2)) {
    printf("types are different");
  }
}
