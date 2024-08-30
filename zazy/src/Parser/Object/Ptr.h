#pragma once
#include "Int.h"

namespace Zazy {

namespace Obj {

class Ptr : public Object {
public:
  Ptr(uint16_t value):
    Object(value, 2) {}

  Obj::TypeT type() override { return Obj::TypeT::PTR; }
  std::string string() { return std::to_string(value); }

};

};
};
