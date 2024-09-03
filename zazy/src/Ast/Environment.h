#pragma once
#include "src/Parser/Parser.h"

namespace Zazy {

class Environment;

typedef std::unique_ptr<Environment> env_t;

class Environment : public Visitor {
public:
  Environment(env_t parent = nullptr):
    parent(std::move(parent)) {}


  inline void set(std::string label, obj_t val) {
    env[label] = val;
  }


  inline auto move() { return std::move(parent); }
  inline bool has(std::string& label) { return env.find(label) != env.end(); }

  inline obj_t get(std::string label) {
    if (has(label)) return env[label];
    if (parent != nullptr) return parent->get(label);

    return nullptr;
  }

private:
  env_t parent;
  std::unordered_map<std::string, obj_t> env;
};
}