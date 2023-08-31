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
  Parser(Lexer l): lexer(l) {}

  void scan();

private:
  void program() {}
  void declaration() {
    if (match<1>({ TokenT::IDENTIFIER })) {
      std::shared_ptr<Token> label = peekPrev();
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
    AnyType<-1, int32_t>::GetValue() = advance()->token;

    Statement stmt = foreach<CommandList, AnyType<-1, Parser*>>::Key2Process();
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
    error(peekPrev(), "Unknown command");
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
  inline Statement Process(Int2Type<TokenT::CMD_AND>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x00A6);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0x00A0 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(0x00E6, peekPrev(), literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_CP>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x00BE);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0x00B8 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(0x00FE, peekPrev(), literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_DEC>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0035);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    std::shared_ptr<Token> cmd = peekPrev();

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

    error(peekPrev(), "Expect to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_DJNZ>) {
    return StatementOneArgCommand(0x0010, peekPrev(), literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_IM>) {
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_INC>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0034);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    std::shared_ptr<Token> cmd = peekPrev();

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

    error(peekPrev(), "Expect to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_OR>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x00B6);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0x00B0 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(0x00F6, peekPrev(), literal(1));
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

    error(peekPrev(), "Expect to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
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

    error(peekPrev(), "Expect to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
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

  inline Statement Process(Int2Type<TokenT::CMD_RL>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB16, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("22", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("22", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB10 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RLC>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB06, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("6", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("6", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB00 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RR>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB1E, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("30", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("30", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB18 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RRC>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB0E, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("14", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("14", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB08 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RST>) {
    // TODO: 
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SLA>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB26, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("38", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("38", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB20 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SRA>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB2E, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("46", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("46", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB28 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SRL>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand( 0xCB3E, peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("62", TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFDCB, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral("62", TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0xCB38 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SUB>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0096);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0x0090 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(0x00D6, peekPrev(), literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_XOR>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x00AE);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(0x00A8 | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(0x00EE, peekPrev(), literal(1));
  }

  // NOTE: Two arg command

private:
  inline Statement addressRegisterOperation(uint16_t opcode) {
    switch (advance()->token) {
      case TokenT::REG_HL: return StatementNoArgCommand( 0x0000 | opcode, peekPrev());
      case TokenT::REG_IX: return StatementOneArgCommand(0xDD00 | opcode, peekPrev(), offset(1));
      case TokenT::REG_IY: return StatementOneArgCommand(0xFD00 | opcode, peekPrev(), offset(1));
    }

    error(peekPrev(), "Expect to be 'HL' | 'IY+o' | 'IX+o'");
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
    // TODO:
    // throw error(peek(), message);
  }

   void error(std::shared_ptr<Token> token, std::string message) {
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