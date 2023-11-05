#include "../tool/set.h"
#include <bitset>
#include <format>
#include <iostream>
#include <signal.h>
int main() {

  SET *mySet2 = newset();
  SET *mySet1 = dupset(mySet2);
  std::cout << std::format("mySet2->nwords:{},mySet2->nbits:{}\n",
                           mySet2->nwords, mySet2->nbits);
  std::cout << std::format("mySet1->nwords:{},mySet1->nbits:{}\n",
                           mySet1->nwords, mySet1->nbits);

  mySet2->map[0] = 1;
  mySet2->map[1] = 2;
  mySet2->map[2] = 3;
  mySet1->map[0] = 4;
  mySet1->map[1] = 3;

  pset(mySet1, fprintf, stdout);
  std::cout << std::endl;
  pset(mySet2, fprintf, stdout);
  std::cout << std::endl;

  // std::cout << std::bitset<sizeof(unsigned short) * 8>(mySet2->map[0])
            // << std::endl;
  int op = _ASSIGN;
  switch (op) {
  case _UNION:
    _set_op(_UNION, mySet1, mySet2);
    pset(mySet1, fprintf, stdout);
    std::cout << std::endl;
    pset(mySet2, fprintf, stdout);
    std::cout << std::endl;
    break;
  case _INTERSECT:
    _set_op(_INTERSECT, mySet1, mySet2);
    pset(mySet1, fprintf, stdout);
    std::cout << std::endl;
    pset(mySet2, fprintf, stdout);
    std::cout << std::endl;
    break;
  case _DIFFERENCE:
    _set_op(_DIFFERENCE, mySet1, mySet2);
    pset(mySet1, fprintf, stdout);
    std::cout << std::endl;
    pset(mySet2, fprintf, stdout);
    std::cout << std::endl;
    break;
  case _ASSIGN:
    _set_op(_ASSIGN, mySet1, mySet2);
    pset(mySet1, fprintf, stdout);
    std::cout << std::endl;
    pset(mySet2, fprintf, stdout);
    std::cout << std::endl;
    break;
  }
  std::cout<<"_set_op end"<<std::endl;

  _addset(mySet1,129);
  _addset(mySet1,139);
  pset(mySet1,fprintf,stdout);
  pset(mySet2,fprintf,stdout);
  std::cout<<std::format("{},{}\n",mySet1->nbits,_ROUND(129))<<std::endl;
  std::cout<<std::format("{},{}\n",num_ele(mySet1),num_ele(mySet2));
  std::cout<<std::format("{}\n",_set_test(mySet1,mySet2));
  // std::cout << std::bitset<sizeof(unsigned short) * 8>(mySet2->map[0])
  //           << std::endl;
  // std::cout << std::bitset<sizeof(unsigned short) * 8>(mySet1->map[0])
  //           << std::endl;

  delset(mySet2);
  delset(mySet1);

  std::cout << "\ntest end" << std::endl;

  std::cout << _BYTES_IN_ARRAY(13) << std::endl;
  int a = 15;
  SET mySet{.map = (_SETTYPE *)malloc(sizeof(_SETTYPE) * 3)};
  mySet.map[0] = 12;
  mySet.map[1] = 15;
  mySet.map[2] = 24;
  mySet.nbits = 19;
  unsigned xx = 48;
  std::cout << std::bitset<sizeof(unsigned short) * 8>(mySet.map[1])
            << std::endl;
  // std::cout<<sizeof(unsigned)<<std::endl;

  std::cout << _MOD_WSIZE(18) << std::endl;
  std::cout << std::format("MEMBER position 18:{}\n", MEMBER(&mySet, 19));
  if (TEST(&mySet, 10))

    std::cout << std::bitset<sizeof(unsigned short) * 8>(mySet.map[1])
              << std::endl;
  // if(0<1){
  // raise(SIGABRT);
  // }
  std::cout << std::format("sizeof(unsigend char):{}", sizeof(unsigned char))
            << std::endl;
  unsigned short map[]{1, 2, 2, 2, 3, 3, 3};
  unsigned char *p = (unsigned char *)map;
  int aa[] = {1, 2, 3, 4, 5, 6, 7};
  std::cout << std::format("address:map:{},map+1{}\n", static_cast<void *>(map),
                           static_cast<void *>(map + 1));
  std::cout << std::format("address:p:{},p+1:{}\n",
                           static_cast<const void *>(p),
                           static_cast<void *>(p + 1));
  std::cout << *map << int(*++(++(++p))) << std::endl;
  std::cout << aa[*p] << std::endl;
  char *ssd{"ll"};
  int x = reinterpret_cast<long>(ssd);
  std::cout << "program ends" << std::endl;
}
