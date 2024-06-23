#include "Temp.h"

short t = 0x12;

struct TT {
  int o;
  int o2;
};

// #define DEBUG

int main() {
  struct TT tt;

  int* t5;

  int ttt = *t5;
  *((int*)(&tt + 4)) = &tt + 2;

  // if (5 < 4 < 3);

  

  asm("ld A, 0x01");
  #ifndef abc
  asm("ld A, 0x02");
  #else 
  asm("ld A, 0x22");
  #endif
  asm("ld A, 0x03");


  return 0;
}
