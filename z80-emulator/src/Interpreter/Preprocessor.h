#pragma once

#include "lib/olcPixelGameEngine.h"
#include "src/Utils/Utils.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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
        if (match("include")) {
          advance('"'); // find the file path
          const auto filepath = string(advance());

          source.erase(started_at, curr - started_at);
          source.insert(curr = started_at, read(filedir / filepath));
          continue;
        }

        if (match("define")) {
          whitespace(); const auto name = identifier();
          whitespace(); const auto args = arguments();
          whitespace(); const auto code = snippet();

          macros[name] = std::pair(args, code);
          source.erase(started_at, curr - started_at);
          continue;
        }
      }
    }

    return snippets();
  }


private:
  void snippets() {
    curr = 0;

    while (!isAtEnd()) {
      const int32_t started_at = curr; const char c = advance();
      if (!Utils::IsAlphaDigit(c)) continue;

      const auto name = identifier();
      if (!macros.count(name)) continue;

      const auto [args, snippet] = macros[name];
      if (!args.size()) {
        // Object-like macro: simple string insertion right into loop stream
        source.erase(started_at, curr - started_at);
        source.insert(started_at, snippet);
        continue;
      }

      whitespace(); const auto params = arguments();

      std::string code = snippet; size_t index = -1;
      for (const auto& arg: args) {
        if (++index >= params.size()) break;

        for (size_t j = 0; j < code.size(); j++) {
          const int32_t started_at = j;
          while (j < code.size() && Utils::IsAlphaDigit(code[j])) j++;

          if (j == started_at || arg != source.substr(started_at, j - started_at)) continue;
          code.erase(started_at, j - started_at);
          code.insert(started_at, *std::next(params.begin(), index));
        }
      }

      source.erase(started_at, curr - started_at);
      source.insert(started_at, code);
    }
  }

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

  std::string identifier() {
    const auto started_at = curr;
    while (!isAtEnd() && Utils::IsAlphaDigit(source[curr])) advance();
    return source.substr(started_at, curr - started_at);
  }

  std::list<std::string> arguments() {
    std::list<std::string> args;

    if (!isAtEnd() && peek() == '(') {
      advance(); // consume '('

      // Parse parameters comma by comma
      while (!isAtEnd() && peek() != ')') {
        whitespace();

        if (source[curr] == ')') break;
        if (source[curr] == ',') whitespace();

        args.push_back(identifier());
      }

      if (!isAtEnd()) advance(); // consume ')'
    }

    return args;
  }

  std::string snippet() {
    std::string body;

    while (!isAtEnd()) {
      const char c = peek();
      if (c == '\n') break; // Normal line end finishes macro definition
      if (c != '\\') { body += advance(); continue; }

      advance(); // consume '\\'

      // If the next visible thing is newline, skip it to merge lines
      if (!isAtEnd() && peek() == '\r') advance();
      if (!isAtEnd() && peek() == '\n') { advance(); body += " "; continue; } // Replace line break with a spacer

      body += '\\'; // It wasn't an escape, add it literally
    }

    return body;
  }

  // TODO: Impl this
  // std::string expandFunctionMacro(const Macro& macro, const std::vector<std::string>& args) {
  //   std::string expanded = macro.body;
  //   // Replace parameters backward to prevent partial matches ruining overlapping names
  //   for (size_t i = 0; i < macro.params.size(); ++i) {
  //       if (i >= args.size()) break;
  //       const std::string& param = macro.params[i];
  //       const std::string& arg = args[i];

  //       size_t pos = 0;
  //       while ((pos = expanded.find(param, pos)) != std::string::npos) {
  //           // Guarantee token boundaries so we don't replace substrings of other variables
  //           bool left_bound = (pos == 0 || !isalnum(expanded[pos - 1]) && expanded[pos - 1] != '_');
  //           bool right_bound = (pos + param.length() == expanded.length() || !isalnum(expanded[pos + param.length()]) && expanded[pos + param.length()] != '_');
            
  //           if (left_bound && right_bound) {
  //               expanded.replace(pos, param.length(), arg);
  //               pos += arg.length();
  //           } else {
  //               pos += param.length();
  //           }
  //       }
  //   }
  //   return expanded;
  // }


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

private:
  int32_t curr = 0; // index of the src, which is pointing to the curr char

public:
  std::string source;
  std::unordered_set<std::filesystem::path> files;
  std::unordered_map<std::string, std::pair<std::list<std::string>, std::string>> macros;
};
};