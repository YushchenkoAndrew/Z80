#pragma once
#include "Char.h"

namespace Zazy {

namespace Obj {

class Error : public Object {
public:
  Error(std::string msg):
    msg(msg), Object(0, 0, Obj::TypeT::ERROR) {}

  std::string string() { return msg; }

private:
  std::string msg;
};

};
};
