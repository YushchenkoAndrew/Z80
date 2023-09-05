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
  Parser(): lexer(Lexer()) {}
  Parser(Lexer l): lexer(l) {}

  ~Parser() { reset(); }

  bool scan(std::string src) {
    reset();

    if (lexer.scan(src)) { errors.push_back(lexer.error()); return true; }
    program();
    return errors.size();
  }

private:
  inline void reset() { nCurr = 0; stmt.clear(); errors.clear(); }

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
    return StatementVariable(label);
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

  inline Expression term(int32_t size = 0) {
    Expression expr = unary(size);

    while (match<5>({ TokenT::BIT_AND, TokenT::BIT_OR, TokenT::BIT_XOR, TokenT::PLUS, TokenT::MINUS })) {
      std::shared_ptr<Token> operation = peekPrev();
      Expression right = unary(size);
      expr = ExpressionBinary(expr, operation, right);
    }

    return expr;
  }

  inline Expression unary(int32_t size = 0) {
    if (match<2>({ TokenT::MINUS, TokenT::BIT_NOT })) {
      std::shared_ptr<Token> operation = peekPrev();
      return ExpressionUnary(operation, unary(size));
    }

    return literal(size);
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
      return ExpressionVariable(peekPrev());
    }

    error(advance(), "Unknown token.");
    return Expression();
  }

