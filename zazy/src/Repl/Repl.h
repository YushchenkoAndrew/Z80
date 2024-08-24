#pragma once
#include <iostream>
#include "src/Lexer/Lexer.h"

namespace Zcc {
  void repl() {
    std::cout << "Welcome to zazy C-like repl [debug]\n> ";

    for (std::string line; std::getline(std::cin, line); std::cout << "> ") {
      Lexer l(line);

      Token::header();
      for (token_t t = l.next(); t->token != TokenT::CMD_EOF; t = l.next()) {
        t->print();
      }
    }
  }
};
