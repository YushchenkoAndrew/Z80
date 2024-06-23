#pragma once
#include "Lexer/Lexer.h"
// #include "Statement/StatementVariable.h"

namespace Zcc {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/parsing-expressions.html#ambiguity-and-the-parsing-game
 *
 * Grammar:
 * 
 *  expression  -> assignment
 *  assignment  -> ( unary ( '.' | '->' ) )? IDENTIFIER '=' assignment | ternary
 *  ternary     -> logic_or '?' expression ':' expression | logic_or
 *  logic_or    -> logic_and ( '||' logic_and )*
 *  logic_and   -> binary_or ( '&&' binary_or )*
 *  binary_or   -> binary_xor ( '|' binary_xor )*
 *  binary_xor  -> binary_and ( '^' binary_and )*
 *  binary_and  -> equality ( '&' equality )*
 *  equality    -> comparison ( ('==' | '!=') comparison )?
 *  comparison  -> shift ( ('>' | '>=' | '<' | '<=') shift )?
 *  shift       -> term ( ('<<' | '>>') term )?
 *  term        -> factor ( ('-' | '+') factor )*
 *  factor      -> cast ( ('/' | '*' | '%') cast )*
 * 
 *  cast        -> '(' type ')' cast | unary
 *  type        -> 'void' | 'unsigned'? ( 'char' | 'short' | 'int') pointer? 
 *  pointer     -> '*' pointer | '*'
 *  unary       -> ('++' | '--' | 'sizeof' )* (
 *                  | ( '+' | '-' | '*' | '&' | '!' | '~' ) cast 
 *                  | postfix
 *                 )
 *  postfix     -> primary ( '[' expression ']'  | '(' arguments ')' | ('.' | '->') IDENTIFIER | '++' | '--' )*
 *  primary     -> CHAR | SHORT | INT | UNSIGNED_CHAR | UNSIGNED_SHORT | UNSIGNED_INT 
 *                 | IDENTIFIER | STRING | '(' expression ')'
 * 
 */
class Parser {
public:
  Parser(std::vector<std::shared_ptr<Token>>& t): tokens(t) {}
  ~Parser() { reset(); }

  bool scan() {
    reset();
    // if (lexer.scan(src)) { errors.insert(errors.end(), lexer.errors.begin(), lexer.errors.end()); return true; }
    program();
    return errors.size();
  }

private:
  inline void reset() { nCurr = 0; /** stmt.clear(); */ errors.clear(); }


// public:
//   template<int32_t T>
//   std::shared_ptr<Statement> Process(Int2Type<T>) {
//     error(peekPrev(), "Unknown operation");
//     return std::make_shared<Statement>();
//   }


  inline std::shared_ptr<Expression> expression() {
    return assignment();
    // consume(TokenT::PLUS, "Expect '+' before expression.");
    // return shift(size);
  }

  inline std::shared_ptr<Expression> assignment() {
    // TODO:
    // auto expr = unary(size);

    // while (match<2>({ TokenT::LEFT_SHIFT, TokenT::RIGHT_SHIFT })) {
    //   std::shared_ptr<Token> operation = peekPrev();
    //   auto right = term(size);
    //   expr = std::make_shared<ExpressionBinary>(expr, operation, right);
    // }

    // return expr;
  }

  inline std::shared_ptr<Expression> ternary() {
    auto expr = logic_or();

    while (match<1>({ TokenT::QUESTION_MARK })) {
      auto left = expression();
      consume(TokenT::COLON, "Expect ':' after expression.");
      auto left = expression();

      // TODO:
      // expr = ...
    }

    return expr;
  }