public:
  template<int32_t T>
  Statement Process(Int2Type<T>) {
    error(peekPrev(), "Unknown operation");
    return Statement();
  }


  inline Statement Process(Int2Type<TokenT::OP_ORG>) { return StatementAddress(peekPrev(), literal(2)); }
  inline Statement Process(Int2Type<TokenT::OP_DB>) {
    std::vector<Expression> data;

    do {
      data.push_back(term());
    } while(match<1>({ TokenT::COMMA }));

    return StatementAllocate(data);
  }

  // NOTE: No Arg Command
  inline Statement Process(Int2Type<TokenT::CMD_CCF>)  { return StatementNoArgCommand(0x003F); }
  inline Statement Process(Int2Type<TokenT::CMD_CPD>)  { return StatementNoArgCommand(0xEDA9); }
  inline Statement Process(Int2Type<TokenT::CMD_CPDR>) { return StatementNoArgCommand(0xEDB9); }
  inline Statement Process(Int2Type<TokenT::CMD_CPI>)  { return StatementNoArgCommand(0xEDA1); }
  inline Statement Process(Int2Type<TokenT::CMD_CPIR>) { return StatementNoArgCommand(0xEDB1); }
  inline Statement Process(Int2Type<TokenT::CMD_CPL>)  { return StatementNoArgCommand(0x002F); }
  inline Statement Process(Int2Type<TokenT::CMD_DAA>)  { return StatementNoArgCommand(0x0027); }
  inline Statement Process(Int2Type<TokenT::CMD_DI>)   { return StatementNoArgCommand(0x00F3); }
  inline Statement Process(Int2Type<TokenT::CMD_EI>)   { return StatementNoArgCommand(0x00FB); }
  inline Statement Process(Int2Type<TokenT::CMD_EXX>)  { return StatementNoArgCommand(0x00D9); }
  inline Statement Process(Int2Type<TokenT::CMD_HALT>) { return StatementNoArgCommand(0x0076); }
  inline Statement Process(Int2Type<TokenT::CMD_IND>)  { return StatementNoArgCommand(0xEDAA); }
  inline Statement Process(Int2Type<TokenT::CMD_INDR>) { return StatementNoArgCommand(0xEDBA); }
  inline Statement Process(Int2Type<TokenT::CMD_INI>)  { return StatementNoArgCommand(0xEDA2); }
  inline Statement Process(Int2Type<TokenT::CMD_INIR>) { return StatementNoArgCommand(0xEDB2); }
  inline Statement Process(Int2Type<TokenT::CMD_LDD>)  { return StatementNoArgCommand(0xEDA8); }
  inline Statement Process(Int2Type<TokenT::CMD_LDDR>) { return StatementNoArgCommand(0xEDB8); }
  inline Statement Process(Int2Type<TokenT::CMD_LDI>)  { return StatementNoArgCommand(0xEDA0); }
  inline Statement Process(Int2Type<TokenT::CMD_LDIR>) { return StatementNoArgCommand(0xEDB0); }
  inline Statement Process(Int2Type<TokenT::CMD_NEG>)  { return StatementNoArgCommand(0xED44); }
  inline Statement Process(Int2Type<TokenT::CMD_NOP>)  { return StatementNoArgCommand(0x0000); }
  inline Statement Process(Int2Type<TokenT::CMD_OTDR>) { return StatementNoArgCommand(0xEDBB); }
  inline Statement Process(Int2Type<TokenT::CMD_OTIR>) { return StatementNoArgCommand(0xEDB3); }
  inline Statement Process(Int2Type<TokenT::CMD_OUTD>) { return StatementNoArgCommand(0xEDAB); }
  inline Statement Process(Int2Type<TokenT::CMD_OUTI>) { return StatementNoArgCommand(0xEDA3); }
  inline Statement Process(Int2Type<TokenT::CMD_RETI>) { return StatementNoArgCommand(0xED4D); }
  inline Statement Process(Int2Type<TokenT::CMD_RETN>) { return StatementNoArgCommand(0xED45); }
  inline Statement Process(Int2Type<TokenT::CMD_RLA>)  { return StatementNoArgCommand(0x0017); }
  inline Statement Process(Int2Type<TokenT::CMD_RLCA>) { return StatementNoArgCommand(0x0007); }
  inline Statement Process(Int2Type<TokenT::CMD_RLD>)  { return StatementNoArgCommand(0xED6F); }
  inline Statement Process(Int2Type<TokenT::CMD_RRA>)  { return StatementNoArgCommand(0x001F); }
  inline Statement Process(Int2Type<TokenT::CMD_RRCA>) { return StatementNoArgCommand(0x000F); }
  inline Statement Process(Int2Type<TokenT::CMD_RRD>)  { return StatementNoArgCommand(0xED67); }
  inline Statement Process(Int2Type<TokenT::CMD_SCF>)  { return StatementNoArgCommand(0x0037); }


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
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0035);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    switch(advance()->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0x003D);
      case TokenT::REG_B:  return StatementNoArgCommand(0x0005);
      case TokenT::REG_BC: return StatementNoArgCommand(0x000B);
      case TokenT::REG_C:  return StatementNoArgCommand(0x000D);
      case TokenT::REG_D:  return StatementNoArgCommand(0x0015);
      case TokenT::REG_DE: return StatementNoArgCommand(0x001B);
      case TokenT::REG_E:  return StatementNoArgCommand(0x001D);
      case TokenT::REG_H:  return StatementNoArgCommand(0x0025);
      case TokenT::REG_HL: return StatementNoArgCommand(0x002B);
      case TokenT::REG_IX: return StatementNoArgCommand(0xDD2B);
      case TokenT::REG_IY: return StatementNoArgCommand(0xFD2B);
      case TokenT::REG_L:  return StatementNoArgCommand(0x002D);
      case TokenT::REG_SP: return StatementNoArgCommand(0x003B);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_DJNZ>) {
    return StatementOneArgCommand(0x0010, literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_IM>) {
    return StatementLambda(literal(1), [](std::vector<uint32_t>& argv) {
      switch (argv[0]) {
        case 0x00: return 0xED46;
        case 0x01: return 0xED56;
        case 0x02: return 0xED5E;
      }
      return 0x00;
    });
  }

  inline Statement Process(Int2Type<TokenT::CMD_INC>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(0x0034);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    switch(advance()->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0x003C);
      case TokenT::REG_B:  return StatementNoArgCommand(0x0004);
      case TokenT::REG_BC: return StatementNoArgCommand(0x0003);
      case TokenT::REG_C:  return StatementNoArgCommand(0x000C);
      case TokenT::REG_D:  return StatementNoArgCommand(0x0014);
      case TokenT::REG_DE: return StatementNoArgCommand(0x0013);
      case TokenT::REG_E:  return StatementNoArgCommand(0x001C);
      case TokenT::REG_H:  return StatementNoArgCommand(0x0024);
      case TokenT::REG_HL: return StatementNoArgCommand(0x0023);
      case TokenT::REG_IX: return StatementNoArgCommand(0xDD23);
      case TokenT::REG_IY: return StatementNoArgCommand(0xFD23);
      case TokenT::REG_L:  return StatementNoArgCommand(0x002C);
      case TokenT::REG_SP: return StatementNoArgCommand(0x0033);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_POP>) {
    switch(advance()->token) {
      case TokenT::REG_AF: return StatementNoArgCommand(0x00F1);
      case TokenT::REG_BC: return StatementNoArgCommand(0x00C1);
      case TokenT::REG_DE: return StatementNoArgCommand(0x00D1);
      case TokenT::REG_HL: return StatementNoArgCommand(0x00E1);
      case TokenT::REG_IX: return StatementNoArgCommand(0xDDE1);
      case TokenT::REG_IY: return StatementNoArgCommand(0xFDE1);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_PUSH>) {
    switch(advance()->token) {
      case TokenT::REG_AF: return StatementNoArgCommand(0x00F5);
      case TokenT::REG_BC: return StatementNoArgCommand(0x00C5);
      case TokenT::REG_DE: return StatementNoArgCommand(0x00D5);
      case TokenT::REG_HL: return StatementNoArgCommand(0x00E5);
      case TokenT::REG_IX: return StatementNoArgCommand(0xDDE5);
      case TokenT::REG_IY: return StatementNoArgCommand(0xFDE5);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RET>) {
    switch(peek()->token) {
      case TokenT::FLAG_C:  advance(); return StatementNoArgCommand(0x00D8);
      case TokenT::FLAG_M:  advance(); return StatementNoArgCommand(0x00F8);
      case TokenT::FLAG_NC: advance(); return StatementNoArgCommand(0x00D0);
      case TokenT::FLAG_NZ: advance(); return StatementNoArgCommand(0x00C0);
      case TokenT::FLAG_P:  advance(); return StatementNoArgCommand(0x00F0);
      case TokenT::FLAG_PE: advance(); return StatementNoArgCommand(0x00E8);
      case TokenT::FLAG_PO: advance(); return StatementNoArgCommand(0x00E0);
      case TokenT::FLAG_Z:  advance(); return StatementNoArgCommand(0x00C8);
    }

    return StatementNoArgCommand(0x00C9);
  }

  inline Statement Process(Int2Type<TokenT::CMD_RST>) {
    return StatementLambda(literal(1), [](std::vector<uint32_t>& argv) {
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
          case TokenT::REG_BC: return StatementNoArgCommand(0xED4A);
          case TokenT::REG_DE: return StatementNoArgCommand(0xED5A);
          case TokenT::REG_HL: return StatementNoArgCommand(0xED6A);
          case TokenT::REG_SP: return StatementNoArgCommand(0xED7A);
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
          case TokenT::REG_BC: return StatementNoArgCommand(0x0009 | mask);
          case TokenT::REG_DE: return StatementNoArgCommand(0x0019 | mask);
          case TokenT::REG_HL: return StatementNoArgCommand(0x0029 | mask);
          case TokenT::REG_SP: return StatementNoArgCommand(0x0039 | mask);
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return Statement();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL' | 'IX' | 'IY'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_BIT>) {
    Expression expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: stmt = StatementLambda(expr, [](std::vector<uint32_t>& argv) { return argv[0] > 7 ? 0x00 : 0x00CB46 | (argv[0] << 3); }); break;
        case TokenT::REG_IX: stmt = StatementLambda({ expr, offset(1) }, [](std::vector<uint32_t>& argv) { return argv[0] > 7 ? 0x00 : 0xDDCB0046 | (argv[1] << 8) | (argv[0] << 3); }); break;
        case TokenT::REG_IY: stmt = StatementLambda({ expr, offset(1) }, [](std::vector<uint32_t>& argv) { return argv[0] > 7 ? 0x00 : 0xFDCB0046 | (argv[1] << 8) | (argv[0] << 3); }); break;
        default: error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementLambda(expr, [](std::vector<uint32_t>& argv) {
        return argv[0] > 7 ? 0x00 : 0xCB40 | (argv[0] << 3) | argv[1];
      }, { Defs::Reg2Mask(peekPrev()->token) });
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_CALL>) { 
    if (!match<8>({ TokenT::FLAG_C, TokenT::FLAG_M, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_P, TokenT::FLAG_PE, TokenT::FLAG_PO, TokenT::FLAG_Z })) {
      return StatementOneArgCommand(0x00CD, literal(2));
    }

    consume(TokenT::COMMA, "Expect ',' after first expression.");
    switch(peekPrev()->token) {
      case TokenT::FLAG_C:  return StatementOneArgCommand(0x00DC, literal(2));
      case TokenT::FLAG_M:  return StatementOneArgCommand(0x00FC, literal(2));
      case TokenT::FLAG_NC: return StatementOneArgCommand(0x00D4, literal(2));
      case TokenT::FLAG_NZ: return StatementOneArgCommand(0x00C4, literal(2));
      case TokenT::FLAG_P:  return StatementOneArgCommand(0x00F4, literal(2));
      case TokenT::FLAG_PE: return StatementOneArgCommand(0x00EC, literal(2));
      case TokenT::FLAG_PO: return StatementOneArgCommand(0x00E4, literal(2));
      case TokenT::FLAG_Z:  return StatementOneArgCommand(0x00CC, literal(2));
    }

    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_EX>) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      consume(TokenT::REG_SP, "Expect register to be 'SP'");
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      return addressRegisterOperation(0x00E3);
    }

    if (match<1>({ TokenT::REG_AF })) {
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      consume(TokenT::REG_AF$, "Expect register to be AF'");
      return StatementNoArgCommand(0x0008);
    }

    if (match<1>({ TokenT::REG_DE })) {
      consume(TokenT::COMMA, "Expect ',' after first expression.");
      consume(TokenT::REG_HL, "Expect register to be 'HL'");
      return StatementNoArgCommand(0x00EB);
    }

    error(peekPrev(), "Expect register to be 'AF' | 'DE' | '(SP)'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_IN>) {
    if (!match<8>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L, TokenT::REG_F })) {
      error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L' | 'F'");
      return Statement();
    }

    std::shared_ptr<Token> reg = peekPrev();
    consume(TokenT::LEFT_BRACE, "Expect '(' after expression.");
    if (reg->token == TokenT::REG_A && !check(TokenT::REG_C)) {
      Statement stmt = StatementOneArgCommand(0x00DB, literal(1));
      consume(TokenT::LEFT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    consume(TokenT::REG_C, "Expect register to be 'C'.");
    consume(TokenT::LEFT_BRACE, "Expect ')' after expression.");

    switch (reg->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0xED78);
      case TokenT::REG_B:  return StatementNoArgCommand(0xED40);
      case TokenT::REG_C:  return StatementNoArgCommand(0xED48);
      case TokenT::REG_D:  return StatementNoArgCommand(0xED50);
      case TokenT::REG_E:  return StatementNoArgCommand(0xED58);
      case TokenT::REG_H:  return StatementNoArgCommand(0xED60);
      case TokenT::REG_L:  return StatementNoArgCommand(0xED68);
      case TokenT::REG_F:  return StatementNoArgCommand(0xED70);
    }

    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_JP>) { 
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt;

      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand(0x00E9); break;
        case TokenT::REG_IX: StatementNoArgCommand(0xDDE9); break;
        case TokenT::REG_IY: StatementNoArgCommand(0xFDE9); break;
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
      case TokenT::FLAG_C:  return StatementOneArgCommand(0x00DA, literal(2));
      case TokenT::FLAG_M:  return StatementOneArgCommand(0x00FA, literal(2));
      case TokenT::FLAG_NC: return StatementOneArgCommand(0x00D2, literal(2));
      case TokenT::FLAG_NZ: return StatementOneArgCommand(0x00C2, literal(2));
      case TokenT::FLAG_P:  return StatementOneArgCommand(0x00F2, literal(2));
      case TokenT::FLAG_PE: return StatementOneArgCommand(0x00EA, literal(2));
      case TokenT::FLAG_PO: return StatementOneArgCommand(0x00E2, literal(2));
      case TokenT::FLAG_Z:  return StatementOneArgCommand(0x00CA, literal(2));
    }

    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_JR>) {
    if (!match<4>({ TokenT::FLAG_C, TokenT::FLAG_NC, TokenT::FLAG_NZ, TokenT::FLAG_Z })) {
      return StatementOneArgCommand(0x0018, literal(2));
    }

    switch(peekPrev()->token) {
      case TokenT::FLAG_C:  return StatementOneArgCommand(0x0038, literal(1));
      case TokenT::FLAG_NC: return StatementOneArgCommand(0x0030, literal(1));
      case TokenT::FLAG_NZ: return StatementOneArgCommand(0x0020, literal(1));
      case TokenT::FLAG_Z:  return StatementOneArgCommand(0x0028, literal(1));
    }

    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_LD>) { 
    if (match<1>({ TokenT::LEFT_BRACE })) {
      switch (peek()->token) {
        case TokenT::REG_BC:
        case TokenT::REG_DE: {
          std::shared_ptr<Token> reg = advance();
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");
          consume(TokenT::REG_A, "Expect register 'A' after expression.");

          switch(reg->token) {
            case TokenT::REG_BC: return StatementNoArgCommand(0x0002);
            case TokenT::REG_DE: return StatementNoArgCommand(0x0012);
          }

          return Statement();
        }

        case TokenT::REG_HL:
          consume(TokenT::REG_HL, "Expect 'HL' after expression.");
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return StatementNoArgCommand(0x70 | Defs::Reg2Mask(peekPrev()->token));
          }

          return StatementOneArgCommand(0x36, literal(1));

        case TokenT::REG_IX: {
          consume(TokenT::REG_IX, "Expect 'IX' after expression.");
          Expression expr = offset(1);

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return StatementLambda(expr, [](std::vector<uint32_t>& argv) { return 0xDD7000 | (argv[1] << 8) | argv[0]; }, { Defs::Reg2Mask(peekPrev()->token)});
          }

          return StatementLambda({ expr, literal(1) }, [](std::vector<uint32_t>& argv) { return 0xDD360000 | (argv[0] << 8) | argv[1]; }, { Defs::Reg2Mask(peekPrev()->token)});
        }

        case TokenT::REG_IY: {
          consume(TokenT::REG_IY, "Expect 'IY' after expression.");
          Expression expr = offset(1);

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          consume(TokenT::COMMA, "Expect ',' after expression.");

          if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
            return StatementLambda(expr, [](std::vector<uint32_t>& argv) { return 0xFD7000 | (argv[1] << 8) | argv[0]; }, { Defs::Reg2Mask(peekPrev()->token)});
          }

          return StatementLambda({ expr, literal(1) }, [](std::vector<uint32_t>& argv) { return 0xFD360000 | (argv[0] << 8) | argv[1]; }, { Defs::Reg2Mask(peekPrev()->token)});
        }
      }


      Expression expr = literal(2);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after expression.");

      switch (advance()->token) {
        case TokenT::REG_A:  return StatementOneArgCommand(0x0032, expr);
        case TokenT::REG_BC: return StatementOneArgCommand(0xED43, expr);
        case TokenT::REG_DE: return StatementOneArgCommand(0xED53, expr);
        case TokenT::REG_HL: return StatementOneArgCommand(0x0022, expr);
        case TokenT::REG_IX: return StatementOneArgCommand(0xDD22, expr);
        case TokenT::REG_IY: return StatementOneArgCommand(0xFD22, expr);
        case TokenT::REG_SP: return StatementOneArgCommand(0xED73, expr);
      }


      error(peekPrev(), "Expect register to be 'A' | 'BC' | 'DE' | 'HL' | 'IX' | 'IY' | 'SP'");
      return Statement();
    }

    switch (advance()->token) {
      case TokenT::REG_A:
        consume(TokenT::COMMA, "Expect ',' after expression.");

        if (match<1>({ TokenT::LEFT_BRACE })) {
          Statement stmt;
          switch (peek()->token) {
            case TokenT::REG_BC: advance(); stmt = StatementNoArgCommand(0x000A); break;
            case TokenT::REG_DE: advance(); stmt = StatementNoArgCommand(0x001A); break;

            case TokenT::REG_HL:
            case TokenT::REG_IX: 
            case TokenT::REG_IY: stmt = addressRegisterOperation(0x007E); break;
          
            default: stmt = StatementOneArgCommand(0x003A, literal(2)); break;
          }

          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
          return StatementNoArgCommand(0x0078 | Defs::Reg2Mask(peekPrev()->token));
        }

        if (match<1>({ TokenT::REG_I })) return StatementNoArgCommand(0xED57);
        if (match<1>({ TokenT::REG_R })) return StatementNoArgCommand(0xED5F);

        return StatementOneArgCommand(0x003E, literal(1));

      case TokenT::REG_B:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation<3>({ 0x0046, 0x0040, 0x0006 });

      case TokenT::REG_C:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation<3>({ 0x004E, 0x0048, 0x000E });

      case TokenT::REG_D:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation<3>({ 0x0056, 0x0050, 0x0016 });

      case TokenT::REG_E:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation<3>({ 0x005E, 0x0058, 0x001E });

      case TokenT::REG_H:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation<3>({ 0x0066, 0x0060, 0x0026 });

      case TokenT::REG_L:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        return basicRegisterOperation<3>({ 0x006E, 0x0068, 0x002E });

      case TokenT::REG_I:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        consume(TokenT::REG_A, "Expect register 'A' after expression.");
        return StatementNoArgCommand(0xED47);

      case TokenT::REG_R:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        consume(TokenT::REG_A, "Expect register 'A' after expression.");
        return StatementNoArgCommand(0xED4F);

      case TokenT::REG_BC:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          Statement stmt = StatementOneArgCommand(0xED4B, literal(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return StatementOneArgCommand(0x0001, literal(2));

      case TokenT::REG_HL:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          Statement stmt = StatementOneArgCommand(0x002A, literal(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return StatementOneArgCommand(0x0021, literal(2));


      case TokenT::REG_IX:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          Statement stmt = StatementOneArgCommand(0xDD2A, literal(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return StatementOneArgCommand(0xDD21, literal(2));

      case TokenT::REG_IY:
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          Statement stmt = StatementOneArgCommand(0xFD2A, literal(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        return StatementOneArgCommand(0xFD21, literal(2));

      case TokenT::REG_SP: 
        consume(TokenT::COMMA, "Expect ',' after expression.");
        if (match<1>({ TokenT::LEFT_BRACE })) {
          Statement stmt = StatementOneArgCommand(0x002A, literal(2));
          consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
          return stmt;
        }

        switch (peek()->token) {
          case TokenT::REG_HL: advance(); return StatementNoArgCommand(0x00F9);
          case TokenT::REG_IX: advance(); return StatementNoArgCommand(0xDDF9);
          case TokenT::REG_IY: advance(); return StatementNoArgCommand(0xFDF9);
        }

        return StatementOneArgCommand(0x0031, literal(2));

    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L' | 'I' | 'R' | 'BC' | 'DE' | 'HL' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_OUT>) {
    consume(TokenT::LEFT_BRACE, "Expect '(' after expression.");

    if (!match<1>({ TokenT::REG_C })) {
      Expression expr = literal(1);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      consume(TokenT::COMMA, "Expect ',' after expression.");
      consume(TokenT::REG_A, "Expect register to be 'A'");
      return StatementOneArgCommand(0x00D3, expr);
    }

    consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
    consume(TokenT::COMMA, "Expect ',' after expression.");
    switch (advance()->token) {
      case TokenT::REG_A:  return StatementNoArgCommand(0xED79);
      case TokenT::REG_B:  return StatementNoArgCommand(0xED41);
      case TokenT::REG_C:  return StatementNoArgCommand(0xED49);
      case TokenT::REG_D:  return StatementNoArgCommand(0xED51);
      case TokenT::REG_E:  return StatementNoArgCommand(0xED59);
      case TokenT::REG_H:  return StatementNoArgCommand(0xED61);
      case TokenT::REG_L:  return StatementNoArgCommand(0xED69);
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_RES>) {
    Expression expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterComplexOperation(0xCB86);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementLambda(expr, [](std::vector<uint32_t>& argv) {
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
          case TokenT::REG_BC: return StatementNoArgCommand(0xED42);
          case TokenT::REG_DE: return StatementNoArgCommand(0xED52);
          case TokenT::REG_HL: return StatementNoArgCommand(0xED62);
          case TokenT::REG_SP: return StatementNoArgCommand(0xED72);
        }

        error(peekPrev(), "Expect register to be 'BC' | 'DE' | 'HL' | 'SP'");
        return Statement();
    }

    error(peekPrev(), "Expect register to be 'A' | 'HL'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_SET>) {
    Expression expr = literal(1);
    consume(TokenT::COMMA, "Expect ',' after first expression.");

    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterComplexOperation(0xCBC6);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementLambda(expr, [](std::vector<uint32_t>& argv) {
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
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(opcode[0]);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(opcode[1] | Defs::Reg2Mask(peekPrev()->token));
    }

    return StatementOneArgCommand(opcode[2], literal(1));
  }

  inline Statement basicRegisterOperation(std::array<uint32_t, 2> opcode) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterComplexOperation(opcode[0]);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(opcode[1] | Defs::Reg2Mask(peekPrev()->token));
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement addressRegisterOperation(uint32_t opcode) {
    switch (advance()->token) {
      case TokenT::REG_HL: return StatementNoArgCommand( 0x0000 | opcode);
      case TokenT::REG_IX: return StatementOneArgCommand(0xDD00 | opcode, offset(1));
      case TokenT::REG_IY: return StatementOneArgCommand(0xFD00 | opcode, offset(1));
    }

    error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
    return Statement();
  }

  inline Statement addressRegisterComplexOperation(uint32_t opcode) {
    uint32_t code = opcode & 0x00FF;
    uint32_t command = ((opcode & 0xFF00) << 8);

    switch (advance()->token) {
      case TokenT::REG_HL: return StatementNoArgCommand(opcode);
      case TokenT::REG_IX: return StatementLambda(offset(1), [](std::vector<uint32_t>& argv) { return 0xDD000000 | argv[1] | (argv[0] << 8); }, { command | code });
      case TokenT::REG_IY: return StatementLambda(offset(1), [](std::vector<uint32_t>& argv) { return 0xFD000000 | argv[1] | (argv[0] << 8); }, { command | code });
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

  std::vector<Statement> stmt;
  std::vector<std::string> errors;

private:
  int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};