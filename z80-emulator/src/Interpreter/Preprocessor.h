#pragma once

#include "src/Interpreter/Reader.h"
#include "src/Utils/Utils.h"
#include <cstdint>
#include <filesystem>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Interpreter {

/**
 * Expands assembly macros in the source code.
 * Define a macro with `#macro` and end it with `#end_macro`.
 *
 * @code
 * MACRO WAIT n
 *     LD B, n
 * wait\@:
 *     HALT
 *     DJNZ wait\@-$
 * ENDM
 *
 * WAIT 10
 * @endcode
 *
 * - Parameters are replaced with the supplied arguments.
 * - `\@` generates a unique suffix for each invocation (e.g. `wait_1`).
 * - Macro calls can be nested.
 * - Expanded lines retain the origin of the macro call for diagnostics.
 */
class Preprocessor {
public:
  Preprocessor() {}
  ~Preprocessor() { files.clear(); }

  void scan(std::filesystem::path file) {
    auto reader = Reader(file);
    curr = 0; source = reader.source; files = reader.files; origins = reader.origins; macros = reader.macros;
  
    int32_t unique = 0;
  
    while (!isAtEnd()) {
      if (peek() == ';') { advance('\n'); continue; }
      if (!Utils::IsAlphaDigit(peek())) { advance(); continue; }
    
      const int32_t started_at = curr;
      const auto name = identifier();                       // the whole word
      const auto it = macros.find(name);
      if (it == macros.end() || peek() == ':' || !atLineStart(started_at)) continue;
    
      const auto& [params, body] = it->second;
      whitespace(); const auto values = call();
      if (values.size() != params.size()) { printf("Macro '%s' expects %ld argument(s), got %ld", name.c_str(), params.size(), values.size()); continue; }
    
      const auto code = expand(body, params, values, ++unique);
    
      // the expansion adds lines: point them all at the call site
      const auto row = std::count(source.begin(), source.begin() + started_at, '\n');
      if (row < (int64_t)origins.size()) {
        const auto origin = origins[row];
        origins.insert(origins.begin() + row + 1, std::count(code.begin(), code.end(), '\n'), origin);
      }
    
      source.replace(started_at, curr - started_at, code);
      curr = started_at; // rescan so macros can call macros
    }
  }


  static std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t");
    return b == std::string::npos ? "" : s.substr(b, s.find_last_not_of(" \t") - b + 1);
  }

  std::vector<std::string> call() { // rest of line, split on top-level commas
    std::vector<std::string> args; std::string cur; int depth = 0; bool quoted = false;

    while (!isAtEnd() && peek() != '\n') {
      const char c = peek();

      if (!quoted && c == ';') break;
      if (c == '"') quoted = !quoted;
      if (!quoted) depth += (c == '(') - (c == ')');
      if (!quoted && depth == 0 && c == ',') { args.push_back(trim(cur)); cur.clear(); advance(); continue; }

      cur += advance();
    }

    cur = trim(cur);

    if (!cur.empty() || !args.empty()) args.push_back(cur);
    return args;
  }

  std::string expand(const std::string& body, const std::list<std::string>& params, const std::vector<std::string>& values, int32_t id) {
    std::unordered_map<std::string, std::string> map; size_t i = 0;
    for (const auto& p : params) map[p] = values[i++];

    std::string out;
    for (size_t j = 0; j < body.size();) {
      if (body.compare(j, 2, "\\@") == 0) { out += "_" + std::to_string(id); j += 2; continue; }
      if (!Utils::IsAlphaDigit(body[j])) { out += body[j++]; continue; }

      const auto start = j;
      while (j < body.size() && Utils::IsAlphaDigit(body[j])) j++;
      const auto word = body.substr(start, j - start);
      const auto p = map.find(word);
      out += p == map.end() ? word : p->second;
    }
    return out;
  }


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

  inline bool match(std::string pattern) {
    for (const auto c : pattern) {
      if (peek() != c) return false;
      advance();
    }

    return true;
  }

  bool atLineStart(int32_t i) {
    while (i > 0 && (source[i - 1] == ' ' || source[i - 1] == '\t')) i--;
    return i == 0 || source[i - 1] == '\n';
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
  std::vector<Origin> origins; 
  std::unordered_set<std::filesystem::path> files;
  std::unordered_map<std::string, std::pair<std::list<std::string>, std::string>> macros;
};
};