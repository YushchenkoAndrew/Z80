#pragma once

#include "src/Interpreter/Token.h"
#include <cstdint>
#include <cstdio>
#include <map>
#include <optional>
#include <vector>

namespace Interpreter {

/**
 * Two-way map between emitted bytes and the source statement that produced them.
 *
 *   address -> source:  "which line produced the byte at 0x8042?"
 *   source  -> address: "where did line 17 end up in memory?"
 *
 * Lines and columns are 1-based (same as Token) and refer to the text the Lexer saw,
 * i.e. AFTER the Preprocessor merged includes and expanded macros.
 */
class Lookup {
public:
  struct Entry { uint32_t address = 0; uint32_t size = 0; int32_t line = 0; };

  void reset() { byAddress.clear(); byLine.clear(); }
  void add(uint32_t address, uint32_t size, const Token& token) {
    if (!size) return;

    byAddress[address] = Entry{ address, size, token.line };
    byLine[token.line].push_back(address);
  }

  /// memory -> code. Works for any byte inside an instruction, not only the first one.
  std::optional<Entry> atAddress(uint32_t address) const {
    auto it = byAddress.upper_bound(address);
    if (it == byAddress.begin()) return std::nullopt;
    else --it; 

    const auto bContain = it->second.address >= address && it->second.address - address < it->second.size;
    if (!bContain) return std::nullopt;
    return it->second;
  }

  /// code -> memory. Usually one entry; several if a line holds more than one statement.
  std::vector<Entry> atLine(int32_t line) const {
    std::vector<Entry> result;

    auto it = byLine.find(line);
    if (it == byLine.end()) return result;

    for (uint32_t address : it->second) {
      const Entry& e = byAddress.at(address);
      printf("%d & %d => %d ==== %d \n", line, e.line, e.line == line, e.address);
      if (e.line == line) result.push_back(e); // skip entries overwritten by an overlapping ORG
    }

    return result;
  }

  /// Same as atLine, but for comment / blank / label lines: uses the next line that emitted bytes.
  std::vector<Entry> atOrAfterLine(int32_t line) const {
    auto it = byLine.lower_bound(line);
    printf("%d == %d line: %d \n", line, it == byLine.end(), it->first);
    return it == byLine.end() ? std::vector<Entry>{} : atLine(it->first);
  }

  /// All entries ordered by address (handy for dumping a listing file).
  const std::map<uint32_t, Entry>& entries() const { return byAddress; }

// private:
  std::map<uint32_t, Entry> byAddress;                 // start address -> entry
  std::map<int32_t, std::vector<uint32_t>> byLine;     // line -> start addresses
};

};
