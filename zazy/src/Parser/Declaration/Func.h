#pragma once
#include "Enum.h"

namespace Zazy {

namespace Decl {

class Func : public Statement {
public:
  Func(token_t type, token_t name, std::vector<stmt_t> params, stmt_t body):
    type(type), name(name), params(params), body(body) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitDeclFunc(this);
  }

  void print() { 
    printf("%s %s(", type->lexeme.c_str(), name->lexeme.c_str());
    for (int32_t i = 0; i < params.size(); i++) { if (i) printf(", "); params[i]->print(); }
    printf(") "); body->print();
  }

public:
  token_t type;
  token_t name;
  std::vector<stmt_t> params;
  stmt_t body;
};

};
};

