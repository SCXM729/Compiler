#include"set.h"
#include<bitset>
#include<iostream>
#include<format>
int main(){
    std::cout<<_BYTES_IN_ARRAY(13)<<std::endl;
    int a=15;
    SET mySet{.map=(_SETTYPE*)malloc(sizeof(_SETTYPE)*3)};
    mySet.map[0]=12;
    mySet.map[1]=15;
    mySet.map[2]=24;
    mySet.nbits=19;
    unsigned xx=48;
    std::cout<< std::bitset<sizeof(unsigned short)*8>(mySet.map[1])<<std::endl;
    // std::cout<<sizeof(unsigned)<<std::endl;

    std::cout<<_MOD_WSIZE(18)<<std::endl;
    std::cout<<std::format("MEMBER position 18:{}\n",MEMBER(&mySet,19) );
    
    std::cout<< std::bitset<sizeof(unsigned short)*8>(mySet.map[1])<<std::endl;
}
