#pragma once
// #include "Lexer/Lexer.h"
// #include "Expression/Var.h"
#include "Visitor/Disassemble.h"
// #include "Statement/StatementVariable.h"

namespace Zcc {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/parsing-expressions.html#ambiguity-and-the-parsing-game
 *
 * Grammar:
 * 
 *  expression  -> assignment
 *  assignment  -> ternary ( '=' ternary )*
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
 *  factor      -> unary ( ('/' | '*' | '%') unary )*
 * 
 *  unary       -> ('++' | '--' | 'sizeof' )* ( '+' | '-' | '*' | '&' | '!' | '~' )? cast 
 *  cast        -> '(' type ')' cast | postfix
 *  type        -> ( 'void' | 'unsigned'? ( 'char' | 'short' | 'int') pointer? )
 *  pointer     -> '*' pointer | '*'
 *  postfix     -> primary ( '[' expression ']'  | '(' arguments? ')' | ('.' | '->') IDENTIFIER | '++' | '--' )*
 *  arguments   -> expression ( ',' expression )* 
 *  primary     -> CHAR | SHORT | INT | UNSIGNED_CHAR | UNSIGNED_SHORT | UNSIGNED_INT 
 *                 | IDENTIFIER | '{' arguments? '}' | '(' expression ')'
 * 
 */
class Parser {
public:
  Parser(std::vector<std::shared_ptr<Token>>& t): tokens(t) {}
  ~Parser() { reset(); }

