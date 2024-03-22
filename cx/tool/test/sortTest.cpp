#include "../../tool/compiler.h"
#include <cstdio>
#include <cstdlib>
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
void func(void **base, int nel, int elsize, int (*cmp)(void *, void *)) {}
int d(void *a, void *b) {}

int main() {
  // char arr[10] = {1, 20, 3, 67, 89, 10, 23, 14, 12, 10};
  // ssort(arr, sizeof(arr) / sizeof(arr[0]), sizeof(arr[0]), cmp);
  // for (auto i : arr) {
  //   printf("%d\t", i);
  // }

  int *aa[10];
  for (int i = 0; i <10;i++) {
    aa[i] = (int *)malloc(sizeof(int));
    *aa[i] = 10-i;
  }
  *aa[5]=7;
  for(int i=10;--i>=0;){
    printf("%d",*aa[i]);
  }
  printf("\n");

  assort((void **)aa, 10, 4, cmp);

  for(int i=0;i<10;i++){
    printf("%d\t",*aa[i]);
  }

}
