#pragma once
#include "Parser.h"

namespace Interpreter {

typedef std::vector<uint8_t> MemoryT;

class Environment {
public:
  inline void reset() { addr = 0x0000; memory.clear(); vars.clear(); unknown.clear(); }

  inline void define(std::string key, MemoryT val) { 
    vars[key] = val;
    if (unknown.find(key) == unknown.end()) return;

    for (auto address : unknown[key]) {
      for (uint32_t i = 0; i < val.size(); i++) {
        memory[address - i] = val[val.size() - i - 1];
      }
    }

    unknown[key].clear(); unknown.erase(key);
  }

  inline void undefine(std::string key) {
    if (unknown.find(key) == unknown.end()) unknown[key] = {};
    unknown[key].push_back(addr + 1);
  }


  inline bool has(std::string key) { return vars.find(key) != vars.end(); }
  inline MemoryT get(std::string key) { 
    if (!key.compare("$")) return addr2Bytes();
    if (has(key)) return vars[key];
    return { 0x00, 0x00 };
  }

  inline void allocate(std::string key, MemoryT address) {
    define(key, address);
    if (address.size() == 1) addr = (uint32_t)address[0];
    else addr = ((uint32_t)address[1] << 8) | (uint32_t)address[0];

    int32_t diff = memory.size() - addr;
    if (diff >= 0) return;

    for (int32_t i = 0; i < std::abs(diff); i++) memory.push_back(0x00);
  }


  inline void insert(MemoryT bytes) {
    for (uint32_t i = 0; i < bytes.size(); i++) {
      uint32_t index = addr + i;

      if (index < memory.size()) memory[index] = bytes[i];
      else memory.push_back(bytes[i]);
    }
  }

private:
  inline MemoryT addr2Bytes() { return { (uint8_t)((addr >>  8) & 0xFF), (uint8_t)(addr & 0xFF) }; }

public:
  MemoryT memory;

  uint32_t addr = 0x0000;
  std::unordered_map<std::string, MemoryT> vars;
  std::unordered_map<std::string, std::vector<uint32_t>> unknown; // This map points to places which need to be updated 
};

};
