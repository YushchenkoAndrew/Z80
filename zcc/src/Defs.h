#pragma once
#include "Utils/Utils.h"

namespace Zcc {

class Token;
// typedef std::pair<std::string, std::shared_ptr<Token>> RealtiveToken;

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
enum TokenT {
  // Operations
  NONE, OP_PLUS, OP_MINUS, OP_MORE, OP_MORE_OR_EQUAL, OP_LESS, OP_LESS_OR_EQUAL,
  OP_OR, OP_AND, OP_NOT, OP_INC, OP_DEC, OP_EQUAL, OP_NOT_EQUAL,
  OP_DOT, OP_ARROW, OP_DIV, OP_MOD,

  // Bit operation in assignment
  BIT_OR, BIT_XOR, BIT_NOT, BIT_LEFT_SHIFT, BIT_RIGHT_SHIFT,

  LEFT_BRACE,  RIGHT_BRACE, COMMA, STAR, AMPERSAND, SEMICOLON, COLON,
  LEFT_SQUARE_BRACE, RIGHT_SQUARE_BRACE, QUESTION_MARK,
  LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE, ELLIPSIS,

  ASSIGN, ASSIGN_MOD, ASSIGN_DIV, ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_MUL,
  ASSIGN_BIT_OR, ASSIGN_BIT_AND, ASSIGN_BIT_XOR,
  
  // Literals
  IDENTIFIER, STRING, NUMBER, 

  // Keywords
  W_BREAK, W_CASE, W_CHAR, W_CONTINUE, W_DEFAULT, W_DO, W_GOTO,
  W_ELSE, W_ENUM, W_EXTERN, W_FOR, W_IF, W_INT, W_RETURN, W_SIZEOF,
  W_STATIC, W_SWTCH, W_VOID, W_WHILE, W_ARGC,

  // Commands
  CMD_INCLUDE, CMD_DEFINE, CMD_IFDEF, CMD_IFNDEF, CMD_ELSE, CMD_ENDIF, CMD_UNDEF,
  CMD_EOF
};

typedef TypeList<TokenT, std::string> TokenSTR;


typedef TypeList<
  AnyType<TokenT::CMD_INCLUDE,TokenSTR>, TypeList<
  AnyType<TokenT::CMD_DEFINE, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_IFDEF,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_IFNDEF, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_ELSE,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_ENDIF,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_UNDEF,  TokenSTR>, NullType>>>>>>>
    CommandList;


class Defs {

public:
  static void Init() {
    AnyType<TokenT::CMD_INCLUDE, TokenSTR>::GetValue() = "#include";
    AnyType<TokenT::CMD_DEFINE,  TokenSTR>::GetValue() = "#define";
    AnyType<TokenT::CMD_IFDEF,   TokenSTR>::GetValue() = "#ifdef";
    AnyType<TokenT::CMD_IFNDEF,  TokenSTR>::GetValue() = "#ifndef";
    AnyType<TokenT::CMD_ELSE,    TokenSTR>::GetValue() = "#else";
    AnyType<TokenT::CMD_ENDIF,   TokenSTR>::GetValue() = "#endif";


    AnyType<TokenT::W_BREAK,   TokenSTR>::GetValue() = "break";
    AnyType<TokenT::W_CASE,    TokenSTR>::GetValue() = "case";
    AnyType<TokenT::W_CHAR,    TokenSTR>::GetValue() = "char";
    AnyType<TokenT::W_CONTINUE,TokenSTR>::GetValue() = "continue";
    AnyType<TokenT::W_DEFAULT, TokenSTR>::GetValue() = "default";
    AnyType<TokenT::W_DO,      TokenSTR>::GetValue() = "do";
    AnyType<TokenT::W_GOTO,    TokenSTR>::GetValue() = "goto";
    AnyType<TokenT::W_ELSE,    TokenSTR>::GetValue() = "else";
    AnyType<TokenT::W_ENUM,    TokenSTR>::GetValue() = "enum";
    AnyType<TokenT::W_EXTERN,  TokenSTR>::GetValue() = "extern";
    AnyType<TokenT::W_FOR,     TokenSTR>::GetValue() = "for";
    AnyType<TokenT::W_IF,      TokenSTR>::GetValue() = "if";
    AnyType<TokenT::W_INT,     TokenSTR>::GetValue() = "int";
    AnyType<TokenT::W_RETURN,  TokenSTR>::GetValue() = "return";
    AnyType<TokenT::W_SIZEOF,  TokenSTR>::GetValue() = "sizeof";
    AnyType<TokenT::W_STATIC,  TokenSTR>::GetValue() = "static";
    AnyType<TokenT::W_SWTCH,   TokenSTR>::GetValue() = "switch";
    AnyType<TokenT::W_VOID,    TokenSTR>::GetValue() = "void";
    AnyType<TokenT::W_WHILE,   TokenSTR>::GetValue() = "while";
    AnyType<TokenT::W_ARGC,    TokenSTR>::GetValue() = "__argc";
  }
};
}