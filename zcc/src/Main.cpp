#include <stdio.h>

#define DEBUG_MODE
// #include "Lexer/Lexer.h"
#include "Parser/Parser.h"

// Check Memory Leaking
//#define MEM_TRACK
//#include "include/MemTrack.h"

void error(const std::string& prefix, std::vector<std::string>& errors) {
  for (auto& msg : errors) printf("[%s] %s", prefix.c_str(), msg.c_str());
}

int main() {
  Zcc::Defs::Init(); // Initialize types

  // filedir = std::filesystem::path(path).remove_filename();
  // filepath = std::filesystem::canonical(path);
  // filename = std::filesystem::path(path).filename();

  Zcc::Preprocessor p("assets/Main.c");
  if (p.scan()) error(p.alias, p.errors);

  #ifdef DEBUG_MODE
  p.debug();
  #endif

  Zcc::Lexer l(p.src);
  if (l.scan()) error("LEXER", l.errors);

  #ifdef DEBUG_MODE
  l.debug();
  #endif

  Zcc::Parser pr(l.tokens);
  // auto expr = pr.expression();
  if (pr.scan()) error("PARSER", pr.errors);

  Zcc::Disassemble d;

  if (pr.temp32 == nullptr) printf("NULLPTR\n");
  else d.scan(pr.temp32);

  // for (auto err : p.errors) printf("[ERROR]: %s", err.c_str());

  // return scan((buf.str()));

  // LuaScript luaConfig;
  // if (!luaConfig.Init("src/lua/Config.lua")) return 0;
  // if (!luaConfig.GetTable("Init")) return 0;

  // // TODO: Create defs to load colors from lua
  // // Defs::Load(&luaConfig);

  // const int32_t nScreenWidth = luaConfig.GetTableValue<int32_t>(nullptr, "width");
  // const int32_t nScreenHeight = luaConfig.GetTableValue<int32_t>(nullptr, "height");
  // const int32_t nPixel = luaConfig.GetTableValue<int32_t>(nullptr, "pixel");

  // App app(luaConfig);
  // if (app.Construct(nScreenWidth, nScreenHeight, nPixel, nPixel)) app.Start();
  
  return 0;
}
