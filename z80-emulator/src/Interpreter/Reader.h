#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>

namespace Interpreter {

class Reader {
public:
  Reader(std::filesystem::path path) {
    if (!std::filesystem::exists(path = path.lexically_normal())) { printf("File '%s' not found\n", path.c_str()); return; }

    std::ifstream file(path); std::stringstream buffer;
    buffer << file.rdbuf(); file.close(); 

    auto dir = path.parent_path();
    source = buffer.str(); files.insert(path);

    while (!isAtEnd()) {
      const int32_t started_at = curr; const char c = advance();

      if (c == ';') { advance('\n'); continue; }
      if (c == '#') {
        if (match("include")) {
          advance('"'); // find the file path
          const auto filepath = string(advance());

          source.erase(started_at, curr - started_at);

          auto reader = Reader(dir / filepath);
          for (auto& f : reader.files) files.insert(f);
          source.insert(curr = started_at, reader.source);
          continue;
        }
      }
    }
  }

  ~Reader() {}

private:
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
  inline void advance(const char c) { while (!isAtEnd() && peek() != c) advance(); }
  inline void whitespace() { while (!isAtEnd() && (peek() == ' ' || peek() == '\t')) advance(); }
  inline const char peek() { return isAtEnd() ? '\0' : source[curr]; }
  inline bool isAtEnd() { return curr >= source.length(); }

public:
  int32_t curr = 0; // index of the src, which is pointing to the curr char
  std::string source;
  std::unordered_set<std::filesystem::path> files;
};
};