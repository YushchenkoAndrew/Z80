#pragma once
#include "src/Parser/Declaration/Var.h"

namespace Zazy {

namespace Obj {

class Char : public Object {
public:
  Char(uint8_t value):
    Object(value, 1, Obj::TypeT::CHAR) {}

  std::string string() { return std::to_string(value); }
};

};
};
