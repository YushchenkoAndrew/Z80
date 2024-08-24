#pragma once
#include "Ternary.h"

namespace Zcc {
namespace Expr {

class Type : public Expression {
public:
  Type(std::shared_ptr<Token> type):
    type(type) {}
  
  
  void* accept(Visitor* visitor) override {
    return visitor->visitExprType(this);
  }

public:
  std::shared_ptr<Token> type;
  // std::shared_ptr<Token> operation;
};
};
};

