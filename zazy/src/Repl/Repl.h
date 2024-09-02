#pragma once
#include <iostream>
// #include "src/Lexer/Lexer.h"
#include "src/Ast/Evaluate.h"

namespace Zazy {
// namespace REPL {

  // uint8_t cmd(std::string& line) {
  //   if (line.size() < 2) return '\0';

  //   char op = line[1];
  //   line = line.substr(2, line.size() - 2);

  //   if (op != CommandT::help) return op;

  //   return '\0';
  // }


class Repl {
private:
  enum CommandT {
    OP_EVAL, OP_HELP, OP_LEXER, OP_PARSER
  };

  typedef TypeList<
    TypeList<Int2Type<OP_EVAL>,   Int2Type<'\0'>>, TypeList<
    TypeList<Int2Type<OP_HELP>,   Int2Type<'h'>>, TypeList<
    TypeList<Int2Type<OP_LEXER>,  Int2Type<'l'>>, TypeList<
    TypeList<Int2Type<OP_PARSER>, Int2Type<'p'>>, NullType>>>>
      CommandList;

  inline static void hello() {
    std::cout << "Welcome to zazy C-like repl [debug]\n"
              << "Type \\h for help\n"
              << "> ";
  }

public:
  Repl():
    env(std::make_unique<Environment>(nullptr)) {}

  void exec() {
    for (Repl::hello(); std::getline(std::cin, line); std::cout << "> ") {
      AnyType<-1, int32_t>::GetValue() = line.size() > 1 && line[0] == '\\' ? line[1] : '\0';
      foreach<CommandList, Repl>::Command(this);
      printf("\n");
    }
  }


  template<int32_t T>
  inline void Command(Int2Type<T>) { printf("Unknown command."); }

  inline void Command(Int2Type<OP_EVAL>) {
    Evaluate eval = Evaluate(Parser(Lexer(line)), std::move(env));
    do {
      auto obj = eval.next();
      if (obj == nullptr) std::cout << "Something went completely wrong.";
      else printf("%s", obj->string().c_str());
    } while(!eval.isAtEnd());

    env = eval.move();
  }

  inline void Command(Int2Type<CommandT::OP_HELP>) {
    std::cout << "\\l    Display code representation in token table\n"
              << "\\p    Display reverse code representation from ast\n"
              << "\\h    Displays help options\n"
              << "\n";
  }

  inline void Command(Int2Type<CommandT::OP_LEXER>) {
    Token::header();

    Lexer l(line.substr(2, line.size() - 2));
    for (token_t t = l.next(); t->token != TokenT::CMD_EOF; t = l.next()) {
      t->print();
    }
  }

  inline void Command(Int2Type<CommandT::OP_PARSER>) {
    Parser p(Lexer(line.substr(2, line.size() - 2)));
    auto ast = p.next();

    if (p.errors.size()) {
      for (auto& err : p.errors) std::cout << err;
      return;
    } 

    if (ast == nullptr) std::cout << "Something went completely wrong.";
    else ast->print();
  }

private:
  env_t env;
  std::string line;
};


};
