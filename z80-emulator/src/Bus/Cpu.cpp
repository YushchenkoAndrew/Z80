#include "Bus.h"

namespace Bus {
namespace Z80 {

  inline uint8_t CPU::Read() { return bus->Read(regPC()++); }

   std::string CPU::Disassemble(){
    std::string assembler = "";
  
    for (uint32_t i = 0; i < 0xFFFF;) {
      AnyType<-1, int32_t>::GetValue() = bus->Read(i++);
      auto cmd = foreach<Instructions, CPU>::Key2Value();

      for (int32_t j = 1; j < cmd.second; j++, i++) {
        // TODO: Use std::snprintf
      }
    }

    return assembler;
  }

};

};
