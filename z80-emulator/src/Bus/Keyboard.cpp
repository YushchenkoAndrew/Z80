#include "Bus.h"

namespace Bus {
  void Keyboard::Interrupt() { bus->Interrupt(); }
};