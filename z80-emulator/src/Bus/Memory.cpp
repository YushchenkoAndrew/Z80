#include "Bus.h"

namespace Bus {

#define TEMPLATE template <int32_t TypeT, int32_t SizeT>
#define CLASS Memory<TypeT, SizeT>

  TEMPLATE void CLASS::Disassemble() {
    dasm = bus->Disassemble();
    lexer.scan(dasm.first); 
  }


template class Memory<MemoryT::W27C512, W27C512_SIZE>;
template class Memory<MemoryT::IMS1423, IMS1423_SIZE>;

#undef TEMPLATE
#undef CLASS
};