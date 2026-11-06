#pragma once

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace Interpreter {

struct Origin {
  int32_t line = 0; 
  std::filesystem::path file;

  bool operator==(const Origin& o) const { return line == o.line && file == o.file; }
};

class Reader {
public:
  Reader(std::filesystem::path path) {
    if (!std::filesystem::exists(path = path.lexically_normal())) { printf("File '%s' not found\n", path.c_str()); return; }

    std::ifstream file(path); std::stringstream buffer;
    buffer << file.rdbuf(); file.close(); 

    auto dir = path.parent_path();
    source = buffer.str(); files.insert(path);

    const auto count = std::count(source.begin(), source.end(), '\n') + 1;
    for (int32_t i = 1; i <= count; i++) origins.push_back({ i, path });

    while (!isAtEnd()) {
      const int32_t started_at = curr; const char c = advance();

      if (c == ';') { advance('\n'); continue; }
      if (c == '#') {
        if (match("include")) {
          advance('"'); // find the file path
          const auto filepath = string(advance());

          // row of the merged text this directive sits on (0-based)
          const auto row = std::count(source.begin(), source.begin() + started_at, '\n');
          source.erase(started_at, curr - started_at);

          auto reader = Reader(dir / filepath);
          for (auto& f : reader.files) files.insert(f);

          // swap the directive's line for the included file's lines
          if (!reader.origins.empty()) {
            origins.erase(origins.begin() + row);
            origins.insert(origins.begin() + row, reader.origins.begin(), reader.origins.end());
          }

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
  std::vector<Origin> origins; // origins[i] = where line i+1 of `source` came from
  std::unordered_set<std::filesystem::path> files;
};
};