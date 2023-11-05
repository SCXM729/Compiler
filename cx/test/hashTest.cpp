#include "../tool/hash.h"
#include <format>
#include <iostream>
struct myStruct {
  int a2;
  // int a3;
  int *a[1];
};

int main() {
  myStruct *m1 = (myStruct *)calloc(1,sizeof(myStruct) + 4 * 4);
  // m1->a[0][0]=1;
  std::cout<<std::addressof(m1->a)<<std::endl;
  std::cout<<std::addressof(m1->a[0])<<std::endl;
  int **p = &(m1->a)[1];

  int **p1 = &m1->a[0];
  if (p1 == p) {
    exit(0);
  }
  free(m1);
  std::cout << std::format("sizeof(BUCKET):{},sizeof(HASH_TAB):{}",
                           sizeof(BUCKET), sizeof(HASH_TAB))
            << std::endl;
  std::cout << std::format("sizeof(myStruct):{}\n", sizeof(myStruct));
  BUCKET *sym;
  int size = 0;
  sym = (BUCKET *)calloc(size + sizeof(BUCKET), 1);
  std::cout << std::addressof(sym);
  free(sym);


}
