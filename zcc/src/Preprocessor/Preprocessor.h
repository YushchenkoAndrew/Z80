#pragma once
#include "src/Token.h"


namespace Zcc {

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#recognizing-lexemes
 */
class Preprocessor {
public:
  Preprocessor(std::string path): vars({}) {
    filedir = std::filesystem::path(path).remove_filename();
    filename = std::filesystem::path(path).filename();

    std::ifstream f(path); std::stringstream buf;
    buf << f.rdbuf(); f.close();

    src = buf.str();
  }

  ~Preprocessor() { reset(); }

private:
  void parse();

public:
  bool scan() {
    // src = text; 
    reset();
    
    while (!isAtEnd()) parse();

    if (scopes.size()) error("Unclosed macros condition statement .");
    return errors.size();
  }


private:
  inline void reset() {
    nStart = 0; nCurr = 0; nCol = 1; nLine = 1;
    vars.clear(); errors.clear();
  }

  std::unique_ptr<Token> string(const char &c) {
    while (peek() != c) {
      if (peek() == '\n' || peek() == '\r') break;
      advance();
    }

    if (peek() != c) return error("Unclosed string.");
    if (nCurr - nStart - 2 > STRSIZE) return error("String exceeded allowed size of " + std::to_string(STRSIZE) + "." );

    advance(); // closing quote
    return createToken(TokenT::NONE, src.substr(nStart + 1, nCurr - nStart - 2));
  }

  std::unique_ptr<Token> comments() {
    // while (advance() != '\n' && !isAtEnd());
    while (advance() != '\n');
    src.replace(nStart, nCurr - nStart, "");

    nCurr = nStart;
    return createToken(TokenT::NONE, "");
  }

  std::unique_ptr<Token> identifier() {
    while (isAlphaNumeric(peek())) advance();

    std::string literal = src.substr(nStart, nCurr - nStart);
    if (!has(literal)) return createToken(TokenT::IDENTIFIER, literal);

    src.replace(nStart, nCurr - nStart, vars[literal]);
    return createToken(TokenT::NONE);
  }

