#pragma once
#include "Lexer.h"
#include "Statement/StatementVariable.h"

namespace Interpreter {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/parsing-expressions.html#ambiguity-and-the-parsing-game
 *
 * Grammar:
 *  program     -> declaration* EOF
 *  declaration -> variable | func | statement
 *  include     -> '#' IDENTIFIER '(' STRING ')'
 *  variable    -> IDENTIFIER 'EQU' shift 
 *  func        -> IDENTIFIER ':'
 *  statement   -> COMMAND (expression)?  // NOTE: Grammar for each command will be hardcoded based on manual, this grammar is just a common example
 * 
 *  offset      -> '+' shift
 *  shift       -> term (('>>' | '<<') term)*
 *  term        -> bit (('+' | '-' | '..') bit)*
 *  bit         -> unary (('|' | '&' | '^') unary)*
 *  unary       -> ('~' | '-') unary | literal
 * 
 *  expression  -> argument  (',' argument)?
 *  argument    -> REGISTER | literal | "(" operation ")"
 *  operation   -> literal | REGISTER ('+' NUMBER)?
 *  literal     -> NUMBER | IDENTIFIER | STRING;
 */
class Parser {
public:
  Parser(): lexer(Lexer()) {}
  Parser(Lexer l): lexer(l) {}

  ~Parser() { reset(); }

  bool scan(std::string src) {
    reset();

    if (lexer.scan(src)) { errors.insert(errors.end(), lexer.errors.begin(), lexer.errors.end()); return true; }
    program();
    return errors.size();
  }

private:
  inline void reset() { nCurr = 0; stmt.clear(); errors.clear(); }

  inline void program() {
    while (!isAtEnd()) stmt.push_back(declaration());
  }


  std::shared_ptr<Statement> declaration() {
    if (match<1>({ TokenT::IDENTIFIER })) {
      switch (peek()->token) {
        case TokenT::COLON: return func();
        case TokenT::OP_EQU: return variable();
      }

      error(advance(), "Unknown token.");
      return std::make_shared<Statement>();
    } else if (match<1>({ TokenT::OP_INCLUDE })) {
      consume(TokenT::STRING, "Expect string after include stmt.");
      return std::make_shared<StatementInclude>(std::make_shared<ExpressionLiteral>(peekPrev(), 0));
    }

    return statement();
  }

  std::shared_ptr<Statement> func() {
    std::shared_ptr<Token> label = peekPrev();
    consume(TokenT::COLON, "Expect ':' before expression.");
    return std::make_shared<StatementVariable>(label);
  }

  std::shared_ptr<Statement> variable() {
    std::shared_ptr<Token> label = peekPrev();
    consume(TokenT::OP_EQU, "Expect 'EQU' before expression.");
    return std::make_shared<StatementVariable>(label, shift());
  }

  std::shared_ptr<Statement> statement() {
    AnyType<-1, int32_t>::GetValue() = advance()->token;
    return foreach<CommandList, Parser>::Key2Process(this);
  }

  inline std::shared_ptr<Expression> offset(int32_t size = 0) {
    consume(TokenT::PLUS, "Expect '+' before expression.");
    return shift(size);
  }