  bool scan() {
    reset();
    // if (lexer.scan(src)) { errors.insert(errors.end(), lexer.errors.begin(), lexer.errors.end()); return true; }
    // program();
    temp32 = expression();
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
    auto expr = ternary();

    while (match<1>({ TokenT::ASSIGN })) {
      auto right = ternary();
      expr = std::make_shared<Expr::Assign>(expr, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> ternary() {
    auto expr = logic_or();

    if (match<1>({ TokenT::QUESTION_MARK })) {
      auto left = expression();
      consume(TokenT::COLON, "Expect ':' after expression.");
      auto right = expression();
      expr = std::make_shared<Expr::Ternary>(expr, left, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> logic_or() {
    auto expr = logic_and();

    while (match<1>({ TokenT::OP_OR })) {
      auto op = peekPrev();
      auto right = logic_and();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> logic_and() {
    auto expr = binary_or();

    while (match<1>({ TokenT::OP_AND })) {
      auto op = peekPrev();
      auto right = binary_or();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> binary_or() {
    auto expr = binary_xor();

    while (match<1>({ TokenT::BIT_OR })) {
      auto op = peekPrev();
      auto right = binary_xor();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> binary_xor() {
    auto expr = binary_and();

    while (match<1>({ TokenT::BIT_XOR })) {
      auto op = peekPrev();
      auto right = binary_and();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> binary_and() {
    auto expr = equality();

    while (match<1>({ TokenT::BIT_XOR })) {
      auto op = peekPrev();
      auto right = equality();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> equality() {
    auto expr = comparison();

    if (match<2>({ TokenT::OP_EQUAL, TokenT::OP_NOT_EQUAL })) {
      auto op = peekPrev();
      auto right = comparison();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> comparison() {
    auto expr = shift();

    if (match<4>({ TokenT::OP_MORE, TokenT::OP_MORE_OR_EQUAL, TokenT::OP_LESS, TokenT::OP_LESS_OR_EQUAL })) {
      auto op = peekPrev();
      auto right = shift();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> shift() {
    auto expr = term();

    if (match<2>({ TokenT::BIT_RIGHT_SHIFT, TokenT::BIT_LEFT_SHIFT })) {
      auto op = peekPrev();
      auto right = term();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> term() {
    auto expr = factor();

    while (match<2>({ TokenT::OP_PLUS, TokenT::OP_MINUS })) {
      auto op = peekPrev();
      auto right = factor();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> factor() {
    auto expr = unary();

    while (match<3>({ TokenT::OP_DIV, TokenT::STAR, TokenT::OP_MOD })) {
      auto op = peekPrev();
      auto right = unary();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline std::shared_ptr<Expression> cast() {
    int nStart = nCurr;
    if (!match<1>({ TokenT::LEFT_BRACE })) return postfix();

    auto t = type();
    if (t == nullptr) { nCurr = nStart; return postfix(); }

    consume(TokenT::RIGHT_BRACE, "Expected ')' after typename.");
    return std::make_shared<Expr::Cast>(t, cast());
  }

  inline std::shared_ptr<Expression> unary() {
    std::shared_ptr<Expression> expr = nullptr, right = nullptr;

    while (match<3>({ TokenT::OP_INC, TokenT::OP_DEC, TokenT::W_SIZEOF })) {
      auto op = peekPrev();
      expr = std::make_shared<Expr::Unary>(op, expr == nullptr ? right : expr);
    }

    if (match<6>({ TokenT::OP_PLUS, TokenT::OP_MINUS, TokenT::STAR, TokenT::AMPERSAND, TokenT::OP_NOT, TokenT::BIT_NOT })) {
      auto op = peekPrev();
      auto temp = cast(); right.swap(temp);

      expr = std::make_shared<Expr::Unary>(op, expr == nullptr ? right : expr);
    } else { auto temp = cast(); right.swap(temp); }
    
    return expr == nullptr ? right : expr;
  }

  inline std::shared_ptr<Expression> postfix() {
    auto expr = primary();

    while (match<6>({ TokenT::LEFT_SQUARE_BRACE, TokenT::LEFT_BRACE, TokenT::OP_DOT, TokenT::OP_ARROW, TokenT::OP_INC, TokenT::OP_DEC })) {
      auto op = peekPrev();

      switch (op->token) {
        case TokenT::OP_INC:
        case TokenT::OP_DEC:
          expr = std::make_shared<Expr::Suffix>(expr, op);
          break;

        case TokenT::OP_DOT:
        case TokenT::OP_ARROW:
          expr = std::make_shared<Expr::Get>(expr, op, identifier());
          break;

        case TokenT::LEFT_SQUARE_BRACE: {
          auto right = expression();
          consume(TokenT::RIGHT_SQUARE_BRACE, "Expect ']' after expression.");
          auto op = std::make_shared<Token>(TokenT::NONE, "", "", 0, 0);
          expr = std::make_shared<Expr::Get>(expr, op, right);
          break;
        }

        case TokenT::LEFT_BRACE: {
          auto args = arguments(TokenT::RIGHT_BRACE);
          expr = std::make_shared<Expr::Invoke>(expr, args);
          break;
        }
      }
    }
    
    return expr;
  }

  inline std::shared_ptr<Expression> type() {
    auto encapsulate = [&](std::shared_ptr<Expression> expr) {
      for (int i = 0; match<1>({ TokenT::STAR }); i++) {
        auto ptr = std::make_shared<Expr::Type>(peekPrev());
        expr = std::make_shared<Expr::Cast>(expr, ptr);
        if (i < PTRDEPTH) continue;

        error(peekPrev(), "Pointer type definition exceeded allow depth.");
        return (std::shared_ptr<Expression>)nullptr;
      }

      return expr;
    };

    auto type = advance();
    switch (type->token) {
      case TokenT::W_VOID: 
        return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::VOID)));

      case TokenT::W_UNSIGNED: {
        switch (advance()->token) {
          case TokenT::W_CHAR:
            return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::UNSIGNED_CHAR)));

          case TokenT::W_SHORT:
            return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::UNSIGNED_SHORT)));

          case TokenT::W_INT:
            return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::UNSIGNED_INT)));
        }
      }

      case TokenT::W_CHAR:
        return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::CHAR)));

      case TokenT::W_SHORT:
        return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::SHORT)));

      case TokenT::W_INT:
        return encapsulate(std::make_shared<Expr::Type>(convert(type, TokenT::INT)));
    }

    // error(type, "Unsupported type.");
    return nullptr;
  }

  inline std::shared_ptr<Expression> primary() {
    if (match<6>({ TokenT::CHAR, TokenT::SHORT, TokenT::INT, TokenT::UNSIGNED_CHAR, TokenT::UNSIGNED_SHORT, TokenT::INT })) {
      return std::make_shared<Expr::Literal>(peekPrev());
    }

    if (match<1>({ TokenT::LEFT_CURLY_BRACE })) {
      auto args = arguments(TokenT::RIGHT_CURLY_BRACE);
      return std::make_shared<Expr::Array>(args);
    }


    if (match<1>({ TokenT::LEFT_BRACE })) {
      auto expr = expression();
      consume(TokenT::RIGHT_BRACE, "Expect ')' after expression.");
      return std::make_shared<Expr::Group>(expr);
    }

    return identifier();
  }

  inline std::shared_ptr<Expression> identifier() {
    if (match<1>({ TokenT::IDENTIFIER })) {
      return std::make_shared<Expr::Var>(peekPrev());
    }

    error(peek(), "Unexpected expression.");
    return nullptr;
  }


  inline std::vector<std::shared_ptr<Expression>> arguments(TokenT closer) {
    std::vector<std::shared_ptr<Expression>> args{};
    if (peek()->token != closer) {
      do {
        args.push_back(expression());
      } while(match<1>({ TokenT::COMMA }));
    }

    consume(closer, "Expect closed brackets after expression.");
    return args;
  }



private:
  inline std::shared_ptr<Token> peek() { return tokens[nCurr]; }
  inline std::shared_ptr<Token> peekPrev() { return tokens[nCurr - 1]; }
  inline bool isAtEnd() { return peek()->token == TokenT::CMD_EOF; }
  inline bool check(TokenT type) { return isAtEnd() ? false : peek()->token == type; }

  inline std::shared_ptr<Token> convert(std::shared_ptr<Token> src, TokenT token) {
    return std::make_shared<Token>(token, src->lexeme, src->literal, src->col, src->line);
  }

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
  std::shared_ptr<Expression> temp32 = nullptr;
  std::vector<std::string> errors;

private:
  int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};