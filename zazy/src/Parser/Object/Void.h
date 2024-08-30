#pragma once
#include "Short.h"

namespace Zazy {

namespace Obj {

class Void : public Object {
public:
  Void():
    Object(0, 0) {}

  Obj::TypeT type() override { return Obj::TypeT::VOID; }
  std::string string() { return std::to_string(value); }

};

};
};
