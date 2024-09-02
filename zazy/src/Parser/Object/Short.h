#pragma once
#include "Return.h"

namespace Zazy {

namespace Obj {

class Short : public Object {
public:
  Short(uint16_t value):
    Object(value, 2, Obj::TypeT::SHORT) {}

  std::string string() { return std::to_string(value); }

};

};
};
