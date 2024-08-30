#pragma once
#include <iostream>
// #include "src/Lexer/Lexer.h"
#include "src/Parser/Visitor/Evaluate.h"

namespace Zazy {
namespace Repl {

  enum CommandT {
    none    = '\0',
    help    = 'h',
    lexer   = 'l',
    parser  = 'p',
  };

  uint8_t cmd(std::string& line) {
    if (line.size() < 2) return '\0';

    char op = line[1];
    line = line.substr(2, line.size() - 2);

    if (op != CommandT::help) return op;
    std::cout << "\\l    Display code representation in token table\n"
              << "\\p    Display reverse code representation from ast\n"
              << "\\h    Displays help options\n"
              << "\n";

    return '\0';
  }

  void exec() {
    std::cout << "Welcome to zazy C-like repl [debug]\n"
              << "Type \\h for help\n"
              << "> ";

    for (std::string line; std::getline(std::cin, line); std::cout << "> ") {
      uint8_t op = line.size() > 0 && line[0] == '\\' ? cmd(line) : CommandT::none;

      // TODO: add op handler

      Lexer l(line);
      Parser p(l);

      Evaluate eval(p);


      // p.declaration()->print();
      printf("%s", eval.next()->string().c_str());
      printf("\n");

      // Token::header();

      // for (token_t t = l.next(); t->token != TokenT::CMD_EOF; t = l.next()) {
      //   t->print();
      // }
    }
  }

};
};
