#include "Bus.h"

namespace Bus {
  inline void Keyboard::Interrupt() { bus->Interrupt(); }
};