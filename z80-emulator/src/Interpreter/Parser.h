#pragma once
#include "Lexer.h"
#include "Statement/StatementVisitor.h"

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

    // Statement t = 
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
  Statement Process(Int2Type<T>) {
    // TODO: Add error
    return Statement();
  }

  // NOTE: No Arg Command
  inline Statement Process(Int2Type<TokenT::CMD_CCF>)  { return StatementNoArgCommand(0x003F, TokenT::CMD_CCF); }
  inline Statement Process(Int2Type<TokenT::CMD_CPD>)  { return StatementNoArgCommand(0xEDA9, TokenT::CMD_CPD); }
  inline Statement Process(Int2Type<TokenT::CMD_CPDR>) { return StatementNoArgCommand(0xEDB9, TokenT::CMD_CPDR); }
  inline Statement Process(Int2Type<TokenT::CMD_CPI>)  { return StatementNoArgCommand(0xEDA1, TokenT::CMD_CPI); }
  inline Statement Process(Int2Type<TokenT::CMD_CPIR>) { return StatementNoArgCommand(0xEDB1, TokenT::CMD_CPIR); }
  inline Statement Process(Int2Type<TokenT::CMD_CPL>)  { return StatementNoArgCommand(0x002F, TokenT::CMD_CPL); }
  inline Statement Process(Int2Type<TokenT::CMD_DAA>)  { return StatementNoArgCommand(0x0027, TokenT::CMD_DAA); }
  inline Statement Process(Int2Type<TokenT::CMD_DI>)   { return StatementNoArgCommand(0x00F3, TokenT::CMD_DI); }
  inline Statement Process(Int2Type<TokenT::CMD_EI>)   { return StatementNoArgCommand(0x00FB, TokenT::CMD_EI); }
  inline Statement Process(Int2Type<TokenT::CMD_EXX>)  { return StatementNoArgCommand(0x00D9, TokenT::CMD_EXX); }
  inline Statement Process(Int2Type<TokenT::CMD_HALT>) { return StatementNoArgCommand(0x0076, TokenT::CMD_HALT); }
  inline Statement Process(Int2Type<TokenT::CMD_IND>)  { return StatementNoArgCommand(0xEDAA, TokenT::CMD_IND); }
  inline Statement Process(Int2Type<TokenT::CMD_INDR>) { return StatementNoArgCommand(0xEDBA, TokenT::CMD_INDR); }
  inline Statement Process(Int2Type<TokenT::CMD_INI>)  { return StatementNoArgCommand(0xEDA2, TokenT::CMD_INI); }
  inline Statement Process(Int2Type<TokenT::CMD_INIR>) { return StatementNoArgCommand(0xEDB2, TokenT::CMD_INIR); }
  inline Statement Process(Int2Type<TokenT::CMD_LDD>)  { return StatementNoArgCommand(0xEDA8, TokenT::CMD_LDD); }
  inline Statement Process(Int2Type<TokenT::CMD_LDDR>) { return StatementNoArgCommand(0xEDB8, TokenT::CMD_LDDR); }
  inline Statement Process(Int2Type<TokenT::CMD_LDI>)  { return StatementNoArgCommand(0xEDA0, TokenT::CMD_LDI); }
  inline Statement Process(Int2Type<TokenT::CMD_LDIR>) { return StatementNoArgCommand(0xEDB0, TokenT::CMD_LDIR); }
  inline Statement Process(Int2Type<TokenT::CMD_NEG>)  { return StatementNoArgCommand(0xED44, TokenT::CMD_NEG); }
  inline Statement Process(Int2Type<TokenT::CMD_NOP>)  { return StatementNoArgCommand(0x0000, TokenT::CMD_NOP); }
  inline Statement Process(Int2Type<TokenT::CMD_OTDR>) { return StatementNoArgCommand(0xEDBB, TokenT::CMD_OTDR); }
  inline Statement Process(Int2Type<TokenT::CMD_OTIR>) { return StatementNoArgCommand(0xEDB3, TokenT::CMD_OTIR); }
  inline Statement Process(Int2Type<TokenT::CMD_OUTD>) { return StatementNoArgCommand(0xEDAB, TokenT::CMD_OUTD); }
  inline Statement Process(Int2Type<TokenT::CMD_OUTI>) { return StatementNoArgCommand(0xEDA3, TokenT::CMD_OUTI); }
  inline Statement Process(Int2Type<TokenT::CMD_RETI>) { return StatementNoArgCommand(0xED4D, TokenT::CMD_RETI); }
  inline Statement Process(Int2Type<TokenT::CMD_RETN>) { return StatementNoArgCommand(0xED45, TokenT::CMD_RETN); }
  inline Statement Process(Int2Type<TokenT::CMD_RLA>)  { return StatementNoArgCommand(0x0017, TokenT::CMD_RLA); }
  inline Statement Process(Int2Type<TokenT::CMD_RLCA>) { return StatementNoArgCommand(0x0007, TokenT::CMD_RLCA); }
  inline Statement Process(Int2Type<TokenT::CMD_RLD>)  { return StatementNoArgCommand(0xED6F, TokenT::CMD_RLD); }
  inline Statement Process(Int2Type<TokenT::CMD_RRA>)  { return StatementNoArgCommand(0x001F, TokenT::CMD_RRA); }
  inline Statement Process(Int2Type<TokenT::CMD_RRCA>) { return StatementNoArgCommand(0x000F, TokenT::CMD_RRCA); }
  inline Statement Process(Int2Type<TokenT::CMD_RRD>)  { return StatementNoArgCommand(0xED67, TokenT::CMD_RRD); }
  inline Statement Process(Int2Type<TokenT::CMD_SCF>)  { return StatementNoArgCommand(0x0037, TokenT::CMD_SCF); }


  // NOTE: One arg command
  inline Statement Process(Int2Type<TokenT::CMD_AND>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO:
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_CP>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_DEC>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO:
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_DJNZ>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO:
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_IM>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_INC>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_OR>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_POP>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_PUSH>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RET>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RL>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RLC>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RR>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RRC>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RST>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SLA>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SRA>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SRL>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SUB>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_XOR>) {
    // return StatementNoArgCommand(0x0037, TokenT::CMD_SCF);
    // TODO: 
    return Statement();
  }


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