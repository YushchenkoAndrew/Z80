#pragma once
#include "Error.h"

namespace Zazy {

namespace Obj {

class Int : public Object {
public:
  Int(uint32_t value):
    Object(value, 4, Obj::TypeT::INT) {}

  std::string string() { return std::to_string(value); }

};

};
};