  void macros() {
    while (isAlphaNumeric(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(nStart, nCurr - nStart);
    foreach<CommandList, AnyType<-1, std::string>>::Value2Process(this);
  }

public:
  template<int32_t T>
  void Process(Int2Type<T>) {
    error("Unknown operation '" + src.substr(nStart, nCurr - nStart) + "'");
    // return std::make_shared<Statement>();
  }


  inline void Process(Int2Type<TokenT::CMD_INCLUDE>) { 
    blank();
    if (peek() != '"') return (void)error("Expected \" after #include keyword.");

    int32_t nStart = savepoint();
    auto path = string(advance());
    auto filepath = filedir / std::filesystem::path(path->literal);

    std::ifstream f(filepath); std::stringstream buf;
    if (!f) return (void)error("File '" + path->literal + "' cannot be opened.");

    buf << f.rdbuf(); f.close();
    src.replace(nStart, nCurr - nStart, buf.str()); nCurr = nStart; 
  }

  inline void Process(Int2Type<TokenT::CMD_DEFINE>) { 
    blank(); int32_t nStart = savepoint();
    auto var = identifier(); blank();

    int nValueAt = nCurr; 
    while (peek() != '\n') parse();
    std::string value = src.substr(nValueAt, nCurr - nValueAt);

    if (value.size()) vars[var->literal] = value;
    else vars[var->literal] = "true";

    src.replace(nStart, nCurr - nStart, ""); nCurr = nStart;
  }

  inline void Process(Int2Type<TokenT::CMD_UNDEF>) { 
    blank(); int32_t nStart = savepoint();
    auto var = identifier(); blank();

    if (has(var->literal)) vars.erase(vars.find(var->literal));
    src.replace(nStart, nCurr - nStart, ""); nCurr = nStart;
  }

  inline void Process(Int2Type<TokenT::CMD_IFDEF>) { 
    blank(); int32_t nStart = savepoint();
    while(advance() != '\n' && !isAtEnd());

    std::string literal = src.substr(this->nStart, nCurr - this->nStart - 1);
    if (!literal.size()) return (void)error("Expected variable after #ifdef.");

    scopes.push_back(std::pair(has(literal), nStart));
    src.replace(nStart, nCurr - nStart, ""); nCurr = nStart;
  }

  inline void Process(Int2Type<TokenT::CMD_IFNDEF>) { 
    blank(); int32_t nStart = savepoint();
    while(advance() != '\n' && !isAtEnd());

    std::string literal = src.substr(this->nStart, nCurr - this->nStart - 1);
    if (!literal.size()) return (void)error("Expected variable after #ifdef.");

    scopes.push_back(std::pair(!has(literal), nStart));
    src.replace(nStart, nCurr - nStart, ""); nCurr = nStart;
  }

  inline void Process(Int2Type<TokenT::CMD_ELSE>) { 
    if (!scopes.size()) return (void)error("Unexpected #else command.");

    auto curr = scopes.back(); scopes.pop_back();
    if (curr.first) src.replace(nStart, nCurr - nStart, ""); 
    else src.replace(curr.second, nCurr - curr.second, "");

    scopes.push_back(std::pair(!curr.first, nStart));
    nCurr = nStart;
  }

  inline void Process(Int2Type<TokenT::CMD_ENDIF>) { 
    if (!scopes.size()) return (void)error("Unexpected #endif command.");

    auto curr = scopes.back(); scopes.pop_back();
    if (curr.first) src.replace(nStart, nCurr - nStart, ""); 
    else src.replace(curr.second, nCurr - curr.second, "");

    nCurr = nStart;
  }

private:
  inline const char advance() { nCol++; return src[nCurr++]; }
  inline const char peek() { return isAtEnd() ? '\0' : src[nCurr]; }
  inline const char peekPrev() { return !nCurr ? '\0' : src[nCurr - 1]; }
  inline const char peekNext() { return nCurr + 1 >= src.length() ? '\0' : src[nCurr + 1]; }

  inline bool has(const std::string& key) { return vars.find(key) != vars.end(); }
  inline int32_t savepoint() { int32_t nPrev = nStart; nStart = nCurr; return nPrev; }

  inline void blank() { while(match(' ')); }

  template<int32_t T>
  inline bool match(std::array<const char, T> arr) {
    for (auto& el : arr) if (match(el)) return true;
    return false;
  }

  inline bool match(const char expected) {
    if (isAtEnd()) return false;
    if (src[nCurr] != expected) return false;

    nCol++; nCurr++;
    return true;
  }

  inline std::unique_ptr<Token> createToken(TokenT type, std::string literal = "") {
    const int32_t len = nCurr - nStart;
    return std::make_unique<Token>(Token(type, src.substr(nStart, len), literal, nCol - len, nLine));
  }

  inline bool isAtEnd() { return nCurr >= src.length(); }
  inline bool isAlpha(const char &c) { return Utils::IsAlpha(c) || c == '.' || c == '\''; }
  inline bool isAlphaNumeric(const char &c) { return isAlpha(c) || Utils::IsDigit(c); }


  inline std::unique_ptr<Token> error(std::string message) {
    errors.push_back("[Ln " + std::to_string(nLine) + " Col " + std::to_string(nCurr) + "] Error: " + message + "\n");
    return createToken(TokenT::NONE);
  }

public:
  void debug() {
    size_t curr = 0, prev = -1;
    while ((curr = src.find("\n", prev + 1)) != std::string::npos) {
      printf("[%s] %s", alias, src.substr(prev + 1, curr - prev).c_str());
      prev = curr;
    }
  }

private:
  std::string filedir;
  std::string filename;

  int32_t nStart = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the src, which is pointing to the curr char

  int32_t nCol = 1; // tracks current line position
  int32_t nLine = 1; // tracks current line position

  std::vector<std::pair<bool, int32_t>> scopes;
  std::unordered_map<std::string, std::string> vars;

public:
  const char* alias = "PROC";

  std::string src;
  std::vector<std::string> errors;
};

};