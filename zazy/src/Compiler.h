#pragma once
#include "src/Preprocessor/Preprocessor.h"
#include "src/Parser/Parser.h"
#include "src/Parser/Printer.h"
#include "src/Sema/Sema.h"
#include "src/CodeGen/CodeGen.h"

namespace Zazy {

struct CompileOptions {
  CodeGen::Options cg;
  bool stdlib = true;
  std::string libDir;                       // folder with zazy.zh
  std::vector<std::string> includeDirs;
  std::vector<std::pair<std::string, std::string>> defines;
  bool dumpTokens = false;
  bool dumpAst = false;
};

/** preprocess -> parse -> sema -> codegen. Returns false after printing the diagnostics. */
inline bool Compile(const std::string& name, const std::string* text, const CompileOptions& o, std::string& asmOut) {
  Diag::reset();
  Preprocessor pp;
  for (auto& d : o.includeDirs) pp.includeDirs.push_back(d);
  if (o.stdlib && !o.libDir.empty()) {
    pp.includeDirs.push_back(o.libDir);
    pp.preludes.push_back((std::filesystem::path(o.libDir) / "zazy.zh").string());
  }
  for (auto& d : o.defines) pp.define(d.first, d.second);
  if (o.cg.bare) pp.define("__BARE__", "1");

  std::string pre;
  bool ok = text ? pp.processText(name, *text, pre) : pp.processFile(name, pre);
  if (!ok) { Diag::flush(); return false; }

  if (o.dumpTokens) {
    Lexer lx(pre);
    Token::header();
    for (;;) { auto t = lx.next(); t->print(); if (t->token == T_EOF) break; }
  }

  Parser parser(pre);
  auto prog = parser.parse();
  if (Diag::errorCount()) { Diag::flush(); return false; }

  Sema sema;
  if (!sema.run(prog)) { Diag::flush(); return false; }
  if (o.dumpAst) Printer::print(prog);

  try {
    CodeGen cg(sema, o.cg);
    asmOut = cg.generate(prog, pp.asmIncludes);
  } catch (std::exception& e) {
    Diag::plain(e.what());
    Diag::errorCount()++;
    Diag::flush();
    return false;
  }
  Diag::flush();     // warnings
  return true;
}

};
