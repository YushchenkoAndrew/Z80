#pragma once
#include "Ptr.h"

namespace Zazy {

namespace Obj {

class Short : public Object {
public:
  Short(uint16_t value):
    Object(value, 2) {}

  Obj::TypeT type() override { return Obj::TypeT::SHORT; }
  std::string string() { return std::to_string(value); }

};

};
};
