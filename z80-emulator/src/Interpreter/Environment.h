#pragma once
#include "Parser.h"

namespace Interpreter {

class Environment {
public:
  Environment(uint32_t a = 0): addr(a), startAt(a) {}

  inline void reset() {
    addr = startAt; memory.clear(); vars.clear(); unknown.clear();
    for (int32_t i = 0; i < startAt; i++) memory.push_back(0x00);
  }

  inline void define(std::string key, MemoryT val) { 
    vars[key] = val;
    if (unknown.find(key) == unknown.end()) return;

    for (auto address : unknown[key]) {
      for (uint32_t i = 0; i < val.size(); i++) {
        memory[address + i] = val[i];
      }
    }

    unknown[key].clear(); unknown.erase(key);
  }

  inline void undefine(std::string key) {
    if (unknown.find(key) == unknown.end()) unknown[key] = {};
    unknown[key].push_back(addr + 1);
  }


  inline bool has(std::string key) { return vars.find(key) != vars.end(); }
  inline MemoryT get(std::string key, int32_t size) { 
    if (!key.compare("$")) return addr2Bytes();
    if (has(key)) {
      int32_t diff = vars[key].size() - size;
      if (diff >= 0) return vars[key];

      MemoryT res(std::abs(diff), 0x00);
      res.insert(res.end(), vars[key].begin(), vars[key].end());
      return res;
    }

    undefine(key);
    return MemoryT(size, 0x00);
  }

  inline void allocate(std::string key, MemoryT address) {
    define(key, address);
    if (address.size() == 1) addr = (uint32_t)address[0];
    else addr = ((uint32_t)address[1] << 8) | (uint32_t)address[0];

    int32_t diff = memory.size() - addr;
    if (diff >= 0) return;

    for (int32_t i = 0; i < std::abs(diff); i++) memory.push_back(0x00);
  }


  void insert(MemoryT bytes) {
    for (uint32_t i = 0; i < bytes.size(); i++) {
      uint32_t index = addr + i;

      if (index < memory.size()) memory[index] = bytes[i];
      else memory.push_back(bytes[i]);
    }

    addr += bytes.size();
  }

  void save(std::string path) {
    std::ofstream f; f.open(path);

    for (auto byte : memory) {
      f.write(reinterpret_cast<char*>(&byte), sizeof byte);
    }

    f.close();
  }

  inline void bind(std::shared_ptr<Token> token) { tokens[addr] = token; }

private:
  inline MemoryT addr2Bytes() { return { (uint8_t)((addr >>  8) & 0xFF), (uint8_t)(addr & 0xFF) }; }

public:
  MemoryT memory;

  uint32_t addr = 0x0000;
  const uint32_t startAt;
  std::unordered_map<std::string, MemoryT> vars;
  std::unordered_map<std::string, std::vector<uint32_t>> unknown; // This map points to places which need to be updated 

  std::unordered_map<uint32_t, std::shared_ptr<Token>> tokens;
};

};
