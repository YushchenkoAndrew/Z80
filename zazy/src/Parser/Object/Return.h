#pragma once
#include "Ptr.h"

namespace Zazy {

namespace Obj {

class Return : public Object {
public:
  Return(obj_t result):
    result(result), Object(0, 0, Obj::TypeT::RETURN) {}

  std::string string() { return std::string("<return>") + result->string(); }

public:
  obj_t result;
};
};
};