  inline std::shared_ptr<Expression> logic_or() {
    auto expr = logic_and();

    while (match<1>({ TokenT::OP_OR })) {
      auto right = logic_and();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> logic_and() {
    auto expr = binary_or();

    while (match<1>({ TokenT::OP_AND })) {
      auto right = binary_or();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> binary_or() {
    auto expr = binary_xor();

    while (match<1>({ TokenT::BIT_OR })) {
      auto right = binary_xor();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> binary_xor() {
    auto expr = binary_and();

    while (match<1>({ TokenT::BIT_XOR })) {
      auto right = binary_and();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> binary_and() {
    auto expr = equality();

    while (match<1>({ TokenT::BIT_XOR })) {
      auto right = equality();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> equality() {
    auto expr = comparison();

    if (match<2>({ TokenT::OP_EQUAL, TokenT::OP_NOT_EQUAL })) {
      auto right = comparison();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> comparison() {
    auto expr = shift();

    if (match<4>({ TokenT::OP_MORE, TokenT::OP_MORE_OR_EQUAL, TokenT::OP_LESS, TokenT::OP_LESS_OR_EQUAL })) {
      auto right = shift();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> shift() {
    auto expr = term();

    if (match<2>({ TokenT::BIT_RIGHT_SHIFT, TokenT::BIT_LEFT_SHIFT })) {
      auto right = term();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> term() {
    auto expr = factor();

    while (match<2>({ TokenT::OP_PLUS, TokenT::OP_MINUS })) {
      auto right = factor();
      // TODO:
      // expr = ....
    }

    return expr;
  }

  inline std::shared_ptr<Expression> term() {
    auto expr = cast();

    while (match<3>({ TokenT::OP_DIV, TokenT::STAR, TokenT::OP_MOD })) {
      auto right = cast();
      // TODO:
      // expr = ....
    }

    return expr;
  }

//  *  cast        -> '(' typename ')' cast | unary
//  *  unary       -> ('++' | '--' | 'sizeof' )* (
//  *                  | ( '+' | '-' | '*' | '&' | '!' | '~' ) cast 
//  *                  | postfix
//  *                 )
//  *  postfix     -> primary ( '[' expression ']'  | '(' arguments ')' | ('.' | '->') IDENTIFIER | '++' | '--' )*
//  *  primary     -> CHAR | SHORT | INT | UNSIGNED_CHAR | UNSIGNED_SHORT | UNSIGNED_INT 
//  *                 | IDENTIFIER | STRING | '(' expression ')'
//  * 
//  *  type        -> 'void' | 'unsigned'? ( 'char' | 'short' | 'int')
//  *  pointer     -> '*' pointer | '*'

  inline std::shared_ptr<Expression> cast() {
    if (!match<1>({ TokenT::LEFT_BRACE })) return unary();

    // TODO:
    type();

    cast();


    // return expr;
  }



  inline std::shared_ptr<Expression> unary() {
    while (match<3>({ TokenT::OP_INC, TokenT::OP_DEC, TokenT::W_SIZEOF })) {
      // TODO:
      // std::shared_ptr<Token> operation = peekPrev();
      // return std::make_shared<ExpressionUnary>(operation, unary(size));
    }

    if (match<3>({ TokenT::OP_PLUS, TokenT::OP_MINUS, TokenT::STAR, TokenT::AMPERSAND, TokenT::OP_NOT, TokenT::BIT_NOT })) {
      // TODO
      cast();
    } else postfix();
  }

  inline std::shared_ptr<Expression> postfix() {
    auto expr = primary();

    // TODO:
    // if (match<1>({ TokenT::LEFT_SQUARE_BRACE })) {}

  }



  inline std::shared_ptr<Expression> type() {

    // make it lambda ??
    // inline std::shared_ptr<Expression> pointer() {
    // TODO: ????????
    // while (match<1>({ TokenT::STAR })) {
    //   poin
    // }
  // }
    // TODO:
  }

  inline std::shared_ptr<Expression> primary() {
  }




  // inline std::shared_ptr<Statement> Process(Int2Type<TokenT::OP_ORG>) { return std::make_shared<StatementAddress>(peekPrev(), literal(2)); }

private:
  inline std::shared_ptr<Token> peek() { return tokens[nCurr]; }
  inline std::shared_ptr<Token> peekPrev() { return tokens[nCurr - 1]; }
  inline bool isAtEnd() { return peek()->token == TokenT::CMD_EOF; }
  inline bool check(TokenT type) { return isAtEnd() ? false : peek()->token == type; }

  inline std::shared_ptr<Token> advance() {
    if (!isAtEnd()) nCurr++;
    return peekPrev();
  }

  template<int32_t T>
  bool match(std::array<TokenT, T> types) {
    for (auto &type : types) {
      if (!check(type)) continue;
      advance();
      return true;
    }

    return false;
  }

  inline void consume(TokenT type, std::string message) {
    if (check(type)) { advance(); return; }
    // error(peek(), message);
    error(advance(), message);
  }

  void error(std::shared_ptr<Token> token, std::string message) {
    std::stringstream ss;
    ss << "[Ln " << token->line << " Col " << token->col << "] Error at '" << token->lexeme << "': " << message << "\n";
    errors.push_back(ss.str());
  }


public:
  std::vector<std::shared_ptr<Token>>& tokens;

  // std::vector<std::shared_ptr<Statement>> stmt;
  std::vector<std::string> errors;

private:
  int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};