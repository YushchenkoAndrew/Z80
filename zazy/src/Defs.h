#pragma once
#include "Utils/Utils.h"

namespace Zazy {

/**
 * Token kinds.
 *
 * NOTE: the keyword / token-name tables used to be nested TypeLists searched with
 * foreach<>::Value2Key() (case-insensitive!). They are plain maps now, so `If` is no longer
 * a keyword and adding a token is a one-line change. include/Typelist.h and Foreach.h are
 * still used by the REPL command dispatch.
 */
enum TokenT {
  // Operations
  OP_PLUS, OP_MINUS, OP_MORE, OP_MORE_OR_EQUAL, OP_LESS, OP_LESS_OR_EQUAL,
  OP_OR, OP_AND, OP_NOT, OP_INC, OP_DEC, OP_EQUAL, OP_NOT_EQUAL,
  OP_DOT, OP_ARROW, OP_DIV, OP_MOD,

  // Bit operation in assignment
  BIT_OR, BIT_XOR, BIT_NOT, BIT_LEFT_SHIFT, BIT_RIGHT_SHIFT,

  // Braces
  LEFT_BRACE,  RIGHT_BRACE, LEFT_SQUARE_BRACE, RIGHT_SQUARE_BRACE,
  LEFT_CURLY_BRACE, RIGHT_CURLY_BRACE,

  //
  COMMA, STAR, AMPERSAND, SEMICOLON, COLON, QUESTION_MARK,

  // Assigned operation
  ASSIGN, ASSIGN_PLUS, ASSIGN_MINUS, ASSIGN_DIV, ASSIGN_MOD, ASSIGN_MUL,
  ASSIGN_BIT_OR, ASSIGN_BIT_AND, ASSIGN_BIT_XOR, ASSIGN_SHL, ASSIGN_SHR,

  // Literals
  INVALID, IDENTIFIER, STRING, NUMBER, CHAR, ASM_BLOCK,

  // Keywords
  W_AUTO, W_BREAK, W_CASE, W_CONST, W_CONTINUE, W_DEFAULT, W_DO, W_GOTO,
  W_ELSE, W_ENUM, W_EXTERN, W_FOR, W_IF, W_RETURN, W_SIZEOF,
  W_STATIC, W_STRUCT, W_SWITCH, W_TYPEDEF, W_VOID, W_WHILE, W_ASM, W_NAKED, W_INTERRUPT,
  W_UINT8, W_INT8, W_UINT16, W_INT16,
  W_CHAR, W_SHORT, W_INT,           // aliases: char = uint8_t, short = int = int16_t

  T_EOF
};

inline const std::unordered_map<std::string, TokenT>& Keywords() {
  static const std::unordered_map<std::string, TokenT> k = {
    { "auto", W_AUTO }, { "break", W_BREAK }, { "case", W_CASE }, { "const", W_CONST },
    { "continue", W_CONTINUE }, { "default", W_DEFAULT }, { "do", W_DO }, { "goto", W_GOTO },
    { "else", W_ELSE }, { "enum", W_ENUM }, { "extern", W_EXTERN }, { "for", W_FOR },
    { "if", W_IF }, { "return", W_RETURN }, { "sizeof", W_SIZEOF }, { "static", W_STATIC },
    { "struct", W_STRUCT }, { "switch", W_SWITCH }, { "typedef", W_TYPEDEF }, { "void", W_VOID },
    { "while", W_WHILE }, { "asm", W_ASM }, { "naked", W_NAKED }, { "interrupt", W_INTERRUPT },
    { "uint8_t", W_UINT8 }, { "int8_t", W_INT8 }, { "uint16_t", W_UINT16 }, { "int16_t", W_INT16 },
    { "char", W_CHAR }, { "short", W_SHORT }, { "int", W_INT },
  };
  return k;
}

inline const char* TokenName(TokenT t) {
  switch (t) {
    case OP_PLUS: return "PLUS";        case OP_MINUS: return "MINUS";
    case OP_MORE: return "MORE";        case OP_MORE_OR_EQUAL: return "GTEQ";
    case OP_LESS: return "LESS";        case OP_LESS_OR_EQUAL: return "LTEQ";
    case OP_OR: return "LOGOR";         case OP_AND: return "LOGAND";
    case OP_NOT: return "XMARK";        case OP_INC: return "INC";
    case OP_DEC: return "DEC";          case OP_EQUAL: return "EQUAL";
    case OP_NOT_EQUAL: return "NOTEQ";  case OP_DOT: return "DOT";
    case OP_ARROW: return "ARROW";      case OP_DIV: return "DIV";
    case OP_MOD: return "MOD";          case BIT_OR: return "PIPE";
    case BIT_XOR: return "CARET";       case BIT_NOT: return "TILDE";
    case BIT_LEFT_SHIFT: return "LSHIFT"; case BIT_RIGHT_SHIFT: return "RSHIFT";
    case LEFT_BRACE: return "LPAREN";   case RIGHT_BRACE: return "RPAREN";
    case LEFT_SQUARE_BRACE: return "LBRACK"; case RIGHT_SQUARE_BRACE: return "RBRACK";
    case LEFT_CURLY_BRACE: return "LBRACE";  case RIGHT_CURLY_BRACE: return "RBRACE";
    case COMMA: return "COMMA";         case STAR: return "STAR";
    case AMPERSAND: return "AMPER";     case SEMICOLON: return "SEMI";
    case COLON: return "COLON";         case QUESTION_MARK: return "QMARK";
    case ASSIGN: return "ASSIGN";       case ASSIGN_PLUS: return "ASPLUS";
    case ASSIGN_MINUS: return "ASMINUS"; case ASSIGN_DIV: return "ASDIV";
    case ASSIGN_MOD: return "ASMOD";    case ASSIGN_MUL: return "ASMUL";
    case ASSIGN_BIT_OR: return "ASPIPE"; case ASSIGN_BIT_AND: return "ASAMPER";
    case ASSIGN_BIT_XOR: return "ASCARET"; case ASSIGN_SHL: return "ASLSHIFT";
    case ASSIGN_SHR: return "ASRSHIFT";
    case INVALID: return "INVALID";     case IDENTIFIER: return "IDENT";
    case STRING: return "STRING";       case NUMBER: return "NUMBER";
    case CHAR: return "CHAR";           case ASM_BLOCK: return "ASMBLK";
    case T_EOF: return "EOF";
    default: break;
  }
  for (auto& [name, tok] : Keywords()) if (tok == t) { static thread_local std::string s; s = Utils::Uppercase(const_cast<std::string&>(name)); return s.c_str(); }
  return "?";
}

}
