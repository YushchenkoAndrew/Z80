#pragma once
#include "Parser.h"

namespace Interpreter {

class Environment {
public:
  Environment(uint32_t a = 0): addr(a), startAt(a) {}
  inline void reset() { addr = startAt; unknown.clear(); memory.clear(); memory.resize(startAt, 0); }

  inline void define(std::string key, MemoryT val) { 
    if (vars->first) return; // if vars is locked
    unknown.remove(key); vars->second[key] = val;
  }

  inline void undefine(std::string key) {
    // if (vars->first) return; // if vars is locked
    unknown.push_back(key); unknown.unique();
  }

  inline bool has(std::string key) { return vars->second.find(key) != vars->second.end(); }
  inline MemoryT get(std::string key, int32_t size) { 
    if (!key.compare("$")) return addr2Bytes();

    if (has(key)) {
      int32_t diff = vars->second[key].size() - size;
      if (diff >= 0) return vars->second[key];

      // NOTE: Resize variable if needed
      MemoryT res(std::abs(diff), 0x00);
      res.insert(res.end(), vars->second[key].begin(), vars->second[key].end());
      return res;
    }

    undefine(key);
    return MemoryT(size, 0x00);
  }

  inline void allocate(MemoryT address) {
    if (address.size() == 1) addr = (uint32_t)address[0];
    else addr = ((uint32_t)address[0] << 8) | (uint32_t)address[1];

    int32_t diff = memory.size() - addr;
    if (diff >= 0) return;

    for (int32_t i = 0; i < std::abs(diff); i++) memory.push_back(0x00);
  }


  void insert(MemoryT bytes) {
    for (uint32_t i = 0; i < bytes.size(); i++, addr++) {
      // uint32_t index = addr + i;

      if (addr < memory.size()) memory[addr] = bytes[i];
      else memory.push_back(bytes[i]);
    }

    // addr += bytes.size();
  }

  void save(std::string path) {
    std::ofstream f; f.open(path);

    for (auto byte : memory) {
      f.write(reinterpret_cast<char*>(&byte), sizeof byte);
    }

    f.close();
  }

  inline void bind(std::string filepath, std::shared_ptr<Token> token) { tokens[addr] = std::pair(filepath, token); }

  inline std::list<std::string> files() {
    std::list<std::string> set;
    for (auto& t : tokens) set.push_back(t.second.first);

    set.unique(); return set;
  }

private:
  inline MemoryT addr2Bytes() { return { (uint8_t)((addr >> 8) & 0xFF), (uint8_t)(addr & 0xFF) }; }

public:
  MemoryT memory;

  uint32_t addr = 0x0000;
  const uint32_t startAt;

  std::shared_ptr<VariableT> vars; // first is shown if vars is locked or not
  std::list<std::string> unknown; // Contains unkown vars

  // TODO: Change key to std::pair where first is line number and second is file path
  std::map<uint32_t, RelativeToken> tokens;
};

};
