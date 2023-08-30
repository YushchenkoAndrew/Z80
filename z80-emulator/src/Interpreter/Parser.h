#pragma once
#include "Lexer.h"
#include "Expression/ExpressionVisitor.h"

namespace Interpreter {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/parsing-expressions.html#ambiguity-and-the-parsing-game
 *
 * Grammar:
 *  program     -> declaration* EOF
 *  declaration -> variable | func | statement
 *  variable    -> IDENTIFIER 'EQU' term 
 *  func        -> IDENTIFIER ':'
 *  statement   -> COMMAND (expression)?
 
 *  term        -> unary (('+' | '-' | '|' | '&' | '^') unary)*
 *  unary       -> ('!' | '-') unary | literal
 * 
 *  expression  -> argument  (',' argument)?
 *  argument    -> REGISTER | literal | "(" operation ")"
 *  operation   -> literal | REGISTER ('+' NUMBER)?
 *  literal     -> NUMBER | IDENTIFIER | STRING;
 */
class Parser {
public:
  Parser(Lexer l): lexer(l) {}

  void scan();

  void program() {}
  void declaration() {
    if (match<1>({ TokenT::IDENTIFIER })) {
      Token label = peekPrev();
      if (match<1>({ TokenT::COLON })) {

      }

      if (match<1>({ TokenT::OP_EQU })) {
        Expression expr = term();

      }
    }

    statement();
  }

  void statement() {
    AnyType<-1, Parser*>::GetValue() = this;
    AnyType<-1, int32_t>::GetValue() = peek().token;

    foreach<CommandList, AnyType<-1, Parser*>>::Key2Process();
  }



  inline Expression term() {
    Expression expr = unary();

    while (match<5>({ TokenT::BIT_AND, TokenT::BIT_OR, TokenT::BIT_XOR, TokenT::PLUS, TokenT::MINUS })) {
      Token operation = peekPrev();
      Expression right = unary();
      expr = ExpressionBinary(expr, operation.token, right);
    }

    return expr;
  }

  inline Expression unary() {
    if (match<2>({ TokenT::MINUS, TokenT::BIT_NOT })) {
      Token operation = peekPrev();
      return ExpressionUnary(operation.token, unary());
    }

    return literal();
  }

  Expression literal() {
    if (match<2>({ TokenT::NUMBER, TokenT::STRING })) {
      return ExpressionLiteral(peekPrev());
    }

    // TODO: ??????
    // if (this.match(TokenTypeEnum.IDENTIFIER)) {
    //   return new ConditionVariableExpression(this.peekPrev());
    // }


    // if (this.match(TokenTypeEnum.LEFT_BRACE)) {
    //   const expr = this.expression();
    //   this.consume(TokenTypeEnum.RIGHT_BRACE, `Expect ')' after expression.`);

    //   return new ConditionGroupExpression(expr);
    // }

    // throw this.error(this.peek(), 'Unknown token.');

  }


  // TODO: Made implementation for this, for each cmd
public:
  template<int32_t T>
  void Process(Int2Type<T>) { }


private:

  inline Token peek() { return lexer.vTokens[nCurr]; }
  inline Token peekPrev() { return lexer.vTokens[nCurr - 1]; }
  inline bool isAtEnd() { return peek().token == TokenT::OP_EOF; }
  inline bool check(TokenT type) { return isAtEnd() ? false : peek().token == type; }

  inline Token advance() {
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
    // TODO:
    // throw error(peek(), message);
  }

   void error(TokenT token, std::string message) {
    // TODO:
    // if (token.token == TokenT::OP_EOF) {
    //   return new Error(`[Ln ${token.line}] Error: ${message}`);
    // }

    // return new Error(
    //   `[Ln ${token.line} Col ${token.pos}] Error at '${token.lexeme}': ${message}`,
    // );
  }



private:
  const Lexer lexer;

  int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};