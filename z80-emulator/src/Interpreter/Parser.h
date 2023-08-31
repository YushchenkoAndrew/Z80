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

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'BC' | 'C' | 'D' | 'DE' | 'E' | 'H' | 'HL' | 'IX' | 'IY' | 'L' | 'SP'");
    return Statement();
  }

  inline Statement Process(Int2Type<TokenT::CMD_DJNZ>) {
    return StatementOneArgCommand(0x0010, peekPrev(), literal(1));
  }

  inline Statement Process(Int2Type<TokenT::CMD_IM>) {
    return StatementLambda(literal(1), [](uint16_t val) {
      switch (val) {
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
    return StatementLambda(literal(1), [](uint16_t val) {
      switch (val) {
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
    switch (advance()->token) {
      case TokenT::REG_A:
        consume(TokenT::COMMA, "Expect ',' after first expression.");
        return basicRegisterOperation<3>({ 0x0086, 0x0080, 0x00C6 });

      case TokenT::REG_IX:
      case TokenT::REG_IY:
        uint16_t mask = peekPrev()->token == TokenT::REG_IX ? 0xDD00 : 0xFD00;

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

  // TODO:
  inline Statement Process(Int2Type<TokenT::CMD_BIT>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_CALL>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_EX>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_IN>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_JP>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_JR>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_LD>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_OUT>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_RES>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_SBC>) { return Statement(); }
  inline Statement Process(Int2Type<TokenT::CMD_SET>) { return Statement(); }

private:

  template<int32_t T>
  inline Statement basicRegisterOperation(std::array<uint16_t, T> opcode);

  inline Statement basicRegisterOperation(std::array<uint16_t, 3> opcode) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      Statement stmt = addressRegisterOperation(opcode[0]);
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(opcode[1] | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    return StatementOneArgCommand(opcode[2], peekPrev(), literal(1));
  }

  inline Statement basicRegisterOperation(std::array<uint16_t, 2> opcode) {
    if (match<1>({ TokenT::LEFT_BRACE })) {
      uint16_t code = opcode[0] & 0x00FF;
      uint16_t command = ((opcode[1] & 0xFF00) >> 8);

      Statement stmt;
      switch (advance()->token) {
        case TokenT::REG_HL: StatementNoArgCommand(opcode[0], peekPrev()); break;
        case TokenT::REG_IX: StatementOneArgCommand(0xDD00 | command, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral(std::to_string(code), TokenT::NUMBER))); break;
        case TokenT::REG_IY: StatementOneArgCommand(0xFD00 | command, peekPrev(), ExpressionGroup(offset(1), ExpressionLiteral(std::to_string(code), TokenT::NUMBER))); break;
        default: error(peekPrev(), "Expect register to be 'HL' | 'IY+o' | 'IX+o'");
      }

      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return stmt;
    }

    if (match<7>({ TokenT::REG_A, TokenT::REG_B, TokenT::REG_C, TokenT::REG_D, TokenT::REG_E, TokenT::REG_H, TokenT::REG_L })) {
      return StatementNoArgCommand(opcode[1] | Defs::Reg2Mask(peekPrev()->token), peekPrev());
    }

    error(peekPrev(), "Expect register to be 'A' | 'B' | 'C' | 'D' | 'E' | 'H' | 'L'");
    return Statement();
  }

  inline Statement addressRegisterOperation(uint16_t opcode) {
    switch (advance()->token) {
      case TokenT::REG_HL: return StatementNoArgCommand( 0x0000 | opcode, peekPrev());
      case TokenT::REG_IX: return StatementOneArgCommand(0xDD00 | opcode, peekPrev(), offset(1));
      case TokenT::REG_IY: return StatementOneArgCommand(0xFD00 | opcode, peekPrev(), offset(1));
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