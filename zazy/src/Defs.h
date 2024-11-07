#pragma once
#include "Utils/Utils.h"

namespace Zazy {

#define STRSIZE 256
#define PTRDEPTH 3

class Token;

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
enum TokenT {
  // Operations
  OP_PLUS, OP_MINUS, OP_MORE, OP_MORE_OR_EQUAL, OP_LESS, OP_LESS_OR_EQUAL,
  OP_OR, OP_AND, OP_NOT, OP_INC, OP_DEC, OP_EQUAL, OP_NOT_EQUAL,
  OP_DOT, OP_DIV, OP_MOD,

  // Bit operation in assignment
  BIT_OR, BIT_XOR, BIT_NOT, BIT_LEFT_SHIFT, BIT_RIGHT_SHIFT,

  // Braces
  LEFT_BRACE,  RIGHT_BRACE, LEFT_SQUARE_BRACE, RIGHT_SQUARE_BRACE, 
  LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE,

  // 
  COMMA, STAR, AMPERSAND, SEMICOLON, COLON, QUESTION_MARK,

  // Assigned operarion
  ASSIGN, ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_DIV, ASSIGN_MOD, ASSIGN_MUL,
  ASSIGN_BIT_OR, ASSIGN_BIT_AND, ASSIGN_BIT_XOR,
  
  // Literals
  INVALID, IDENTIFIER, STRING, VOID, CHAR, SHORT, INT,

  // Keywords
  W_AUTO, W_BREAK, W_CASE, W_CHAR, W_CONTINUE, W_DEFAULT, W_DO, W_GOTO,
  W_ELSE, W_ENUM, W_EXTERN, W_FOR, W_IF, W_INT, W_RETURN, W_SIZEOF,
  W_SHORT, W_STATIC, W_STRUCT, W_SWTCH, W_VOID, W_WHILE, W_ARGC,

