#pragma once
#include "Error.h"

namespace Zazy {

namespace Obj {

class Func : public Object {
public:
  Func(token_t type, std::vector<stmt_t> params, stmt_t body):
    type(type), params(params), body(body), Object(0, 2, Obj::TypeT::FUNC) {}

  std::string string() {
    auto result = std::string("<func>((");
    for (int32_t i = 0; i < params.size(); i++) {
      if (i) result += ", ";
      result += std::static_pointer_cast<Decl::Var>(params[i])->type->lexeme;
    }

    return result + ") => " + type->lexeme + ")";
  }

public:
  token_t type;
  std::vector<stmt_t> params;
  stmt_t body;
};

};
};
