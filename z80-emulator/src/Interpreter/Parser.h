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
 *  statement   -> COMMAND (expression)?  // NOTE: Grammar for each command will be hardcoded based on manual, this grammar is just a common example
 * 
 *  offset      -> '+' literal
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
  Parser(Lexer& l): lexer(l) {}

  void scan() {
    stmt.clear(); errors.clear();

    if (lexer.scan()) return;
    program();
  }

private:
  inline void program() {
    while (isAtEnd()) stmt.push_back(declaration());
  }

  Statement declaration() {
    if (match<1>({ TokenT::IDENTIFIER })) {
      switch (peek()->token) {
        case TokenT::COLON: return func();
        case TokenT::OP_EQU: return variable();
      }

      error(advance(), "Unknown token.");
      return Statement();
    }

    return statement();
  }

  Statement func() {
    std::shared_ptr<Token> label = peekPrev();
    consume(TokenT::COLON, "Expect ':' before expression.");
    Expression expr = term();
    // TODO:

    return Statement();
  }

  Statement variable() {
    std::shared_ptr<Token> label = peekPrev();
    consume(TokenT::OP_EQU, "Expect 'EQU' before expression.");
    return StatementVariable(label, term());
  }

  Statement statement() {
    AnyType<-1, Parser*>::GetValue() = this;
    AnyType<-1, int32_t>::GetValue() = advance()->token;

    return foreach<CommandList, AnyType<-1, Parser*>>::Key2Process();
  }

  inline Expression term() {
    Expression expr = unary();

    while (match<5>({ TokenT::BIT_AND, TokenT::BIT_OR, TokenT::BIT_XOR, TokenT::PLUS, TokenT::MINUS })) {
      std::shared_ptr<Token> operation = peekPrev();
      Expression right = unary();
      expr = ExpressionBinary(expr, operation, right);
    }

    return expr;
  }

  inline Expression unary() {
    if (match<2>({ TokenT::MINUS, TokenT::BIT_NOT })) {
      std::shared_ptr<Token> operation = peekPrev();
      return ExpressionUnary(operation, unary());
    }

    return literal();
  }

  inline Expression offset(int32_t size = 0) {
    consume(TokenT::PLUS, "Expect '+' before expression.");
    return literal(size);
  }

  Expression literal(int32_t size = 0) {
    if (match<2>({ TokenT::NUMBER, TokenT::STRING })) {
      return ExpressionLiteral(peekPrev(), size);
    }

    if (match<1>({ TokenT::IDENTIFIER })) {
    // TODO:
    //   return new ConditionVariableExpression(this.peekPrev());
    }

    error(advance(), "Unknown token.");
  }

