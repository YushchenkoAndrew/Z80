#pragma once
#include "Short.h"

namespace Zazy {

namespace Obj {

class Void : public Object {
public:
  Void():
    Object(0, 0) {}

  std::string string() { return "<void>"; }

};

};
};