  // Functions
  // FUNC_ASM,

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

typedef TypeList<
  AnyType<TokenT::W_AUTO,    TokenSTR>, TypeList<
  AnyType<TokenT::W_BREAK,   TokenSTR>, TypeList<
  AnyType<TokenT::W_CASE,    TokenSTR>, TypeList<
  AnyType<TokenT::W_CHAR,    TokenSTR>, TypeList<
  AnyType<TokenT::W_CONTINUE,TokenSTR>, TypeList<
  AnyType<TokenT::W_DEFAULT, TokenSTR>, TypeList<
  AnyType<TokenT::W_DO,      TokenSTR>, TypeList<
  AnyType<TokenT::W_GOTO,    TokenSTR>, TypeList<
  AnyType<TokenT::W_ELSE,    TokenSTR>, TypeList<
  AnyType<TokenT::W_ENUM,    TokenSTR>, TypeList<
  AnyType<TokenT::W_EXTERN,  TokenSTR>, TypeList<
  AnyType<TokenT::W_FOR,     TokenSTR>, TypeList<
  AnyType<TokenT::W_IF,      TokenSTR>, TypeList<
  AnyType<TokenT::W_INT,     TokenSTR>, TypeList<
  AnyType<TokenT::W_RETURN,  TokenSTR>, TypeList<
  AnyType<TokenT::W_SIZEOF,  TokenSTR>, TypeList<
  AnyType<TokenT::W_SHORT,   TokenSTR>, TypeList<
  AnyType<TokenT::W_STATIC,  TokenSTR>, TypeList<
  AnyType<TokenT::W_STRUCT,  TokenSTR>, TypeList<
  AnyType<TokenT::W_SWTCH,   TokenSTR>, TypeList<
  AnyType<TokenT::W_WHILE,   TokenSTR>, TypeList<
  AnyType<TokenT::W_VOID,    TokenSTR>, TypeList<
  AnyType<TokenT::W_ARGC,    TokenSTR>, NullType>>>>>>>>>>>>>>>>>>>>>>>
    KeywordList;


typedef TypeList<
  AnyType<TokenT::INVALID,           TokenSTR>, TypeList<
  AnyType<TokenT::STRING,            TokenSTR>, TypeList<
  AnyType<TokenT::VOID,              TokenSTR>, TypeList<
  AnyType<TokenT::CHAR,              TokenSTR>, TypeList<
  AnyType<TokenT::SHORT,             TokenSTR>, TypeList<
  AnyType<TokenT::INT,               TokenSTR>, TypeList<
  AnyType<TokenT::OP_PLUS,           TokenSTR>, TypeList<
  AnyType<TokenT::OP_MINUS,          TokenSTR>, TypeList<
  AnyType<TokenT::OP_MORE,           TokenSTR>, TypeList<
  AnyType<TokenT::OP_LESS,           TokenSTR>, TypeList<
  AnyType<TokenT::OP_OR,             TokenSTR>, TypeList<
  AnyType<TokenT::OP_AND,            TokenSTR>, TypeList<
  AnyType<TokenT::OP_NOT,            TokenSTR>, TypeList<
  AnyType<TokenT::OP_INC,            TokenSTR>, TypeList<
  AnyType<TokenT::OP_DEC,            TokenSTR>, TypeList<
  AnyType<TokenT::OP_EQUAL,          TokenSTR>, TypeList<
  AnyType<TokenT::OP_DOT,            TokenSTR>, TypeList<
  AnyType<TokenT::OP_DIV,            TokenSTR>, TypeList<
  AnyType<TokenT::OP_MOD,            TokenSTR>, TypeList<
  AnyType<TokenT::BIT_OR,            TokenSTR>, TypeList<
  AnyType<TokenT::BIT_XOR,           TokenSTR>, TypeList<
  AnyType<TokenT::BIT_NOT,           TokenSTR>, TypeList<
  AnyType<TokenT::AMPERSAND,         TokenSTR>, TypeList<
  AnyType<TokenT::COMMA,             TokenSTR>, TypeList<
  AnyType<TokenT::STAR,              TokenSTR>, TypeList<
  AnyType<TokenT::SEMICOLON,         TokenSTR>, TypeList<
  AnyType<TokenT::COLON,             TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN,            TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_PLUS,       TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_MINUS,      TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_DIV,        TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_MOD,        TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_MUL,        TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_BIT_OR,     TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_BIT_AND,    TokenSTR>, TypeList<
  AnyType<TokenT::ASSIGN_BIT_XOR,    TokenSTR>, TypeList<
  AnyType<TokenT::IDENTIFIER,        TokenSTR>, TypeList<
  AnyType<TokenT::OP_NOT_EQUAL,      TokenSTR>, TypeList<
  AnyType<TokenT::OP_MORE_OR_EQUAL,  TokenSTR>, TypeList<
  AnyType<TokenT::OP_LESS_OR_EQUAL,  TokenSTR>, TypeList<
  AnyType<TokenT::BIT_LEFT_SHIFT,    TokenSTR>, TypeList<
  AnyType<TokenT::BIT_RIGHT_SHIFT,   TokenSTR>, TypeList<
  AnyType<TokenT::LEFT_BRACE,        TokenSTR>, TypeList<
  AnyType<TokenT::RIGHT_BRACE,       TokenSTR>, TypeList<
  AnyType<TokenT::LEFT_SQUARE_BRACE, TokenSTR>, TypeList<
  AnyType<TokenT::RIGHT_SQUARE_BRACE,TokenSTR>, TypeList<
  AnyType<TokenT::LEFT_CURLY_BRACE,  TokenSTR>, TypeList<
  AnyType<TokenT::RIGHT_CURLY_BRACE, TokenSTR>, TypeList<
  AnyType<TokenT::QUESTION_MARK,     TokenSTR>, KeywordList>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    TokenList;


class Defs {

public:
  static void Init() {
    AnyType<TokenT::CMD_INCLUDE, TokenSTR>::GetValue() = "#include";
    AnyType<TokenT::CMD_DEFINE,  TokenSTR>::GetValue() = "#define";
    AnyType<TokenT::CMD_IFDEF,   TokenSTR>::GetValue() = "#ifdef";
    AnyType<TokenT::CMD_IFNDEF,  TokenSTR>::GetValue() = "#ifndef";
    AnyType<TokenT::CMD_ELSE,    TokenSTR>::GetValue() = "#else";
    AnyType<TokenT::CMD_ENDIF,   TokenSTR>::GetValue() = "#endif";


    // AnyType<TokenT::FUNC_ASM,  TokenSTR>::GetValue() = "asm";
    AnyType<TokenT::W_AUTO,    TokenSTR>::GetValue() = "auto";
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
    AnyType<TokenT::W_SHORT,   TokenSTR>::GetValue() = "short";
    AnyType<TokenT::W_STATIC,  TokenSTR>::GetValue() = "static";
    AnyType<TokenT::W_STRUCT,  TokenSTR>::GetValue() = "struct";
    AnyType<TokenT::W_SWTCH,   TokenSTR>::GetValue() = "switch";
    AnyType<TokenT::W_VOID,    TokenSTR>::GetValue() = "void";
    AnyType<TokenT::W_WHILE,   TokenSTR>::GetValue() = "while";
    AnyType<TokenT::W_ARGC,    TokenSTR>::GetValue() = "__argc";


    AnyType<TokenT::INVALID,   TokenSTR>::GetValue() = "INVALID";
    AnyType<TokenT::OP_PLUS,   TokenSTR>::GetValue() = "PLUS";
    AnyType<TokenT::OP_MINUS,  TokenSTR>::GetValue() = "MINUS";
    AnyType<TokenT::OP_MORE,   TokenSTR>::GetValue() = "MORE";
    AnyType<TokenT::OP_LESS,   TokenSTR>::GetValue() = "LESS";
    AnyType<TokenT::OP_OR,     TokenSTR>::GetValue() = "LOGOR";
    AnyType<TokenT::OP_AND,    TokenSTR>::GetValue() = "LOGAND";
    AnyType<TokenT::OP_NOT,    TokenSTR>::GetValue() = "XMARK";
    AnyType<TokenT::OP_INC,    TokenSTR>::GetValue() = "INC";
    AnyType<TokenT::OP_DEC,    TokenSTR>::GetValue() = "DEC";
    AnyType<TokenT::OP_EQUAL,  TokenSTR>::GetValue() = "EQUAL";
    AnyType<TokenT::OP_DOT,    TokenSTR>::GetValue() = "DOT";
    AnyType<TokenT::OP_DIV,    TokenSTR>::GetValue() = "DIV";
    AnyType<TokenT::OP_MOD,    TokenSTR>::GetValue() = "MOD";
    AnyType<TokenT::BIT_OR,    TokenSTR>::GetValue() = "PIPE";
    AnyType<TokenT::BIT_XOR,   TokenSTR>::GetValue() = "CARET";
    AnyType<TokenT::BIT_NOT,   TokenSTR>::GetValue() = "TILDE";
    AnyType<TokenT::AMPERSAND, TokenSTR>::GetValue() = "AMPER";
    AnyType<TokenT::COMMA,     TokenSTR>::GetValue() = "COMMA";
    AnyType<TokenT::STAR,      TokenSTR>::GetValue() = "STAR";
    AnyType<TokenT::SEMICOLON, TokenSTR>::GetValue() = "SEMI";
    AnyType<TokenT::COLON,     TokenSTR>::GetValue() = "COLON";
    AnyType<TokenT::IDENTIFIER,TokenSTR>::GetValue() = "IDENT";
    AnyType<TokenT::STRING,    TokenSTR>::GetValue() = "STRING";
    AnyType<TokenT::VOID,      TokenSTR>::GetValue() = "VOID";
    AnyType<TokenT::CHAR,      TokenSTR>::GetValue() = "CHAR";
    AnyType<TokenT::SHORT,     TokenSTR>::GetValue() = "SHORT";
    AnyType<TokenT::INT,       TokenSTR>::GetValue() = "INT";

    AnyType<TokenT::ASSIGN,        TokenSTR>::GetValue() = "ASSIGN";
    AnyType<TokenT::ASSIGN_PLUS,   TokenSTR>::GetValue() = "ASPLUS";
    AnyType<TokenT::ASSIGN_MINUS,  TokenSTR>::GetValue() = "ASMINUS";
    AnyType<TokenT::ASSIGN_DIV,    TokenSTR>::GetValue() = "ASDIV";
    AnyType<TokenT::ASSIGN_MOD,    TokenSTR>::GetValue() = "ASMOD";
    AnyType<TokenT::ASSIGN_MUL,    TokenSTR>::GetValue() = "ASMUL";
    AnyType<TokenT::ASSIGN_BIT_OR, TokenSTR>::GetValue() = "ASPIPE";
    AnyType<TokenT::ASSIGN_BIT_AND,TokenSTR>::GetValue() = "ASAMPER";
    AnyType<TokenT::ASSIGN_BIT_XOR,TokenSTR>::GetValue() = "ASCARET";


    AnyType<TokenT::OP_NOT_EQUAL,      TokenSTR>::GetValue() = "NOTEQ";
    AnyType<TokenT::OP_MORE_OR_EQUAL,  TokenSTR>::GetValue() = "GTEQ";
    AnyType<TokenT::OP_LESS_OR_EQUAL,  TokenSTR>::GetValue() = "LTEQ";
    AnyType<TokenT::BIT_LEFT_SHIFT,    TokenSTR>::GetValue() = "LSHIFT";
    AnyType<TokenT::BIT_RIGHT_SHIFT,   TokenSTR>::GetValue() = "RSHIFT";
    AnyType<TokenT::LEFT_BRACE,        TokenSTR>::GetValue() = "LPAREN";
    AnyType<TokenT::RIGHT_BRACE,       TokenSTR>::GetValue() = "RPAREN";
    AnyType<TokenT::LEFT_SQUARE_BRACE, TokenSTR>::GetValue() = "LBRACK";
    AnyType<TokenT::RIGHT_SQUARE_BRACE,TokenSTR>::GetValue() = "RBRACK";
    AnyType<TokenT::LEFT_CURLY_BRACE,  TokenSTR>::GetValue() = "LBRACE";
    AnyType<TokenT::RIGHT_CURLY_BRACE, TokenSTR>::GetValue() = "RBRACE";
    AnyType<TokenT::QUESTION_MARK,     TokenSTR>::GetValue() = "QMARK";
  }
};
}