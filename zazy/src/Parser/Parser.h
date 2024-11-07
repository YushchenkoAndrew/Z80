#pragma once
// #include "Lexer/Lexer.h"
// #include "Expression/Var.h"
// #include "Visitor/Evaluate.h"
#include "src/Parser/Object/Void.h"
// #include "Statement/StatementVariable.h"

namespace Zazy {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/parsing-expressions.html#ambiguity-and-the-parsing-game
 *
 * Grammar:
 * 
 *  program       -> declaration* EOF
 *  declaration   -> struct_decl | enum_decl | func_decl | var_decl | label_decl | statement 
 * 
 *  struct_decl   -> 'struct' IDENTIFIER TODO:
 *  enum_decl     -> 'enum' IDENTIFIER '{' IDENTIFIER ( '='  expression )?  ( ',' IDENTIFIER ( '='  expression )? )* '}' ';'?
 *  func_decl     -> definitition '(' parameters? ')' ( block_stmt  | ';' )
 *  var_decl      -> definitition ( '='  expression )? ';'?
 *  label_decl    -> IDENTIFIER ':'
 * 
 *  definitition  -> type IDENTIFIER
 *  parameters    -> definitition ( ',' definitition )* 
 * 
 *  statement     -> if_stmt | switch_stmt | for_stmt | while_stmt | until_stmt |
 *                  return_stmt | goto_stmt | break_stmt | continue_stmt | expr_stmt | block_stmt
 * 
 *  if_stmt       -> 'if' '(' expression ')' statement ( 'else' statement )?
 *  switch_stmt   -> 'switch' '(' expression ')' '{' case_stmt* '}'
 *  case_stmt     -> ('case' expression ':' | 'default' ':') statement
 *  for_stmt      -> 'for' '(' ( var_decl | expr_stmt | ';' ) expression? ';' expression? ')' ( statement | ';' )
 *  while_stmt    -> 'while' '(' expression ')' statement
 *  until_stmt    -> 'do' block_stmt 'while' '(' expression ')' ';'?
 * 
 *  return_stmt   -> 'return' expression? ';'?
 *  goto_stmt     -> 'goto' expression ';'?
 *  break_stmt    -> 'break' ';'?
 *  continue_stmt -> 'continue' ';'?
 *  expr_stmt     -> expression ';'?
 *  block_stmt    -> '{' declaration* '}'
 * 
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
 *  factor      -> prefix ( ('/' | '*' | '%') prefix )*
 * 
 *  prefix      -> ('++' | '--' | 'sizeof' ) prefix | unary
 *  unary       -> ( '+' | '-' | '*' | '&' | '!' | '~' ) unary | cast 
 *  cast        -> '(' ( 'void' | 'char' | 'short' | 'int' ) '*'? ')' cast | postfix
 *  type        -> ( 'void' | 'char' | 'short' | 'int' ) '*'? | 'auto'
 *  postfix     -> primary ( '[' expression ']'  | '(' arguments? ')' | ('.' | '->') IDENTIFIER | '++' | '--' )*
 *  arguments   -> expression ( ',' expression )* 
 *  primary     -> CHAR | SHORT | INT | IDENTIFIER | '{' arguments? '}' | '(' expression ')'
 * 
 */
class Parser {
public:
  Parser(Lexer l): lexer(l) { 
      peek = lexer.next();
      peekNext = lexer.next();
  }

  ~Parser() {}

  stmt_t next() {
    // reset();
    // if (lexer.scan(src)) { errors.insert(errors.end(), lexer.errors.begin(), lexer.errors.end()); return true; }
    // program();
    // expression();
    // return errors.size();
    return declaration();
  }


//   inline stmt_t program() {

// // declaration* EOF
//   }


private:

  inline stmt_t declaration() {
    if (match<1>({ TokenT::W_ENUM })) return enum_decl();

    if (isType(peek)) {
      auto stmt = definitition();
      return check(TokenT::LEFT_BRACE) ? func_decl(stmt) : var_decl(stmt);
    }

    return statement();
  }

