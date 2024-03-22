#include <cstdio>
#ifdef DEBUG
int Lev = 0;
#define ENTER(f) printf("%*senter %s [%c][%1.10s]\n",
Lev++ * 4,"",f,Lexeme,Input)
#define LEAVE(f) printf("%*sleave %s [%c][%1.10s]\n",
Lev-- * 4,"",f,)
#else
#define ENTER(f)
#define ENTER(f)
#endif
void funct(){
  ENTER
}
int main(){

}
