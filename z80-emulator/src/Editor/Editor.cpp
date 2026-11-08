#include "Editor.h"
#include "src/Registry.h"

namespace Editor {
  std::string Vim::definitionOf(const std::string& name) { return Registry::Interpreter->definitionOf(name); }
  std::optional<LocationT> Vim::definitionLocation(const std::string& name) { return Registry::Interpreter->definitionLocation(name); }
};