  inline stmt_t enum_decl() {
    std::vector<stmt_t> vars{};
    consume(TokenT::LEFT_CURLY_BRACE, "Expect '{' after identifier.");

    if (!check(TokenT::RIGHT_CURLY_BRACE)) {
      do {
        auto name = peek;
        consume(TokenT::IDENTIFIER, "Expect identifier in enum block.");

        token_t type = std::make_shared<Token>(TokenT::W_AUTO, "", "", peek->col, peek->line);
        vars.push_back(std::make_shared<Decl::Var>(type, name, match<1>({ TokenT::ASSIGN }) ? expression() : nullptr));
      } while(match<1>({ TokenT::COMMA }));
    }

    consume(TokenT::RIGHT_CURLY_BRACE, "Expect '}' after block.");
    match<1>({ TokenT::SEMICOLON });

    return std::make_shared<Decl::Enum>(vars);
  }

  inline std::shared_ptr<Decl::Var> definitition() {
    auto type = advance();
    if (peek->token == TokenT::STAR) type = advance();

    consume(TokenT::IDENTIFIER, "Expect identifier after type.");
    return std::make_shared<Decl::Var>(type, peekPrev, nullptr);
  }


  inline stmt_t func_decl(std::shared_ptr<Decl::Var> stmt) {
    consume(TokenT::LEFT_BRACE, "Expect '(' after identifier.");

    auto params = parameters(TokenT::RIGHT_BRACE);
    auto body = match<1>({ TokenT::SEMICOLON }) ? nullptr : block_stmt();

    return std::make_shared<Decl::Func>(stmt->type, stmt->name, params, body);
  }

  inline stmt_t var_decl(std::shared_ptr<Decl::Var> stmt) {
    if (match<1>({ TokenT::ASSIGN })) stmt->value = expression();

    match<1>({ TokenT::SEMICOLON });
    return stmt;
  }


  inline stmt_t statement() {
//  *  statement     -> if_stmt | switch_stmt | for_stmt | while_stmt | until_stmt |
//  *                  return_stmt | goto_stmt | break_stmt | continue_stmt | expr_stmt | block

    switch (peek->token) {
      case TokenT::W_SWTCH:
      case TokenT::W_DO: 

      case TokenT::W_GOTO: 
      case TokenT::W_BREAK: 
      case TokenT::W_CONTINUE: 
        // TODO:
        break;

      case TokenT::W_WHILE: 
        return while_stmt();

      case TokenT::W_FOR:
        return for_stmt();

      case TokenT::W_IF:
        return if_stmt();

      case TokenT::W_RETURN:
        return return_stmt();

      case TokenT::LEFT_CURLY_BRACE: 
        return block_stmt();

      default:
        return expr_stmt();
    }

    return nullptr;
  }

  inline stmt_t while_stmt() {
    consume(TokenT::W_WHILE, "Expect 'while' before conditional stmt.");
    consume(TokenT::LEFT_BRACE, "Expect '(' after 'while' word.");

    auto condition = expression();
    consume(TokenT::RIGHT_BRACE, "Expect ')' after 'while' expression.");

    stmt_t body = match<1>({ TokenT::SEMICOLON }) ? nullptr : statement();
    return std::make_shared<Stmt::While>(condition, body);
  }

  inline stmt_t for_stmt() {
    consume(TokenT::W_FOR, "Expect 'for' before conditional stmt.");
    consume(TokenT::LEFT_BRACE, "Expect '(' after 'for' word.");

    stmt_t init = check(TokenT::SEMICOLON) ? nullptr : isType(peek) ? var_decl(definitition()) : expr_stmt();
    // consume(TokenT::SEMICOLON, "Expect ';' after 'for' init expression."); // NOTE: this is impl in var_decl

    expr_t condition = check(TokenT::SEMICOLON) ? nullptr : expression();
    consume(TokenT::SEMICOLON, "Expect ';' after 'for' condition expression.");

    expr_t next = check(TokenT::RIGHT_BRACE) ? nullptr : expression();
    consume(TokenT::RIGHT_BRACE, "Expect ')' after 'for' expression.");

    stmt_t body = match<1>({ TokenT::SEMICOLON }) ? nullptr : statement();
    return std::make_shared<Stmt::For>(init, condition, next, body);
  }

  inline stmt_t if_stmt() {
    consume(TokenT::W_IF, "Expect 'if' before conditional stmt.");
    consume(TokenT::LEFT_BRACE, "Expect '(' after 'if' word.");

    auto condition = expression();

    consume(TokenT::RIGHT_BRACE, "Expect ')' after 'if' condition.");
    auto then = statement();

    stmt_t otherwise = match<1>({ TokenT::W_ELSE }) ? statement() : nullptr;

    return std::make_shared<Stmt::If>(condition, then, otherwise);
  }