  inline std::shared_ptr<Expression> shift(int32_t size = 0) {
    auto expr = term(size);

    while (match<2>({ TokenT::LEFT_SHIFT, TokenT::RIGHT_SHIFT })) {
      std::shared_ptr<Token> operation = peekPrev();
      auto right = term(size);
      expr = std::make_shared<ExpressionBinary>(expr, operation, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> term(int32_t size = 0) {
    auto expr = bit(size);

    while (match<3>({ TokenT::PLUS, TokenT::MINUS, TokenT::CONCATENATE })) {
      std::shared_ptr<Token> operation = peekPrev();
      auto right = bit(size);
      expr = std::make_shared<ExpressionBinary>(expr, operation, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> bit(int32_t size = 0) {
    auto expr = unary(size);

    while (match<3>({ TokenT::BIT_AND, TokenT::BIT_OR, TokenT::BIT_XOR })) {
      std::shared_ptr<Token> operation = peekPrev();
      auto right = unary(size);
      expr = std::make_shared<ExpressionBinary>(expr, operation, right);
    }

    return expr;
  }


  inline std::shared_ptr<Expression> unary(int32_t size = 0) {
    if (match<2>({ TokenT::MINUS, TokenT::BIT_NOT })) {
      std::shared_ptr<Token> operation = peekPrev();
      return std::make_shared<ExpressionUnary>(operation, unary(size));
    }

    return literal(size);
  }

  inline std::shared_ptr<Expression> literal(int32_t size = 0) {
    if (match<2>({ TokenT::NUMBER, TokenT::STRING })) {
      return std::make_shared<ExpressionLiteral>(peekPrev(), size);
    }

    if (match<1>({ TokenT::IDENTIFIER })) {
      return std::make_shared<ExpressionVariable>(peekPrev(), size);
    }

    error(advance(), "Unknown token.");
    return std::make_shared<Expression>();
  }

public:
  template<int32_t T>
  std::shared_ptr<Statement> Process(Int2Type<T>) {
    error(peekPrev(), "Unknown operation");
    return std::make_shared<Statement>();
  }


  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::OP_ORG>) { return std::make_shared<StatementAddress>(peekPrev(), literal(2)); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::OP_DB>) {
    auto expr = peekPrev();
    std::vector<std::shared_ptr<Expression>> data;

    do { data.push_back(shift()); } while(match<1>({ TokenT::COMMA }));
    return std::make_shared<StatementAllocate>(expr, data, 1);
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::OP_DW>) {
    auto expr = peekPrev();
    std::vector<std::shared_ptr<Expression>> data;

    do { data.push_back(shift()); } while(match<1>({ TokenT::COMMA }));
    return std::make_shared<StatementAllocate>(expr, data, 2, true);
  }

  // NOTE: No Arg Command
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CCF>)  { return std::make_shared<StatementNoArgCommand>(0x003F, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CPD>)  { return std::make_shared<StatementNoArgCommand>(0xEDA9, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CPDR>) { return std::make_shared<StatementNoArgCommand>(0xEDB9, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CPI>)  { return std::make_shared<StatementNoArgCommand>(0xEDA1, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CPIR>) { return std::make_shared<StatementNoArgCommand>(0xEDB1, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CPL>)  { return std::make_shared<StatementNoArgCommand>(0x002F, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_DAA>)  { return std::make_shared<StatementNoArgCommand>(0x0027, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_DI>)   { return std::make_shared<StatementNoArgCommand>(0x00F3, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_EI>)   { return std::make_shared<StatementNoArgCommand>(0x00FB, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_EXX>)  { return std::make_shared<StatementNoArgCommand>(0x00D9, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_HALT>) { return std::make_shared<StatementNoArgCommand>(0x0076, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_IND>)  { return std::make_shared<StatementNoArgCommand>(0xEDAA, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_INDR>) { return std::make_shared<StatementNoArgCommand>(0xEDBA, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_INI>)  { return std::make_shared<StatementNoArgCommand>(0xEDA2, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_INIR>) { return std::make_shared<StatementNoArgCommand>(0xEDB2, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_LDD>)  { return std::make_shared<StatementNoArgCommand>(0xEDA8, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_LDDR>) { return std::make_shared<StatementNoArgCommand>(0xEDB8, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_LDI>)  { return std::make_shared<StatementNoArgCommand>(0xEDA0, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_LDIR>) { return std::make_shared<StatementNoArgCommand>(0xEDB0, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_NEG>)  { return std::make_shared<StatementNoArgCommand>(0xED44, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_NOP>)  { return std::make_shared<StatementNoArgCommand>(0x0000, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_OTDR>) { return std::make_shared<StatementNoArgCommand>(0xEDBB, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_OTIR>) { return std::make_shared<StatementNoArgCommand>(0xEDB3, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_OUTD>) { return std::make_shared<StatementNoArgCommand>(0xEDAB, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_OUTI>) { return std::make_shared<StatementNoArgCommand>(0xEDA3, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RETI>) { return std::make_shared<StatementNoArgCommand>(0xED4D, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RETN>) { return std::make_shared<StatementNoArgCommand>(0xED45, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RLA>)  { return std::make_shared<StatementNoArgCommand>(0x0017, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RLCA>) { return std::make_shared<StatementNoArgCommand>(0x0007, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RLD>)  { return std::make_shared<StatementNoArgCommand>(0xED6F, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RRA>)  { return std::make_shared<StatementNoArgCommand>(0x001F, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RRCA>) { return std::make_shared<StatementNoArgCommand>(0x000F, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RRD>)  { return std::make_shared<StatementNoArgCommand>(0xED67, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SCF>)  { return std::make_shared<StatementNoArgCommand>(0x0037, peekPrev()); }


  // NOTE: One arg command
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RL>)  { return basicRegisterOperation(0xCB16, 0xCB10, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RLC>) { return basicRegisterOperation(0xCB06, 0xCB00, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RR>)  { return basicRegisterOperation(0xCB1E, 0xCB18, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RRC>) { return basicRegisterOperation(0xCB0E, 0xCB08, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SLA>) { return basicRegisterOperation(0xCB26, 0xCB20, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SRA>) { return basicRegisterOperation(0xCB2E, 0xCB28, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SRL>) { return basicRegisterOperation(0xCB3E, 0xCB38, peekPrev()); }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_AND>) { return basicRegisterOperation(0x00A6, 0x00A0, 0x00E6, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CP>)  { return basicRegisterOperation(0x00BE, 0x00B8, 0x00FE, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_OR>)  { return basicRegisterOperation(0x00B6, 0x00B0, 0x00F6, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SUB>) { return basicRegisterOperation(0x0096, 0x0090, 0x00D6, peekPrev()); }
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_XOR>) { return basicRegisterOperation(0x00AE, 0x00A8, 0x00EE, peekPrev()); }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_DEC>) {
    auto cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto stmt = addressRegisterOperation(0x0035, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    switch(advance()->token) {
      case TokenT::REG_A:  return std::make_shared<StatementNoArgCommand>(0x003D, cmd);
      case TokenT::REG_B:  return std::make_shared<StatementNoArgCommand>(0x0005, cmd);
      case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0x000B, cmd);
      case TokenT::REG_C:  return std::make_shared<StatementNoArgCommand>(0x000D, cmd);
      case TokenT::REG_D:  return std::make_shared<StatementNoArgCommand>(0x0015, cmd);
      case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0x001B, cmd);
      case TokenT::REG_E:  return std::make_shared<StatementNoArgCommand>(0x001D, cmd);
      case TokenT::REG_H:  return std::make_shared<StatementNoArgCommand>(0x0025, cmd);
      case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0x002B, cmd);
      case TokenT::REG_IX: return std::make_shared<StatementNoArgCommand>(0xDD2B, cmd);
      case TokenT::REG_IY: return std::make_shared<StatementNoArgCommand>(0xFD2B, cmd);
      case TokenT::REG_L:  return std::make_shared<StatementNoArgCommand>(0x002D, cmd);
      case TokenT::REG_SP: return std::make_shared<StatementNoArgCommand>(0x003B, cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_DJNZ>) {
    return std::make_shared<StatementLambda>(StatementLambda(peekPrev(), shift(2), [](std::vector<uint32_t> argv) { return (uint32_t)(0x1000 | (argv.back() & 0xFF)); }));
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_IM>) {
    return std::make_shared<StatementLambda>(StatementLambda(peekPrev(), literal(1), [](std::vector<uint32_t> argv) {
      switch (argv[0]) {
        case 0x00: return (uint32_t)0xED46;
        case 0x01: return (uint32_t)0xED56;
        case 0x02: return (uint32_t)0xED5E;
      }
      return (uint32_t)0x00;
    }));
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_INC>) {
    auto cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto stmt = addressRegisterOperation(0x0034, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    switch(advance()->token) {
      case TokenT::REG_A:  return std::make_shared<StatementNoArgCommand>(0x003C, cmd);
      case TokenT::REG_B:  return std::make_shared<StatementNoArgCommand>(0x0004, cmd);
      case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0x0003, cmd);
      case TokenT::REG_C:  return std::make_shared<StatementNoArgCommand>(0x000C, cmd);
      case TokenT::REG_D:  return std::make_shared<StatementNoArgCommand>(0x0014, cmd);
      case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0x0013, cmd);
      case TokenT::REG_E:  return std::make_shared<StatementNoArgCommand>(0x001C, cmd);
      case TokenT::REG_H:  return std::make_shared<StatementNoArgCommand>(0x0024, cmd);
      case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0x0023, cmd);
      case TokenT::REG_IX: return std::make_shared<StatementNoArgCommand>(0xDD23, cmd);
      case TokenT::REG_IY: return std::make_shared<StatementNoArgCommand>(0xFD23, cmd);
      case TokenT::REG_L:  return std::make_shared<StatementNoArgCommand>(0x002C, cmd);
      case TokenT::REG_SP: return std::make_shared<StatementNoArgCommand>(0x0033, cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_POP>) {
    auto cmd = peekPrev();

    switch(advance()->token) {
      case TokenT::REG_AF: return std::make_shared<StatementNoArgCommand>(0x00F1, cmd);
      case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0x00C1, cmd);
      case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0x00D1, cmd);
      case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0x00E1, cmd);
      case TokenT::REG_IX: return std::make_shared<StatementNoArgCommand>(0xDDE1, cmd);
      case TokenT::REG_IY: return std::make_shared<StatementNoArgCommand>(0xFDE1, cmd);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_PUSH>) {
    auto cmd = peekPrev();

    switch(advance()->token) {
      case TokenT::REG_AF: return std::make_shared<StatementNoArgCommand>(0x00F5, cmd);
      case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0x00C5, cmd);
      case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0x00D5, cmd);
      case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0x00E5, cmd);
      case TokenT::REG_IX: return std::make_shared<StatementNoArgCommand>(0xDDE5, cmd);
      case TokenT::REG_IY: return std::make_shared<StatementNoArgCommand>(0xFDE5, cmd);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RET>) {
    auto cmd = peekPrev();

    switch(peek()->token) {
      case TokenT::REG_C:
      case TokenT::FLAG_C:  advance(); return std::make_shared<StatementNoArgCommand>(0x00D8, cmd);
      case TokenT::FLAG_M:  advance(); return std::make_shared<StatementNoArgCommand>(0x00F8, cmd);
      case TokenT::FLAG_NC: advance(); return std::make_shared<StatementNoArgCommand>(0x00D0, cmd);
      case TokenT::FLAG_NZ: advance(); return std::make_shared<StatementNoArgCommand>(0x00C0, cmd);
      case TokenT::FLAG_P:  advance(); return std::make_shared<StatementNoArgCommand>(0x00F0, cmd);
      case TokenT::FLAG_PE: advance(); return std::make_shared<StatementNoArgCommand>(0x00E8, cmd);
      case TokenT::FLAG_PO: advance(); return std::make_shared<StatementNoArgCommand>(0x00E0, cmd);
      case TokenT::FLAG_Z:  advance(); return std::make_shared<StatementNoArgCommand>(0x00C8, cmd);
    }

    return std::make_shared<StatementNoArgCommand>(0x00C9, cmd);
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RST>) {
    return std::make_shared<StatementLambda>(StatementLambda(peekPrev(), literal(1), [](std::vector<uint32_t> argv) {
      switch (argv[0]) {
        case 0x00: return (uint32_t)0x00C7;
        case 0x08: return (uint32_t)0x00CF;
        case 0x10: return (uint32_t)0x00D7;
        case 0x18: return (uint32_t)0x00DF;
        case 0x20: return (uint32_t)0x00E7;
        case 0x28: return (uint32_t)0x00EF;
        case 0x30: return (uint32_t)0x00F7;
        case 0x38: return (uint32_t)0x00FF;
      }
      return (uint32_t)0x00;
    }));
  }



  // NOTE: Two arg command
  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_ADC>) {
    auto cmd = peekPrev();

    std::shared_ptr<Token> reg = advance();
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    switch (reg->token) {
      case TokenT::REG_A:
        return basicRegisterOperation(0x008E, 0x0088, 0x00CE, cmd);

      case TokenT::REG_HL:
        switch(advance()->token) {
          case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0xED4A, cmd);
          case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0xED5A, cmd);
          case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0xED6A, cmd);
          case TokenT::REG_SP: return std::make_shared<StatementNoArgCommand>(0xED7A, cmd);
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return std::make_shared<Statement>();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_ADD>) {
    auto cmd = peekPrev();
    std::shared_ptr<Token> reg = advance();
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    const uint32_t mask =
      reg->token == TokenT::REG_IX ? 0xDD00 :
      reg->token == TokenT::REG_IY ? 0xFD00 : 0x0000;

    switch (reg->token) {
      case TokenT::REG_A:
        return basicRegisterOperation(0x0086, 0x0080, 0x00C6, cmd);

      case TokenT::REG_IX:
      case TokenT::REG_IY:
      case TokenT::REG_HL:
        switch(advance()->token) {
          case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0x0009 | mask, cmd);
          case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0x0019 | mask, cmd);
          case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0x0029 | mask, cmd);
          case TokenT::REG_SP: return std::make_shared<StatementNoArgCommand>(0x0039 | mask, cmd);
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return std::make_shared<Statement>();
    }

    error(reg, "Expect register to be 'A' | 'HL' | 'IX' | 'IY'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_BIT>) {
    auto cmd = peekPrev();
    auto expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      std::shared_ptr<Statement> stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: stmt = std::make_shared<StatementLambda>(StatementLambda(cmd, expr, [](std::vector<uint32_t> argv) { return (uint32_t)(argv[0] > 7 ? 0x00 : 0x00CB46 | (argv[0] << 3)); })); break;
        case TokenT::REG_IX: stmt = std::make_shared<StatementLambda>(StatementLambda(cmd, { expr, offset(1) }, [](std::vector<uint32_t> argv) { return (uint32_t)(argv[0] > 7 ? 0x00 : 0xDDCB0046 | (argv[1] << 8) | (argv[0] << 3)); })); break;
        case TokenT::REG_IY: stmt = std::make_shared<StatementLambda>(StatementLambda(cmd, { expr, offset(1) }, [](std::vector<uint32_t> argv) { return (uint32_t)(argv[0] > 7 ? 0x00 : 0xFDCB0046 | (argv[1] << 8) | (argv[0] << 3)); })); break;
        default: error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return std::make_shared<StatementLambda>(StatementLambda(cmd, expr, [](std::vector<uint32_t> argv) {
        return (uint32_t)(argv[0] > 7 ? 0x00 : 0xCB40 | (argv[0] << 3) | argv[1]);
      }, { Defs::Reg2Mask(peekPrev()->token) }));
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_CALL>) { 
    auto cmd = peekPrev();

    if (match<9>({ TokenT::REG_C, TokenT::FLAG_C, TokenT::FLAG_M, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_P, TokenT::FLAG_PE, TokenT::FLAG_PO, TokenT::FLAG_Z })) {
      std::shared_ptr<Token> flag = peekPrev();
      consume(TokenT::COMMA, "Expect ',' after first expression.");

      switch(flag->token) {
        case TokenT::REG_C:
        case TokenT::FLAG_C:  return std::make_shared<StatementOneArgCommand>(0x00DC, cmd, shift(2));
        case TokenT::FLAG_M:  return std::make_shared<StatementOneArgCommand>(0x00FC, cmd, shift(2));
        case TokenT::FLAG_NC: return std::make_shared<StatementOneArgCommand>(0x00D4, cmd, shift(2));
        case TokenT::FLAG_NZ: return std::make_shared<StatementOneArgCommand>(0x00C4, cmd, shift(2));
        case TokenT::FLAG_P:  return std::make_shared<StatementOneArgCommand>(0x00F4, cmd, shift(2));
        case TokenT::FLAG_PE: return std::make_shared<StatementOneArgCommand>(0x00EC, cmd, shift(2));
        case TokenT::FLAG_PO: return std::make_shared<StatementOneArgCommand>(0x00E4, cmd, shift(2));
        case TokenT::FLAG_Z:  return std::make_shared<StatementOneArgCommand>(0x00CC, cmd, shift(2));
      }

    } else return std::make_shared<StatementOneArgCommand>(0x00CD, cmd, shift(2));

    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_EX>) {
    auto cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      consume(TokenT::REG_SP, "Expect register to be 'SP'");
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      return addressRegisterOperation(0x00E3, cmd);
    }

    if (match<1>({ TokenT::REG_AF })) {
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      consume(TokenT::REG_AF$, "Expect register to be AF'");
      return std::make_shared<StatementNoArgCommand>(0x0008, cmd);
    }

    if (match<1>({ TokenT::REG_DE })) {
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      consume(TokenT::REG_HL, "Expect register to be 'HL'");
      return std::make_shared<StatementNoArgCommand>(0x00EB, cmd);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'DE' | '(SP)'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_IN>) {
    auto cmd = peekPrev();

    if (!match<8>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L, TokenT::REG_F })) {
      error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L' | 'F'");
      return std::make_shared<Statement>();
    }

    std::shared_ptr<Token> reg = peekPrev();
    consume(TokenT::COMMA, "Expect ',' after expression.");
    consume(TokenT::LEFT_BRACE, "Expect '(' after expression.");
    if (reg->token == TokenT::REG_A && !check(TokenT::REG_C)) {
      auto stmt = std::make_shared<StatementOneArgCommand>(0x00DB, cmd, shift(1));
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    consume(TokenT::REG_C, "Expect register to be 'C'.");
    consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");

    switch (reg->token) {
      case TokenT::REG_A: return std::make_shared<StatementNoArgCommand>(0xED78, cmd);
      case TokenT::REG_B: return std::make_shared<StatementNoArgCommand>(0xED40, cmd);
      case TokenT::REG_C: return std::make_shared<StatementNoArgCommand>(0xED48, cmd);
      case TokenT::REG_D: return std::make_shared<StatementNoArgCommand>(0xED50, cmd);
      case TokenT::REG_E: return std::make_shared<StatementNoArgCommand>(0xED58, cmd);
      case TokenT::REG_H: return std::make_shared<StatementNoArgCommand>(0xED60, cmd);
      case TokenT::REG_L: return std::make_shared<StatementNoArgCommand>(0xED68, cmd);
      case TokenT::REG_F: return std::make_shared<StatementNoArgCommand>(0xED70, cmd);
    }

    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_JP>) { 
    auto cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      std::shared_ptr<Statement> stmt;

      switch (advance()->token) {
        case TokenT::REG_HL: stmt = std::make_shared<StatementNoArgCommand>(0x00E9, cmd); break;
        case TokenT::REG_IX: stmt = std::make_shared<StatementNoArgCommand>(0xDDE9, cmd); break;
        case TokenT::REG_IY: stmt = std::make_shared<StatementNoArgCommand>(0xFDE9, cmd); break;
        default: error(peekPrev(), "Expect register to be 'HL' | 'IY' | 'IX'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<9>({ TokenT::REG_C, TokenT::FLAG_C, TokenT::FLAG_M, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_P, TokenT::FLAG_PE, TokenT::FLAG_PO, TokenT::FLAG_Z })) {
      std::shared_ptr<Token> flag = peekPrev();
      consume(TokenT::COMMA, "Expect ',' after first expression.");

      switch(flag->token) {
        case TokenT::REG_C:
        case TokenT::FLAG_C:  return std::make_shared<StatementOneArgCommand>(0x00DA, cmd, shift(2));
        case TokenT::FLAG_M:  return std::make_shared<StatementOneArgCommand>(0x00FA, cmd, shift(2));
        case TokenT::FLAG_NC: return std::make_shared<StatementOneArgCommand>(0x00D2, cmd, shift(2));
        case TokenT::FLAG_NZ: return std::make_shared<StatementOneArgCommand>(0x00C2, cmd, shift(2));
        case TokenT::FLAG_P:  return std::make_shared<StatementOneArgCommand>(0x00F2, cmd, shift(2));
        case TokenT::FLAG_PE: return std::make_shared<StatementOneArgCommand>(0x00EA, cmd, shift(2));
        case TokenT::FLAG_PO: return std::make_shared<StatementOneArgCommand>(0x00E2, cmd, shift(2));
        case TokenT::FLAG_Z:  return std::make_shared<StatementOneArgCommand>(0x00CA, cmd, shift(2));
      }
    } else return std::make_shared<StatementOneArgCommand>(0xC3, cmd, shift(2));


    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_JR>) {
    auto cmd = peekPrev();

    if (match<5>({ TokenT::REG_C, TokenT::FLAG_C, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_Z })) {
      std::shared_ptr<Token> flag = peekPrev();
      consume(TokenT::COMMA, "Expect ',' after first expression.");

      // NOTE: Need to do it such way because compiler crashed with internal error (| ~ | `) 
      return std::make_shared<StatementLambda>(StatementLambda(cmd, shift(2), [](std::vector<uint32_t> argv) {
        switch(argv.back()) {
          case (uint32_t)TokenT::REG_C:
          case (uint32_t)TokenT::FLAG_C:  return (uint32_t)(0x3800 | (argv[1] & 0xFF));
          case (uint32_t)TokenT::FLAG_NC: return (uint32_t)(0x3000 | (argv[1] & 0xFF));
          case (uint32_t)TokenT::FLAG_NZ: return (uint32_t)(0x2000 | (argv[1] & 0xFF));
          case (uint32_t)TokenT::FLAG_Z:  return (uint32_t)(0x2800 | (argv[1] & 0xFF));
        }

        return (uint32_t)0x00;
      }, { (uint32_t)flag->token }));

    } else return std::make_shared<StatementLambda>(StatementLambda(cmd, shift(2), [](std::vector<uint32_t> argv) { return (uint32_t)(0x1800 | (argv.back() & 0xFF)); }));

    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_LD>) { 
    auto cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      switch (peek()->token) {
        case TokenT::REG_BC:
        case TokenT::REG_DE: {
          std::shared_ptr<Token> reg = advance();
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");
          consume(TokenT::REG_A, "Expect register 'A' after expression.");

          switch(reg->token) {
            case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0x0002, cmd);
            case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0x0012, cmd);
          }

          return std::make_shared<Statement>();
        }

        case TokenT::REG_HL:
          consume(TokenT::REG_HL, "Expect 'HL' after expression.");
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return std::make_shared<StatementNoArgCommand>(0x70 | Defs::Reg2Mask(peekPrev()->token), cmd);
          }

          return std::make_shared<StatementOneArgCommand>(0x36, cmd, shift(1));

        case TokenT::REG_IX: {
          consume(TokenT::REG_IX, "Expect 'IX' after expression.");
          auto expr = offset(1);

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return std::make_shared<StatementLambda>(StatementLambda(cmd, expr, [](std::vector<uint32_t> argv) { return (uint32_t)(0xDD7000 | (argv[1] << 8) | argv[0]); }, { Defs::Reg2Mask(peekPrev()->token)}));
          }

          return std::make_shared<StatementLambda>(StatementLambda(cmd, { expr, shift(1) }, [](std::vector<uint32_t> argv) { return (uint32_t)(0xDD360000 | (argv[0] << 8) | argv[1]); }, { Defs::Reg2Mask(peekPrev()->token)}));
        }

        case TokenT::REG_IY: {
          consume(TokenT::REG_IY, "Expect 'IY' after expression.");
          auto expr = offset(1);

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return std::make_shared<StatementLambda>(StatementLambda(cmd, expr, [](std::vector<uint32_t> argv) { return (uint32_t)(0xFD7000 | (argv[1] << 8) | argv[0]); }, { Defs::Reg2Mask(peekPrev()->token)}));
          }

          return std::make_shared<StatementLambda>(StatementLambda(cmd, { expr, shift(1) }, [](std::vector<uint32_t> argv) { return (uint32_t)(0xFD360000 | (argv[0] << 8) | argv[1]); }, { Defs::Reg2Mask(peekPrev()->token)}));
        }
      }


      auto expr = shift(2);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after expression.");

      switch (advance()->token) {
        case TokenT::REG_A:  return std::make_shared<StatementOneArgCommand>(0x0032, cmd, expr);
        case TokenT::REG_BC: return std::make_shared<StatementOneArgCommand>(0xED43, cmd, expr);
        case TokenT::REG_DE: return std::make_shared<StatementOneArgCommand>(0xED53, cmd, expr);
        case TokenT::REG_HL: return std::make_shared<StatementOneArgCommand>(0x0022, cmd, expr);
        case TokenT::REG_IX: return std::make_shared<StatementOneArgCommand>(0xDD22, cmd, expr);
        case TokenT::REG_IY: return std::make_shared<StatementOneArgCommand>(0xFD22, cmd, expr);
        case TokenT::REG_SP: return std::make_shared<StatementOneArgCommand>(0xED73, cmd, expr);
      }


      error(peekPrev(), "Expect register to be 'A' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY' | 'SP'");
      return std::make_shared<Statement>();
    }

    switch (advance()->token) {
      case TokenT::REG_A:
        consume(TokenT::COMMA, "Expect ',' after expression.");

        if (match<1>({ TokenT::LEFT_BRACE })) {
          std::shared_ptr<Statement> stmt;
          switch (peek()->token) {
            case TokenT::REG_BC: advance(); stmt = std::make_shared<StatementNoArgCommand>(0x000A, cmd); break;
            case TokenT::REG_DE: advance(); stmt = std::make_shared<StatementNoArgCommand>(0x001A, cmd); break;

            case TokenT::REG_HL:
            case TokenT::REG_IX: 
            case TokenT::REG_IY: stmt = addressRegisterOperation(0x007E, cmd); break;
          
            default: stmt = std::make_shared<StatementOneArgCommand>(0x003A, cmd, shift(2)); break;
          }

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
          return std::make_shared<StatementNoArgCommand>(0x0078 | Defs::Reg2Mask(peekPrev()->token), cmd);
        }

        if (match<1>({ TokenT::REG_I })) return std::make_shared<StatementNoArgCommand>(0xED57, cmd);
        if (match<1>({ TokenT::REG_R })) return std::make_shared<StatementNoArgCommand>(0xED5F, cmd);

        return std::make_shared<StatementOneArgCommand>(0x003E, cmd, shift(1));

      case TokenT::REG_B:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation(0x0046, 0x0040, 0x0006, cmd);

      case TokenT::REG_C:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation(0x004E, 0x0048, 0x000E, cmd);

      case TokenT::REG_D:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation(0x0056, 0x0050, 0x0016, cmd);

      case TokenT::REG_E:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation(0x005E, 0x0058, 0x001E, cmd);

      case TokenT::REG_H:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation(0x0066, 0x0060, 0x0026, cmd);

      case TokenT::REG_L: 
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation(0x006E, 0x0068, 0x002E, cmd);

      case TokenT::REG_I:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        consume(TokenT::REG_A, "Expect register 'A' after expression.");
        return std::make_shared<StatementNoArgCommand>(0xED47, cmd);

      case TokenT::REG_R:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        consume(TokenT::REG_A, "Expect register 'A' after expression.");
        return std::make_shared<StatementNoArgCommand>(0xED4F, cmd);

      case TokenT::REG_BC:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          auto stmt = std::make_shared<StatementOneArgCommand>(0xED4B, cmd, shift(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return std::make_shared<StatementOneArgCommand>(0x0001, cmd, shift(2));

      case TokenT::REG_DE:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          auto stmt = std::make_shared<StatementOneArgCommand>(0xED5B, cmd, shift(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return std::make_shared<StatementOneArgCommand>(0x0011, cmd, shift(2));

      case TokenT::REG_HL:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          auto stmt = std::make_shared<StatementOneArgCommand>(0x002A, cmd, shift(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return std::make_shared<StatementOneArgCommand>(0x0021, cmd, shift(2));


      case TokenT::REG_IX:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          auto stmt = std::make_shared<StatementOneArgCommand>(0xDD2A, cmd, shift(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return std::make_shared<StatementOneArgCommand>(0xDD21, cmd, shift(2));

      case TokenT::REG_IY:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          auto stmt = std::make_shared<StatementOneArgCommand>(0xFD2A, cmd, shift(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return std::make_shared<StatementOneArgCommand>(0xFD21, cmd, shift(2));

      case TokenT::REG_SP: 
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          auto stmt = std::make_shared<StatementOneArgCommand>(0x002A, cmd, shift(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        switch (peek()->token) {
          case TokenT::REG_HL: advance(); return std::make_shared<StatementNoArgCommand>(0x00F9, cmd);
          case TokenT::REG_IX: advance(); return std::make_shared<StatementNoArgCommand>(0xDDF9, cmd);
          case TokenT::REG_IY: advance(); return std::make_shared<StatementNoArgCommand>(0xFDF9, cmd);
        }

        return std::make_shared<StatementOneArgCommand>(0x0031, cmd, shift(2));

    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L' | 'I' | 'R' | 'BC' | 'DE' | 'HL' | 'SP'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_OUT>) {
    auto cmd = peekPrev();

    consume(TokenT::LEFT_BRACE, "Expect '(' after expression.");

    if (!match<1>({ TokenT::REG_C })) {
      auto expr = shift(1);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after expression.");
      consume(TokenT::REG_A, "Expect register to be 'A'");
      return std::make_shared<StatementOneArgCommand>(0x00D3, cmd, expr);
    }

    consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
    consume(TokenT::COMMA, "Expect ',' after expression.");
    switch (advance()->token) {
      case TokenT::REG_A:  return std::make_shared<StatementNoArgCommand>(0xED79, cmd);
      case TokenT::REG_B:  return std::make_shared<StatementNoArgCommand>(0xED41, cmd);
      case TokenT::REG_C:  return std::make_shared<StatementNoArgCommand>(0xED49, cmd);
      case TokenT::REG_D:  return std::make_shared<StatementNoArgCommand>(0xED51, cmd);
      case TokenT::REG_E:  return std::make_shared<StatementNoArgCommand>(0xED59, cmd);
      case TokenT::REG_H:  return std::make_shared<StatementNoArgCommand>(0xED61, cmd);
      case TokenT::REG_L:  return std::make_shared<StatementNoArgCommand>(0xED69, cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_RES>) {
    auto cmd = peekPrev();
    auto expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto stmt = addressRegisterComplexOperation(0xCB86, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return std::make_shared<StatementLambda>(StatementLambda(cmd, expr, [](std::vector<uint32_t> argv) {
        return (uint32_t)(argv[0] > 7 ? 0x00 : 0xCB80 | (argv[0] << 3) | argv[1]);
      }, { Defs::Reg2Mask(peekPrev()->token) }));
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SBC>) {
    auto cmd = peekPrev();

    std::shared_ptr<Token> reg = advance();
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    switch (reg->token) {
      case TokenT::REG_A:
        return basicRegisterOperation(0x009E, 0x0098, 0x00DE, cmd);

      case TokenT::REG_HL:
        switch(advance()->token) {
          case TokenT::REG_BC: return std::make_shared<StatementNoArgCommand>(0xED42, cmd);
          case TokenT::REG_DE: return std::make_shared<StatementNoArgCommand>(0xED52, cmd);
          case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(0xED62, cmd);
          case TokenT::REG_SP: return std::make_shared<StatementNoArgCommand>(0xED72, cmd);
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return std::make_shared<Statement>();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> Process(Int2Type<TokenT::CMD_SET>) {
    auto cmd = peekPrev();
    auto expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto stmt = addressRegisterComplexOperation(0xCBC6, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return std::make_shared<StatementLambda>(StatementLambda(cmd, expr, [](std::vector<uint32_t> argv) {
        return (uint32_t)(argv[0] > 7 ? 0x00 : 0xCBC0 | (argv[0] << 3) | argv[1]);
      }, { Defs::Reg2Mask(peekPrev()->token) }));
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return std::make_shared<Statement>();
  }

private:

  inline std::shared_ptr<Statement> basicRegisterOperation(uint32_t op0, uint32_t op1, uint32_t op2, std::shared_ptr<Token> cmd) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto stmt = addressRegisterOperation(op0, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return std::make_shared<StatementNoArgCommand>(op1 | Defs::Reg2Mask(peekPrev()->token), cmd);
    }

    return std::make_shared<StatementOneArgCommand>(op2, cmd, shift(1));
  }

  inline std::shared_ptr<Statement> basicRegisterOperation(uint32_t op0, uint32_t op1, std::shared_ptr<Token> cmd) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto stmt = addressRegisterComplexOperation(op0, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return std::make_shared<StatementNoArgCommand>(op1 | Defs::Reg2Mask(peekPrev()->token), cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> addressRegisterOperation(uint32_t opcode, std::shared_ptr<Token> cmd) {
    switch (advance()->token) {
      case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>( 0x0000 | opcode, cmd);
      case TokenT::REG_IX: return std::make_shared<StatementOneArgCommand>(0xDD00 | opcode, cmd, offset(1));
      case TokenT::REG_IY: return std::make_shared<StatementOneArgCommand>(0xFD00 | opcode, cmd, offset(1));
    }

    error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
    return std::make_shared<Statement>();
  }

  inline std::shared_ptr<Statement> addressRegisterComplexOperation(uint32_t opcode, std::shared_ptr<Token> cmd) {
    uint32_t code = opcode & 0x00FF;
    uint32_t command = ((opcode & 0xFF00) << 8);

    switch (advance()->token) {
      case TokenT::REG_HL: return std::make_shared<StatementNoArgCommand>(opcode, cmd);
      case TokenT::REG_IX: return std::make_shared<StatementLambda>(StatementLambda(cmd, offset(1), [](std::vector<uint32_t> argv) { return (uint32_t)(0xDD000000 | argv[1] | (argv[0] << 8)); }, { command | code }));
      case TokenT::REG_IY: return std::make_shared<StatementLambda>(StatementLambda(cmd, offset(1), [](std::vector<uint32_t> argv) { return (uint32_t)(0xFD000000 | argv[1] | (argv[0] << 8)); }, { command | code }));
    }

    error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
    return std::make_shared<Statement>();
  }

private:
  inline std::shared_ptr<Token> peek() { return lexer.tokens[nCurr]; }
  inline std::shared_ptr<Token> peekPrev() { return lexer.tokens[nCurr - 1]; }
  inline bool isAtEnd() { return peek()->token == TokenT::OP_EOF; }
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
  Lexer lexer;

  std::vector<std::shared_ptr<Statement>> stmt;
  std::vector<std::string> errors;

private:
  int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};