public:
  template<int32_t T>
  Statement Process(Int2Type<T>) {
    error(peekPrev(), "Unknown operation");
    return Statement();
  }

  // NOTE: No Arg Command
  inline Statement Process(Int2Type<TokenT::CMD_CCF>)  { return StatementNoArgCommand(0x003F, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_CPD>)  { return StatementNoArgCommand(0xEDA9, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_CPDR>) { return StatementNoArgCommand(0xEDB9, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_CPI>)  { return StatementNoArgCommand(0xEDA1, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_CPIR>) { return StatementNoArgCommand(0xEDB1, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_CPL>)  { return StatementNoArgCommand(0x002F, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_DAA>)  { return StatementNoArgCommand(0x0027, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_DI>)   { return StatementNoArgCommand(0x00F3, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_EI>)   { return StatementNoArgCommand(0x00FB, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_EXX>)  { return StatementNoArgCommand(0x00D9, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_HALT>) { return StatementNoArgCommand(0x0076, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_IND>)  { return StatementNoArgCommand(0xEDAA, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_INDR>) { return StatementNoArgCommand(0xEDBA, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_INI>)  { return StatementNoArgCommand(0xEDA2, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_INIR>) { return StatementNoArgCommand(0xEDB2, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_LDD>)  { return StatementNoArgCommand(0xEDA8, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_LDDR>) { return StatementNoArgCommand(0xEDB8, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_LDI>)  { return StatementNoArgCommand(0xEDA0, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_LDIR>) { return StatementNoArgCommand(0xEDB0, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_NEG>)  { return StatementNoArgCommand(0xED44, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_NOP>)  { return StatementNoArgCommand(0x0000, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_OTDR>) { return StatementNoArgCommand(0xEDBB, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_OTIR>) { return StatementNoArgCommand(0xEDB3, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_OUTD>) { return StatementNoArgCommand(0xEDAB, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_OUTI>) { return StatementNoArgCommand(0xEDA3, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RETI>) { return StatementNoArgCommand(0xED4D, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RETN>) { return StatementNoArgCommand(0xED45, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RLA>)  { return StatementNoArgCommand(0x0017, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RLCA>) { return StatementNoArgCommand(0x0007, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RLD>)  { return StatementNoArgCommand(0xED6F, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RRA>)  { return StatementNoArgCommand(0x001F, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RRCA>) { return StatementNoArgCommand(0x000F, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_RRD>)  { return StatementNoArgCommand(0xED67, peekPrev()); }
  inline Statement Process(Int2Type<TokenT::CMD_SCF>)  { return StatementNoArgCommand(0x0037, peekPrev()); }


  // NOTE: One arg command
  inline Statement Process(Int2Type<TokenT::CMD_RL>)  { return basicRegisterOperation<2>({ 0xCB16, 0xCB10 }); }
  inline Statement Process(Int2Type<TokenT::CMD_RLC>) { return basicRegisterOperation<2>({ 0xCB06, 0xCB00 }); }
  inline Statement Process(Int2Type<TokenT::CMD_RR>)  { return basicRegisterOperation<2>({ 0xCB1E, 0xCB18 }); }
  inline Statement Process(Int2Type<TokenT::CMD_RRC>) { return basicRegisterOperation<2>({ 0xCB0E, 0xCB08 }); }
  inline Statement Process(Int2Type<TokenT::CMD_SLA>) { return basicRegisterOperation<2>({ 0xCB26, 0xCB20 }); }
  inline Statement Process(Int2Type<TokenT::CMD_SRA>) { return basicRegisterOperation<2>({ 0xCB2E, 0xCB28 }); }
  inline Statement Process(Int2Type<TokenT::CMD_SRL>) { return basicRegisterOperation<2>({ 0xCB3E, 0xCB38 }); }

  inline Statement Process(Int2Type<TokenT::CMD_AND>) { return basicRegisterOperation<3>({ 0x00A6, 0x00A0, 0x00E6 }); }
  inline Statement Process(Int2Type<TokenT::CMD_CP>)  { return basicRegisterOperation<3>({ 0x00BE, 0x00B8, 0x00FE }); }
  inline Statement Process(Int2Type<TokenT::CMD_OR>)  { return basicRegisterOperation<3>({ 0x00B6, 0x00B0, 0x00F6 }); }
  inline Statement Process(Int2Type<TokenT::CMD_SUB>) { return basicRegisterOperation<3>({ 0x0096, 0x0090, 0x00D6 }); }
  inline Statement Process(Int2Type<TokenT::CMD_XOR>) { return basicRegisterOperation<3>({ 0x00AE, 0x00A8, 0x00EE }); }

  inline Statement Process(Int2Type<TokenT::CMD_DEC>) {
    std::shared_ptr<Token> cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0035, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    switch(advance()->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0x003D, cmd);
      case TokenT::REG_B:  return StatementNoArgCommand(0x0005, cmd);
      case TokenT::REG_BC: return StatementNoArgCommand(0x000B, cmd);
      case TokenT::REG_C:  return StatementNoArgCommand(0x000D, cmd);
      case TokenT::REG_D:  return StatementNoArgCommand(0x0015, cmd);
      case TokenT::REG_DE: return StatementNoArgCommand(0x001B, cmd);
      case TokenT::REG_E:  return StatementNoArgCommand(0x001D, cmd);
      case TokenT::REG_H:  return StatementNoArgCommand(0x0025, cmd);
      case TokenT::REG_HL: return StatementNoArgCommand(0x002B, cmd);
      case TokenT::REG_IX: return StatementNoArgCommand(0xDD2B, cmd);
      case TokenT::REG_IY: return StatementNoArgCommand(0xFD2B, cmd);
      case TokenT::REG_L:  return StatementNoArgCommand(0x002D, cmd);
      case TokenT::REG_SP: return StatementNoArgCommand(0x003B, cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_DJNZ>) {
    return StatementOneArgCommand(0x0010, peekPrev(), literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_IM>) {
    return StatementLambda(literal(1), peekPrev(), [](std::vector<uint32_t>& argv) {
      switch (argv[0]) {
        case 0x00: return 0xED46;
        case 0x01: return 0xED56;
        case 0x02: return 0xED5E;
      }
      return 0x00;
    });
  }

  inline Statement Process(Int2Type<TokenT::CMD_INC>) {
    std::shared_ptr<Token> cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0034, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    switch(advance()->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0x003C, cmd);
      case TokenT::REG_B:  return StatementNoArgCommand(0x0004, cmd);
      case TokenT::REG_BC: return StatementNoArgCommand(0x0003, cmd);
      case TokenT::REG_C:  return StatementNoArgCommand(0x000C, cmd);
      case TokenT::REG_D:  return StatementNoArgCommand(0x0014, cmd);
      case TokenT::REG_DE: return StatementNoArgCommand(0x0013, cmd);
      case TokenT::REG_E:  return StatementNoArgCommand(0x001C, cmd);
      case TokenT::REG_H:  return StatementNoArgCommand(0x0024, cmd);
      case TokenT::REG_HL: return StatementNoArgCommand(0x0023, cmd);
      case TokenT::REG_IX: return StatementNoArgCommand(0xDD23, cmd);
      case TokenT::REG_IY: return StatementNoArgCommand(0xFD23, cmd);
      case TokenT::REG_L:  return StatementNoArgCommand(0x002C, cmd);
      case TokenT::REG_SP: return StatementNoArgCommand(0x0033, cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_POP>) {
    switch(advance()->token) {
      case TokenT::REG_AF: return StatementNoArgCommand(0x00F1, peekPrev());
      case TokenT::REG_BC: return StatementNoArgCommand(0x00C1, peekPrev());
      case TokenT::REG_DE: return StatementNoArgCommand(0x00D1, peekPrev());
      case TokenT::REG_HL: return StatementNoArgCommand(0x00E1, peekPrev());
      case TokenT::REG_IX: return StatementNoArgCommand(0xDDE1, peekPrev());
      case TokenT::REG_IY: return StatementNoArgCommand(0xFDE1, peekPrev());
    }

    error(peekPrev(), "Expect register to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_PUSH>) {
    switch(advance()->token) {
      case TokenT::REG_AF: return StatementNoArgCommand(0x00F5, peekPrev());
      case TokenT::REG_BC: return StatementNoArgCommand(0x00C5, peekPrev());
      case TokenT::REG_DE: return StatementNoArgCommand(0x00D5, peekPrev());
      case TokenT::REG_HL: return StatementNoArgCommand(0x00E5, peekPrev());
      case TokenT::REG_IX: return StatementNoArgCommand(0xDDE5, peekPrev());
      case TokenT::REG_IY: return StatementNoArgCommand(0xFDE5, peekPrev());
    }

    error(peekPrev(), "Expect register to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RET>) {
    std::shared_ptr<Token> cmd = peekPrev();

    switch(peek()->token) {
      case TokenT::FLAG_C:  advance(); return StatementNoArgCommand(0x00D8, cmd);
      case TokenT::FLAG_M:  advance(); return StatementNoArgCommand(0x00F8, cmd);
      case TokenT::FLAG_NC: advance(); return StatementNoArgCommand(0x00D0, cmd);
      case TokenT::FLAG_NZ: advance(); return StatementNoArgCommand(0x00C0, cmd);
      case TokenT::FLAG_P:  advance(); return StatementNoArgCommand(0x00F0, cmd);
      case TokenT::FLAG_PE: advance(); return StatementNoArgCommand(0x00E8, cmd);
      case TokenT::FLAG_PO: advance(); return StatementNoArgCommand(0x00E0, cmd);
      case TokenT::FLAG_Z:  advance(); return StatementNoArgCommand(0x00C8, cmd);
    }

    return StatementNoArgCommand(0x00C9, cmd);
  }

  inline Statement Process(Int2Type<TokenT::CMD_RST>) {
    return StatementLambda(literal(1), peekPrev(), [](std::vector<uint32_t>& argv) {
      switch (argv[0]) {
        case 0x00: return 0x00C7;
        case 0x08: return 0x00CF;
        case 0x10: return 0x00D7;
        case 0x18: return 0x00DF;
        case 0x20: return 0x00E7;
        case 0x28: return 0x00EF;
        case 0x30: return 0x00F7;
        case 0x38: return 0x00FF;
      }
      return 0x00;
    });
  }



  // NOTE: Two arg command
  inline Statement Process(Int2Type<TokenT::CMD_ADC>) {
    switch (advance()->token) {
      case TokenT::REG_A:
        consume(TokenT::COMMA, "Expect ',' after first expression.");
        return basicRegisterOperation<3>({ 0x008E, 0x0088, 0x00CE });

      case TokenT::REG_HL:
        switch(advance()->token) {
          case TokenT::REG_BC: return StatementNoArgCommand(0xED4A, peekPrev());
          case TokenT::REG_DE: return StatementNoArgCommand(0xED5A, peekPrev());
          case TokenT::REG_HL: return StatementNoArgCommand(0xED6A, peekPrev());
          case TokenT::REG_SP: return StatementNoArgCommand(0xED7A, peekPrev());
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return Statement();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_ADD>) {
    uint32_t mask = 0x0000;

    switch (advance()->token) {
      case TokenT::REG_A:
        consume(TokenT::COMMA, "Expect ',' after first expression.");
        return basicRegisterOperation<3>({ 0x0086, 0x0080, 0x00C6 });

      case TokenT::REG_IX:
      case TokenT::REG_IY:
        mask = peekPrev()->token == TokenT::REG_IX ? 0xDD00 : 0xFD00;

      case TokenT::REG_HL:
        switch(advance()->token) {
          case TokenT::REG_BC: return StatementNoArgCommand(0x0009 | mask, peekPrev());
          case TokenT::REG_DE: return StatementNoArgCommand(0x0019 | mask, peekPrev());
          case TokenT::REG_HL: return StatementNoArgCommand(0x0029 | mask, peekPrev());
          case TokenT::REG_SP: return StatementNoArgCommand(0x0039 | mask, peekPrev());
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return Statement();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL' | 'IX' | 'IY'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_BIT>) {
    std::shared_ptr<Token> cmd = peekPrev();

    Expression expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: stmt = StatementLambda(expr, cmd, [](std::vector<uint32_t>& argv) { return argv[0] > 7 ? 0x00 : 0x00CB46 | (argv[0] << 3); }); break;
        case TokenT::REG_IX: stmt = StatementLambda({ expr, offset(1) }, cmd, [](std::vector<uint32_t>& argv) { return argv[0] > 7 ? 0x00 : 0xDDCB0046 | (argv[1] << 8) | (argv[0] << 3); });
        case TokenT::REG_IY: stmt = StatementLambda({ expr, offset(1) }, cmd, [](std::vector<uint32_t>& argv) { return argv[0] > 7 ? 0x00 : 0xFDCB0046 | (argv[1] << 8) | (argv[0] << 3); });
        default: error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementLambda(expr, cmd, [](std::vector<uint32_t>& argv) {
        return argv[0] > 7 ? 0x00 : 0xCB40 | (argv[0] << 3) | argv[1];
      }, { Defs::Reg2Mask(peekPrev()->token) });
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_CALL>) { 
    std::shared_ptr<Token> cmd = peekPrev();

    if (!match<8>({ TokenT::FLAG_C, TokenT::FLAG_M, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_P, TokenT::FLAG_PE, TokenT::FLAG_PO, TokenT::FLAG_Z })) {
      return StatementOneArgCommand(0x00CD, cmd, literal(2));
    }

    consume(TokenT::COMMA, "Expect ',' after first expression.");
    switch(peekPrev()->token) {
      case TokenT::FLAG_C:  return StatementOneArgCommand(0x00DC, cmd, literal(2));
      case TokenT::FLAG_M:  return StatementOneArgCommand(0x00FC, cmd, literal(2));
      case TokenT::FLAG_NC: return StatementOneArgCommand(0x00D4, cmd, literal(2));
      case TokenT::FLAG_NZ: return StatementOneArgCommand(0x00C4, cmd, literal(2));
      case TokenT::FLAG_P:  return StatementOneArgCommand(0x00F4, cmd, literal(2));
      case TokenT::FLAG_PE: return StatementOneArgCommand(0x00EC, cmd, literal(2));
      case TokenT::FLAG_PO: return StatementOneArgCommand(0x00E4, cmd, literal(2));
      case TokenT::FLAG_Z:  return StatementOneArgCommand(0x00CC, cmd, literal(2));
    }
  }

  inline Statement Process(Int2Type<TokenT::CMD_EX>) {
    std::shared_ptr<Token> cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      consume(TokenT::REG_SP, "Expect register to be 'SP'");
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      return addressRegisterOperation(0x00E3, cmd);
    }

    if (match<1>({ TokenT::REG_AF })) {
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      consume(TokenT::REG_AF$, "Expect register to be AF'");
      return StatementNoArgCommand(0x0008, cmd);
    }

    if (match<1>({ TokenT::REG_DE })) {
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      consume(TokenT::REG_HL, "Expect register to be 'HL'");
      return StatementNoArgCommand(0x00EB, cmd);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'DE' | '(SP)'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_IN>) {
    std::shared_ptr<Token> cmd = peekPrev();

    if (!match<8>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L, TokenT::REG_F })) {
      error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L' | 'F'");
      return Statement();
    }

    std::shared_ptr<Token> reg = peekPrev();
    consume(TokenT::LEFT_BRACE, "Expect '(' after expression.");
    if (reg->token == TokenT::REG_A && !check(TokenT::REG_C)) {
      Statement stmt = StatementOneArgCommand(0x00DB, cmd, literal(1));
      consume(TokenT::LEFT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    consume(TokenT::REG_C, "Expect register to be 'C'.");
    consume(TokenT::LEFT_BRACE, "Expect ')' after expression.");

    switch (reg->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0xED78, cmd);
      case TokenT::REG_B:  return StatementNoArgCommand(0xED40, cmd);
      case TokenT::REG_C:  return StatementNoArgCommand(0xED48, cmd);
      case TokenT::REG_D:  return StatementNoArgCommand(0xED50, cmd);
      case TokenT::REG_E:  return StatementNoArgCommand(0xED58, cmd);
      case TokenT::REG_H:  return StatementNoArgCommand(0xED60, cmd);
      case TokenT::REG_L:  return StatementNoArgCommand(0xED68, cmd);
      case TokenT::REG_F:  return StatementNoArgCommand(0xED70, cmd);
    }
  }

  inline Statement Process(Int2Type<TokenT::CMD_JP>) { 
    std::shared_ptr<Token> cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;

      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand(0x00E9, peekPrev()); break;
        case TokenT::REG_IX: StatementNoArgCommand(0xDDE9, peekPrev()); break;
        case TokenT::REG_IY: StatementNoArgCommand(0xFDE9, peekPrev()); break;
        default: error(peekPrev(), "Expect register to be 'HL' | 'IY' | 'IX'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (!match<8>({ TokenT::FLAG_C, TokenT::FLAG_M, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_P, TokenT::FLAG_PE, TokenT::FLAG_PO, TokenT::FLAG_Z })) {
      error(peekPrev(), "Expect flag to be 'C' | 'M' | 'NC' | 'NZ' | 'P' | 'PE' | 'PO' | 'Z'");
      return Statement();
    }

    consume(TokenT::COMMA, "Expect ',' after first expression.");
    switch(peekPrev()->token) {
      case TokenT::FLAG_C:  return StatementOneArgCommand(0x00DA, cmd, literal(2));
      case TokenT::FLAG_M:  return StatementOneArgCommand(0x00FA, cmd, literal(2));
      case TokenT::FLAG_NC: return StatementOneArgCommand(0x00D2, cmd, literal(2));
      case TokenT::FLAG_NZ: return StatementOneArgCommand(0x00C2, cmd, literal(2));
      case TokenT::FLAG_P:  return StatementOneArgCommand(0x00F2, cmd, literal(2));
      case TokenT::FLAG_PE: return StatementOneArgCommand(0x00EA, cmd, literal(2));
      case TokenT::FLAG_PO: return StatementOneArgCommand(0x00E2, cmd, literal(2));
      case TokenT::FLAG_Z:  return StatementOneArgCommand(0x00CA, cmd, literal(2));
    }
  }

  inline Statement Process(Int2Type<TokenT::CMD_JR>) {
    std::shared_ptr<Token> cmd = peekPrev();

    if (!match<4>({ TokenT::FLAG_C, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_Z })) {
      return StatementOneArgCommand(0x0018, cmd, literal(2));
    }

    switch(peekPrev()->token) {
      case TokenT::FLAG_C:  return StatementOneArgCommand(0x0038, cmd, literal(1));
      case TokenT::FLAG_NC: return StatementOneArgCommand(0x0030, cmd, literal(1));
      case TokenT::FLAG_NZ: return StatementOneArgCommand(0x0020, cmd, literal(1));
      case TokenT::FLAG_Z:  return StatementOneArgCommand(0x0028, cmd, literal(1));
    }
  }

  inline Statement Process(Int2Type<TokenT::CMD_LD>) { 
    std::shared_ptr<Token> cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      switch (peek()->token) {
        case TokenT::REG_BC:
        case TokenT::REG_DE: {
          std::shared_ptr<Token> reg = advance();
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");
          consume(TokenT::REG_A, "Expect register 'A' after expression.");

          switch(reg->token) {
            case TokenT::REG_BC: return StatementNoArgCommand(0x0002, cmd);
            case TokenT::REG_DE: return StatementNoArgCommand(0x0012, cmd);
          }

          return Statement();
        }

        case TokenT::REG_HL:
          consume(TokenT::REG_HL, "Expect 'HL' after expression.");
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return StatementNoArgCommand(0x70 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
          }

          return StatementOneArgCommand(0x36, peekPrev(), literal(1));

        case TokenT::REG_IX: {
          consume(TokenT::REG_IX, "Expect 'IX' after expression.");
          Expression expr = offset(1);

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return StatementLambda(expr, cmd, [](std::vector<uint32_t>& argv) { return 0xDD7000 | (argv[1] << 8) | argv[0]; }, { Defs::Reg2Mask(peekPrev()->token)});
          }

          return StatementLambda({ expr, literal(1) }, cmd, [](std::vector<uint32_t>& argv) { return 0xDD360000 | (argv[0] << 8) | argv[1]; }, { Defs::Reg2Mask(peekPrev()->token)});
        }

        case TokenT::REG_IY: {
          consume(TokenT::REG_IY, "Expect 'IY' after expression.");
          Expression expr = offset(1);

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return StatementLambda(expr, cmd, [](std::vector<uint32_t>& argv) { return 0xFD7000 | (argv[1] << 8) | argv[0]; }, { Defs::Reg2Mask(peekPrev()->token)});
          }

          return StatementLambda({ expr, literal(1) }, cmd, [](std::vector<uint32_t>& argv) { return 0xFD360000 | (argv[0] << 8) | argv[1]; }, { Defs::Reg2Mask(peekPrev()->token)});
        }
      }


      Expression expr = literal(2);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after expression.");

      switch (advance()->token) {
        case TokenT::REG_A:  return StatementOneArgCommand(0x0032, cmd, expr);
        case TokenT::REG_BC: return StatementOneArgCommand(0xED43, cmd, expr);
        case TokenT::REG_DE: return StatementOneArgCommand(0xED53, cmd, expr);
        case TokenT::REG_HL: return StatementOneArgCommand(0x0022, cmd, expr);
        case TokenT::REG_IX: return StatementOneArgCommand(0xDD22, cmd, expr);
        case TokenT::REG_IY: return StatementOneArgCommand(0xFD22, cmd, expr);
        case TokenT::REG_SP: return StatementOneArgCommand(0xED73, cmd, expr);
      }


      error(peekPrev(), "Expect register to be 'A' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY' | 'SP'");
      return Statement();
    }

    // TODO: ...
    // LD A,(BC)
    // LD A,(DE)
    // LD A,(HL)
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_OUT>) {
    std::shared_ptr<Token> cmd = peekPrev();
    consume(TokenT::LEFT_BRACE, "Expect '(' after expression.");

    if (!match<1>({ TokenT::REG_C })) {
      Expression expr = literal(1);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after expression.");
      consume(TokenT::REG_A, "Expect register to be 'A'");
      return StatementOneArgCommand(0x00D3, cmd, expr);
    }

    consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
    consume(TokenT::COMMA, "Expect ',' after expression.");
    switch (advance()->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0xED79, cmd);
      case TokenT::REG_B:  return StatementNoArgCommand(0xED41, cmd);
      case TokenT::REG_C:  return StatementNoArgCommand(0xED49, cmd);
      case TokenT::REG_D:  return StatementNoArgCommand(0xED51, cmd);
      case TokenT::REG_E:  return StatementNoArgCommand(0xED59, cmd);
      case TokenT::REG_H:  return StatementNoArgCommand(0xED61, cmd);
      case TokenT::REG_L:  return StatementNoArgCommand(0xED69, cmd);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RES>) {
    std::shared_ptr<Token> cmd = peekPrev();

    Expression expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterComplexOperation(0xCB86, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementLambda(expr, cmd, [](std::vector<uint32_t>& argv) {
        return argv[0] > 7 ? 0x00 : 0xCB80 | (argv[0] << 3) | argv[1];
      }, { Defs::Reg2Mask(peekPrev()->token) });
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SBC>) {
    switch (advance()->token) {
      case TokenT::REG_A:
        consume(TokenT::COMMA, "Expect ',' after first expression.");
        return basicRegisterOperation<3>({ 0x009E, 0x0098, 0x00DE });

      case TokenT::REG_HL:
        switch(advance()->token) {
          case TokenT::REG_BC: return StatementNoArgCommand(0xED42, peekPrev());
          case TokenT::REG_DE: return StatementNoArgCommand(0xED52, peekPrev());
          case TokenT::REG_HL: return StatementNoArgCommand(0xED62, peekPrev());
          case TokenT::REG_SP: return StatementNoArgCommand(0xED72, peekPrev());
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return Statement();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SET>) {
    std::shared_ptr<Token> cmd = peekPrev();

    Expression expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterComplexOperation(0xCBC6, cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementLambda(expr, cmd, [](std::vector<uint32_t>& argv) {
        return argv[0] > 7 ? 0x00 : 0xCBC0 | (argv[0] << 3) | argv[1];
      }, { Defs::Reg2Mask(peekPrev()->token) });
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

private:

  template<int32_t T>
  inline Statement basicRegisterOperation(std::array<uint32_t, T> opcode) { return Statement(); }

  inline Statement basicRegisterOperation(std::array<uint32_t, 3> opcode) {
    std::shared_ptr<Token> cmd = peekPrev();

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(opcode[0], cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(opcode[1] | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(opcode[2], peekPrev(), literal(1));
  }

  inline Statement basicRegisterOperation(std::array<uint32_t, 2> opcode) {
    std::shared_ptr<Token> cmd = peekPrev();
    
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterComplexOperation(opcode[0], cmd);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(opcode[1] | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement addressRegisterOperation(uint32_t opcode, std::shared_ptr<Token> cmd = nullptr) {
    switch (advance()->token) {
      case TokenT::REG_HL: return StatementNoArgCommand( 0x0000 | opcode, cmd == nullptr ? peekPrev() : cmd);
      case TokenT::REG_IX: return StatementOneArgCommand(0xDD00 | opcode, cmd == nullptr ? peekPrev() : cmd, offset(1));
      case TokenT::REG_IY: return StatementOneArgCommand(0xFD00 | opcode, cmd == nullptr ? peekPrev() : cmd, offset(1));
    }

    error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
    return Statement();
  }

  inline Statement addressRegisterComplexOperation(uint32_t opcode, std::shared_ptr<Token> cmd = nullptr) {
    uint32_t code = opcode & 0x00FF;
    uint32_t command = ((opcode & 0xFF00) << 8);

    switch (advance()->token) {
      case TokenT::REG_HL: StatementNoArgCommand(opcode, cmd); break;
      case TokenT::REG_IX: StatementLambda(offset(1), cmd, [](std::vector<uint32_t>& argv) { return 0xDD000000 | argv[1] | (argv[0] << 8); }, { command | code });
      case TokenT::REG_IY: StatementLambda(offset(1), cmd, [](std::vector<uint32_t>& argv) { return 0xFD000000 | argv[1] | (argv[0] << 8); }, { command | code });
    }

    error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
    return Statement();
  }

private:
  inline std::shared_ptr<Token> peek() { return lexer.vTokens[nCurr]; }
  inline std::shared_ptr<Token> peekPrev() { return lexer.vTokens[nCurr - 1]; }
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
    error(peek(), message);
  }

  void error(std::shared_ptr<Token> token, std::string message) {
    std::stringstream ss;
    ss << "[Ln " << token->line << " Col " << token->col << "] Error at '" << token->lexeme << "': " << message << "\n";
    errors.push_back(ss.str());
  }


public:
  std::vector<Statement> stmt;
  std::vector<std::string> errors;

private:
  Lexer& lexer;
  int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};