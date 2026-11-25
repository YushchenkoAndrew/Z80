#include "Lexer.h"

namespace Zazy {

char Lexer::escape(bool& ok) {
  ok = true;
  const char c = advance();
  switch (c) {
    case 'n': return '\n';  case 'r': return '\r';  case 't': return '\t';
    case 'a': return '\a';  case 'b': return '\b';  case 'f': return '\f';
    case 'v': return '\v';  case '\\': return '\\'; case '\'': return '\'';
    case '"': return '"';   case '?': return '?';
    case 'x': {
      if (!Utils::IsHexDigit(peek())) { ok = false; return 0; }
      int v = 0;
      while (Utils::IsHexDigit(peek())) v = (v << 4) | Utils::Hex2Int(advance());
      return (char)v;
    }
    default:
      if (Utils::IsOctDigit(c)) {            // \0 \033 ...
        int v = c - '0';
        for (int i = 0; i < 2 && Utils::IsOctDigit(peek()); i++) v = (v << 3) | (advance() - '0');
        return (char)v;
      }
  }

  ok = false;
  return 0;
}

token_t Lexer::string() {
  std::string out;

  while (!isAtEnd() && peek() != '"') {
    if (peek() == '\n') break;
    if (peek() == '\\') {
      advance();
      bool ok; const char c = escape(ok);
      if (!ok) return error("Unknown escape sequence.");
      out += c;
      continue;
    }

    out += advance();
  }

  if (peek() != '"') return error("Unclosed string.");

  advance(); // closing quote
  return newToken(TokenT::STRING, out);
}

token_t Lexer::character() {
  int v = 0;

  if (peek() == '\\') {
    advance();
    bool ok; v = (uint8_t)escape(ok);
    if (!ok) return error("Unknown escape sequence.");
  } else if (!isAtEnd() && peek() != '\'' && peek() != '\n') v = (uint8_t)advance();
  else return error("Empty character literal.");

  if (!match('\'')) return error("Unclosed character literal.");
  return newToken(TokenT::CHAR, std::to_string(v));
}

token_t Lexer::number() {
  if (peekPrev() == '0' && (peek() == 'x' || peek() == 'X' || peek() == 'b' || peek() == 'B' || peek() == 'o' || peek() == 'O')) {
    const char kind = advance();
    int32_t base = 16;
    const int32_t from = nCurr;

    switch (kind) {
      case 'x': case 'X': base = 16; while (Utils::IsHexDigit(peek())) advance(); break;
      case 'o': case 'O': base = 8;  while (Utils::IsOctDigit(peek())) advance(); break;
      default:            base = 2;  while (Utils::IsBinDigit(peek())) advance(); break;
    }

    if (from == nCurr) return error("Invalid number.");

    const auto num = std::stoull(src.substr(from, nCurr - from), nullptr, base);
    while (peek() == 'u' || peek() == 'U' || peek() == 'l' || peek() == 'L') advance();
    if (num > 0xFFFFFFFFull) return error("Number is too big.");
    return newToken(TokenT::NUMBER, std::to_string(num));
  }

  while (Utils::IsDigit(peek())) advance();

  const auto num = std::stoull(src.substr(nStart, nCurr - nStart));
  while (peek() == 'u' || peek() == 'U' || peek() == 'l' || peek() == 'L') advance();
  if (num > 0xFFFFFFFFull) return error("Number is too big.");
  return newToken(TokenT::NUMBER, std::to_string(num));
}

token_t Lexer::asmBlock(int32_t line, int32_t col) {
  // current position is right after '{'
  int32_t depth = 1;
  std::string text;

  while (!isAtEnd()) {
    const char c = advance();
    if (c == '\n') { nLine++; nCol = 1; }

    if (c == ';') {                       // asm comment: braces inside do not count
      text += c;
      while (!isAtEnd() && peek() != '\n') text += advance();
      continue;
    }

    if (c == '"') {                       // string inside asm
      text += c;
      while (!isAtEnd() && peek() != '"' && peek() != '\n') text += advance();
      if (peek() == '"') text += advance();
      continue;
    }

    if (c == '{') depth++;
    if (c == '}' && --depth == 0) {
      return std::make_shared<Token>(Token(TokenT::ASM_BLOCK, "asm", text, col, line));
    }

    text += c;
  }

  nStartLine = line; nStartCol = col;
  return error("Unclosed asm block.");
}

token_t Lexer::identifier() {
  while (Utils::IsAlphaDigit(peek())) advance();

  const auto text = src.substr(nStart, nCurr - nStart);
  const auto it = Keywords().find(text);
  if (it == Keywords().end()) return newToken(TokenT::IDENTIFIER);

  if (it->second == TokenT::W_ASM) {      // asm { raw asm }
    int32_t i = nCurr;
    while (i < (int32_t)src.size() && (src[i] == ' ' || src[i] == '\t' || src[i] == '\r' || src[i] == '\n')) i++;

    if (i < (int32_t)src.size() && src[i] == '{') {
      const auto line = nStartLine, col = nStartCol;
      for (int32_t k = nCurr; k <= i; k++) { if (src[k] == '\n') { nLine++; nCol = 1; } else nCol++; }
      nCurr = i + 1;
      return asmBlock(line, col);
    }
  }

  return newToken(it->second);
}

token_t Lexer::next() {
  while (!isAtEnd()) {
    nStart = nCurr; nStartCol = nCol; nStartLine = nLine;

    const char c = advance();
    switch (c) {
      case ':': return newToken(TokenT::COLON);
      case ';': return newToken(TokenT::SEMICOLON);
      case ',': return newToken(TokenT::COMMA);
      case '?': return newToken(TokenT::QUESTION_MARK);
      case '.': return newToken(TokenT::OP_DOT);
      case '~': return newToken(TokenT::BIT_NOT);

      case '(': return newToken(TokenT::LEFT_BRACE);
      case ')': return newToken(TokenT::RIGHT_BRACE);
      case '[': return newToken(TokenT::LEFT_SQUARE_BRACE);
      case ']': return newToken(TokenT::RIGHT_SQUARE_BRACE);
      case '{': return newToken(TokenT::LEFT_CURLY_BRACE);
      case '}': return newToken(TokenT::RIGHT_CURLY_BRACE);

      case '*':
        if (match('=')) return newToken(TokenT::ASSIGN_MUL);
        return newToken(TokenT::STAR);

      case '/':
        if (match('/')) { while (!isAtEnd() && peek() != '\n') advance(); break; }
        if (match('*')) {
          while (!isAtEnd() && !(peek() == '*' && peekNext() == '/')) { if (advance() == '\n') { nLine++; nCol = 1; } }
          if (!isAtEnd()) { advance(); advance(); }
          break;
        }
        if (match('=')) return newToken(TokenT::ASSIGN_DIV);
        return newToken(TokenT::OP_DIV);

      case '%':
        if (match('=')) return newToken(TokenT::ASSIGN_MOD);
        return newToken(TokenT::OP_MOD);

      case '!':
        if (match('=')) return newToken(TokenT::OP_NOT_EQUAL);
        return newToken(TokenT::OP_NOT);

      case '=':
        if (match('=')) return newToken(TokenT::OP_EQUAL);
        return newToken(TokenT::ASSIGN);

      case '+':
        if (match('+')) return newToken(TokenT::OP_INC);
        if (match('=')) return newToken(TokenT::ASSIGN_PLUS);
        return newToken(TokenT::OP_PLUS);

      case '-':
        if (match('-')) return newToken(TokenT::OP_DEC);
        if (match('=')) return newToken(TokenT::ASSIGN_MINUS);
        if (match('>')) return newToken(TokenT::OP_ARROW);
        return newToken(TokenT::OP_MINUS);

      case '^':
        if (match('=')) return newToken(TokenT::ASSIGN_BIT_XOR);
        return newToken(TokenT::BIT_XOR);

      case '|':
        if (match('|')) return newToken(TokenT::OP_OR);
        if (match('=')) return newToken(TokenT::ASSIGN_BIT_OR);
        return newToken(TokenT::BIT_OR);

      case '&':
        if (match('&')) return newToken(TokenT::OP_AND);
        if (match('=')) return newToken(TokenT::ASSIGN_BIT_AND);
        return newToken(TokenT::AMPERSAND);

      case '>':
        if (match('=')) return newToken(TokenT::OP_MORE_OR_EQUAL);
        if (match('>')) return newToken(match('=') ? TokenT::ASSIGN_SHR : TokenT::BIT_RIGHT_SHIFT);
        return newToken(TokenT::OP_MORE);

      case '<':
        if (match('=')) return newToken(TokenT::OP_LESS_OR_EQUAL);
        if (match('<')) return newToken(match('=') ? TokenT::ASSIGN_SHL : TokenT::BIT_LEFT_SHIFT);
        return newToken(TokenT::OP_LESS);

      case '"': return string();
      case '\'': return character();

      case '\n': nLine++; nCol = 1; break;
      case ' ': case '\r': case '\t': break; // Ignore whitespace.

      default:
        if (Utils::IsDigit(c)) return number();
        if (Utils::IsAlpha(c)) return identifier();
        return error(std::string("Unexpected character '") + c + "'.");
    }
  }

  nStart = nCurr; nStartCol = nCol; nStartLine = nLine;
  return newToken(TokenT::T_EOF);
}
};
