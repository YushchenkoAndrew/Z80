#pragma once
#include "src/Parser/Parser.h"

namespace Zazy {

class Environment;

typedef std::unique_ptr<Environment> env_t;

class Environment : public Visitor {
public:
  Environment(env_t parent = nullptr):
    parent(std::move(parent)) {}


  inline void set(std::string label, token_t type, obj_t val) {
    env[label] = std::pair(type, val);
  }

  inline obj_t get(std::string label) { return extract(label).second; }
  inline token_t type(std::string label) { return extract(label).first; }

  inline auto move() { return std::move(parent); }
  inline bool has(std::string& label) { return env.find(label) != env.end(); }

private:
  inline std::pair<token_t, obj_t> extract(std::string label) {
    if (has(label)) return env[label];
    if (parent != nullptr) return parent->extract(label);

    return std::pair(nullptr, nullptr);
  }

private:
  env_t parent;
  std::unordered_map<std::string, std::pair<token_t, obj_t>> env;
};
}