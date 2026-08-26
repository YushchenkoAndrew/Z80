#pragma once

#include "lib/olcPixelGameEngine.h"
#include <unordered_set>

namespace Interpreter {

class Preprocessor {
public:
  Preprocessor() {}
  ~Preprocessor() { files.clear(); }

  void scan(std::filesystem::path file) {
    const auto filedir = std::filesystem::path(file).remove_filename();
    curr = 0; files.clear(); source = read(file);

    while (!isAtEnd()) {
      const int32_t started_at = curr; const char c = advance();

      if (c == ';') { advance('\n'); continue; }
      if (c == '#') {
        if (!match("include")) continue;

        advance('"'); // find the file path
        const auto filepath = string(advance());

        source.erase(started_at, curr - started_at);
        source.insert(curr = started_at, read(filedir / filepath));
      }
    }
  }


private:
  std::string read(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) return "";

    std::ifstream file(path); std::stringstream buffer;
    buffer << file.rdbuf(); file.close(); 

    return (files.insert(path.lexically_normal()), buffer.str());
  }

  std::string string(const char c) {
    const auto started_at = curr;
    while (peek() != c && !isAtEnd()) advance();

    if (peek() != c) return "";
    advance(); // closing quote

    return source.substr(started_at, curr - started_at - 1);
  }

  inline bool match(std::string pattern) {
    for (const auto c : pattern) {
      if (peek() != c) return false;
      advance();
    }

    return true;
  }

  inline const char advance() { return source[curr++]; }
  inline void advance(const char c) { while (peek() != c && !isAtEnd()) advance(); }
  inline const char peek() { return isAtEnd() ? '\0' : source[curr]; }
  inline bool isAtEnd() { return curr >= source.length(); }

private:
  int32_t curr = 0; // index of the src, which is pointing to the curr char

public:
  std::string source;
  std::unordered_set<std::filesystem::path> files;
};
};