  inline stmt_t return_stmt() {
    consume(TokenT::W_RETURN, "Expect 'return' stmt.");
    if (match<1>({ TokenT::SEMICOLON })) return std::make_shared<Stmt::Return>(nullptr);

    auto expr = expression();
    match<1>({ TokenT::SEMICOLON });
    return std::make_shared<Stmt::Return>(expr);
  }

  inline stmt_t block_stmt() {
    consume(TokenT::LEFT_CURLY_BRACE, "Expect '{' before block.");

    auto block = std::make_shared<Stmt::Block>((std::vector<stmt_t>){});
    while (!isAtEnd() && !check(TokenT::RIGHT_CURLY_BRACE)) {
      block->stmt.push_back(declaration());
      match<1>({ TokenT::SEMICOLON });
    }

    consume(TokenT::RIGHT_CURLY_BRACE, "Expect '}' after block.");
    return block;
  }

  inline stmt_t expr_stmt() {
    auto expr = expression();
    match<1>({ TokenT::SEMICOLON });
    return std::make_shared<Stmt::Expr>(expr);
  }

  inline expr_t expression() {
    return assignment();
  }

  inline expr_t assignment() {
    auto expr = ternary();

    while (match<1>({ TokenT::ASSIGN })) {
      auto right = ternary();
      expr = std::make_shared<Expr::Assign>(expr, right);
    }

    return expr;
  }

  inline expr_t ternary() {
    auto expr = logic_or();

    if (match<1>({ TokenT::QUESTION_MARK })) {
      auto left = expression();
      consume(TokenT::COLON, "Expect ':' after expression.");
      auto right = expression();
      expr = std::make_shared<Expr::Ternary>(expr, left, right);
    }

    return expr;
  }

