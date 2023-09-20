#include "Bus.h"

namespace Bus {

#define TEMPLATE template <int32_t TypeT, int32_t SizeT>
#define CLASS Memory<TypeT, SizeT>

  TEMPLATE void CLASS::Disassemble() {
    dasm = bus->Disassemble();
    lexer.scan(dasm.first); 
  }


template class Memory<MemoryT::EEPROM, 65536>;

#undef TEMPLATE
#undef CLASS
};