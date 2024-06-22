#include "Temp.h"

// #define DEBUG

int main() {

  asm("ld A, 0x01");
  #ifndef abc
  asm("ld A, 0x02");
  #else 
  asm("ld A, 0x22");
  #endif
  asm("ld A, 0x03");


  return 0;
}