  inline expr_t logic_or() {
    auto expr = logic_and();

    while (match<1>({ TokenT::OP_OR })) {
      auto op = peekPrev;
      auto right = logic_and();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t logic_and() {
    auto expr = binary_or();

    while (match<1>({ TokenT::OP_AND })) {
      auto op = peekPrev;
      auto right = binary_or();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t binary_or() {
    auto expr = binary_xor();

    while (match<1>({ TokenT::BIT_OR })) {
      auto op = peekPrev;
      auto right = binary_xor();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t binary_xor() {
    auto expr = binary_and();

    while (match<1>({ TokenT::BIT_XOR })) {
      auto op = peekPrev;
      auto right = binary_and();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t binary_and() {
    auto expr = equality();

    while (match<1>({ TokenT::BIT_XOR })) {
      auto op = peekPrev;
      auto right = equality();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t equality() {
    auto expr = comparison();

    if (match<2>({ TokenT::OP_EQUAL, TokenT::OP_NOT_EQUAL })) {
      auto op = peekPrev;
      auto right = comparison();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t comparison() {
    auto expr = shift();

    if (match<4>({ TokenT::OP_MORE, TokenT::OP_MORE_OR_EQUAL, TokenT::OP_LESS, TokenT::OP_LESS_OR_EQUAL })) {
      auto op = peekPrev;
      auto right = shift();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t shift() {
    auto expr = term();

    if (match<2>({ TokenT::BIT_RIGHT_SHIFT, TokenT::BIT_LEFT_SHIFT })) {
      auto op = peekPrev;
      auto right = term();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t term() {
    auto expr = factor();

    while (match<2>({ TokenT::OP_PLUS, TokenT::OP_MINUS })) {
      auto op = peekPrev;
      auto right = factor();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }

  inline expr_t factor() {
    auto expr = prefix();

    while (match<3>({ TokenT::OP_DIV, TokenT::STAR, TokenT::OP_MOD })) {
      auto op = peekPrev;
      auto right = prefix();
      expr = std::make_shared<Expr::Binary>(expr, op, right);
    }

    return expr;
  }
  
  inline expr_t prefix() {
    if (match<3>({ TokenT::OP_INC, TokenT::OP_DEC, TokenT::W_SIZEOF })) {
      auto op = peekPrev;
      auto right = prefix();
      return std::make_shared<Expr::Unary>(op, right);
    }

    return unary();
  }

  inline expr_t unary() {
    if (match<6>({ TokenT::OP_PLUS, TokenT::OP_MINUS, TokenT::STAR, TokenT::AMPERSAND, TokenT::OP_NOT, TokenT::BIT_NOT })) {
      auto op = peekPrev;
      auto right = unary();
      return std::make_shared<Expr::Unary>(op, right);
    }
    
    return cast();
  }

  inline expr_t cast() {
    if (peek->token == TokenT::LEFT_BRACE && isType(peekNext)) {
      consume(TokenT::LEFT_BRACE, "Expect '(' before type.");

      auto type = advance();
      if (peek->token == TokenT::STAR) type = advance();

      consume(TokenT::RIGHT_BRACE, "Expect ')' after type.");
      return std::make_shared<Expr::Cast>(type, cast());
    }

    return postfix();
  }

  inline expr_t postfix() {
    auto expr = primary();

    while (match<5>({ TokenT::LEFT_SQUARE_BRACE, TokenT::LEFT_BRACE, TokenT::OP_DOT, TokenT::OP_INC, TokenT::OP_DEC })) {
      auto op = peekPrev;

      switch (op->token) {
        case TokenT::OP_INC:
        case TokenT::OP_DEC:
          expr = std::make_shared<Expr::Suffix>(expr, op);
          break;

        case TokenT::OP_DOT:
          expr = std::make_shared<Expr::Get>(expr, op, identifier());
          break;

        case TokenT::LEFT_SQUARE_BRACE: {
          auto right = expression();
          consume(TokenT::RIGHT_SQUARE_BRACE, "Expect ']' after expression.");
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

  inline expr_t primary() {
    if (match<4>({ TokenT::CHAR, TokenT::SHORT, TokenT::INT, TokenT::STRING })) {
      return std::make_shared<Expr::Literal>(peekPrev);
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

  inline expr_t identifier() {
    if (match<1>({ TokenT::IDENTIFIER })) {
      return std::make_shared<Expr::Var>(peekPrev);
    }

    error(advance(), "Unexpected expression.");
    return  std::make_shared<Expr::Literal>(invalid());
  }


  inline std::vector<expr_t> arguments(TokenT closer) {
    std::vector<expr_t> args{};
    if (!check(closer)) {
      do {
        args.push_back(expression());
      } while(match<1>({ TokenT::COMMA }));
    }

    consume(closer, "Expect closed brackets after expression.");
    return args;
  }

  inline std::vector<stmt_t> parameters(TokenT closer) {
    std::vector<stmt_t> params{};
    if (!check(closer)) {
      do {
        params.push_back(definitition());
      } while(match<1>({ TokenT::COMMA }));
    }

    consume(closer, "Expect closed brackets after expression.");
    return params;
  }

  inline bool isType(token_t& t) {
    if (t == nullptr) return false;
    return t->token == TokenT::W_VOID ||
      t->token == TokenT::W_CHAR || 
      t->token == TokenT::W_SHORT || 
      t->token == TokenT::W_INT ||
      t->token == TokenT::W_AUTO;
  }


public:
  inline bool isAtEnd() { return peek->token == TokenT::CMD_EOF; }

private:
  inline bool check(TokenT type) { return isAtEnd() ? false : peek->token == type; }

  inline std::shared_ptr<Token> advance() {
    if (!isAtEnd()) {
      peekPrev = peek; 
      peek = peekNext;
      peekNext = lexer.next();
    }

    return peekPrev;
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

  inline void error(std::shared_ptr<Token> token, std::string message) {
    std::stringstream ss;
    ss << "[Ln " << token->line << " Col " << token->col << "] Error at '" << token->lexeme << "': " << message << "\n";
    errors.push_back(ss.str());
  }

  inline token_t invalid() {
    return std::make_shared<Token>(TokenT::INVALID, peekPrev->lexeme, peekPrev->literal, peekPrev->col, peekPrev->line);
  }


private:
  Lexer lexer;

  token_t peek = nullptr;
  token_t peekPrev = nullptr;
  token_t peekNext = nullptr;

  // std::vector<std::shared_ptr<Statement>> stmt;
  // std::shared_ptr<Expression> temp32 = nullptr;

public:
  std::vector<std::string> errors;
  // int32_t nCurr = 0; // index of the token, which is pointing to the curr token
};

};