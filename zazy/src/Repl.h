#pragma once
#include "Compiler.h"

namespace Zazy {

/**
 * Tiny interactive shell. Every entered chunk is compiled as a whole translation unit and the
 * asm is printed. A chunk ends with an empty line, so multi line functions can be typed in.
 *
 *   \l   toggle the token dump       \p   toggle the typed AST dump
 *   \a   toggle the asm dump         \q   quit
 */
class Repl {
public:
  Repl(CompileOptions o): opt(o) { opt.cg.bare = true; opt.cg.source = "<repl>"; }

  void run() {
    printf("zazy REPL (--bare). Finish a chunk with an empty line. \\l tokens  \\p ast  \\a asm  \\q quit\n");
    bool showAsm = true;
    std::string chunk, line;
    for (;;) {
      printf(chunk.empty() ? ">> " : ".. ");
      fflush(stdout);
      if (!std::getline(std::cin, line)) break;
      std::string t = Trim(line);
      if (chunk.empty() && !t.empty() && t[0] == '\\') {
        if (t == "\\q") break;
        if (t == "\\l") { opt.dumpTokens = !opt.dumpTokens; printf("tokens %s\n", opt.dumpTokens ? "on" : "off"); }
        else if (t == "\\p") { opt.dumpAst = !opt.dumpAst; printf("ast %s\n", opt.dumpAst ? "on" : "off"); }
        else if (t == "\\a") { showAsm = !showAsm; printf("asm %s\n", showAsm ? "on" : "off"); }
        else printf("unknown command\n");
        continue;
      }
      if (!t.empty()) { chunk += line + "\n"; continue; }
      if (chunk.empty()) continue;
      std::string asmText;
      if (Compile("<repl>", &chunk, opt, asmText) && showAsm) printf("%s", asmText.c_str());
      chunk.clear();
    }
  }

private:
  CompileOptions opt;
};
};
