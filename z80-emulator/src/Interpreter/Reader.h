#pragma once

#include "src/Utils/Utils.h"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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
      const int32_t started_at = curr;

      if (peek() == ';') { advance('\n'); continue; }
      if (match("INCLUDE")) {
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
          macros.insert(reader.macros.begin(), reader.macros.end());
        }

        source.insert(curr = started_at, reader.source);
        continue;
      }

      if (match("MACRO")) { position.push_back(started_at); continue; }
      if (match("ENDM")) {
        if (position.empty()) continue;

        const int32_t end_at = started_at; // start of '#end_macro'
        const int32_t begin  = position.back(); position.pop_back();
        const int32_t finish = curr;  // right after '#end_macro'

        curr = begin; match("MACRO");
        whitespace(); const auto name = identifier();
        const auto args = parameters();
        printf("%s\n", name.c_str());

        auto body = source.substr(curr, end_at - curr);
        body.erase(0, body.find_first_not_of("\r\n"));
        body.erase(body.find_last_not_of(" \t\r\n") + 1);
        macros[name] = { args, body };

        // keep the line count so `origins` stays aligned
        const auto lines = std::count(source.begin() + begin, source.begin() + finish, '\n');
        source.replace(begin, finish - begin, std::string(lines, '\n'));
        curr = begin + lines;
        continue;
      }

      advance();
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

  std::string identifier() {
    const auto started_at = curr;
    while (!isAtEnd() && Utils::IsAlphaDigit(source[curr])) advance();
    return source.substr(started_at, curr - started_at);
  }

  std::list<std::string> parameters() {
    std::list<std::string> args;

    while (!isAtEnd() && peek() != '\n' && peek() != ';') {
      whitespace(); const auto id = identifier();
      if (!id.empty()) args.push_back(id);

      whitespace();

      if (peek() == ',') advance();
      else if (id.empty()) break; // unexpected char: stop instead of spinning
    }

    return args;
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

  std::list<int32_t> position;
  std::unordered_map<std::string, std::pair<std::list<std::string>, std::string>> macros;

};
};