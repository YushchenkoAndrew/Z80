#include "Compiler.h"
#include "Repl.h"

using namespace Zazy;

static void usage() {
  printf(
    "zazy - a small C compiler for the Z80 computer\n\n"
    "usage: zazy [options] input.zz\n\n"
    "  -o FILE            write the asm to FILE (default: input with .z80, '-' for stdout)\n"
    "  --bare             no Init.z80: minimal reset code (RST 10h prints to port 0x00), for tests\n"
    "  --init PATH        INCLUDE path of your Init.z80 (default \"lib/Init.z80\")\n"
    "  --data-base ADDR   RAM address of the global variables (default 0x8000)\n"
    "  --no-comments      do not annotate the asm\n"
    "  --math PATH        INCLUDE path of Math.z80 (default lib/utils/Math.z80)\n"
    "  --no-stdlib        do not read zazy.zh (bindings of the hand written routines)\n"
    "  --lib DIR          folder of zazy.zh (default: $ZAZY_LIB, <exe>/lib, ./lib)\n"
    "  -I DIR             #include search folder\n"
    "  -D NAME[=VALUE]    define a macro\n"
    "  --tokens | --ast   dump tokens / typed tree\n"
    "  --repl             interactive shell\n");
}

static std::string findLib(const char* argv0, const std::string& given) {
  namespace fs = std::filesystem;
  std::vector<fs::path> c;
  if (!given.empty()) c.push_back(given);
  if (const char* e = getenv("ZAZY_LIB")) c.push_back(e);
  std::error_code ec;
  fs::path exe = fs::weakly_canonical(fs::path(argv0), ec).parent_path();
  c.push_back(exe / "lib"); c.push_back(exe / ".." / "lib"); c.push_back("lib");
  for (auto& p : c) if (fs::exists(p / "zazy.zh", ec)) return p.string();
  return "";
}

int main(int argc, char** argv) {
  CompileOptions o;
  std::string input, output, libDir;
  bool repl = false, lib = true;

  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];
    auto need = [&](const char* what) -> std::string {
      if (i + 1 >= argc) { fprintf(stderr, "zazy: %s needs a value\n", what); exit(2); }
      return argv[++i];
    };
    if (a == "-h" || a == "--help") { usage(); return 0; }
    else if (a == "-o") output = need("-o");
    else if (a == "--bare") o.cg.bare = true;
    else if (a == "--init") o.cg.initPath = need("--init");
    else if (a == "--math") o.cg.mathPath = need("--math");
    else if (a == "--data-base") o.cg.dataBase = (uint32_t)strtoul(need("--data-base").c_str(), nullptr, 0);
    else if (a == "--no-comments") o.cg.comments = false;
    else if (a == "--no-stdlib") lib = false;
    else if (a == "--lib") libDir = need("--lib");
    else if (a == "--tokens") o.dumpTokens = true;
    else if (a == "--ast") o.dumpAst = true;
    else if (a == "--repl") repl = true;
    else if (a == "-I") o.includeDirs.push_back(need("-I"));
    else if (a.compare(0, 2, "-I") == 0) o.includeDirs.push_back(a.substr(2));
    else if (a == "-D" || a.compare(0, 2, "-D") == 0) {
      std::string d = a == "-D" ? need("-D") : a.substr(2);
      auto eq = d.find('=');
      o.defines.push_back(eq == std::string::npos ? std::make_pair(d, std::string("1")) : std::make_pair(d.substr(0, eq), d.substr(eq + 1)));
    }
    else if (!a.empty() && a[0] == '-' && a != "-") { fprintf(stderr, "zazy: unknown option %s\n", a.c_str()); usage(); return 2; }
    else input = a;
  }

  o.stdlib = lib;
  if (lib) {
    o.libDir = findLib(argv[0], libDir);
    if (o.libDir.empty()) { fprintf(stderr, "zazy: cannot find zazy.zh, use --lib DIR or --no-stdlib\n"); return 2; }
  }

  if (repl) { Repl(o).run(); return 0; }
  if (input.empty()) { usage(); return 2; }

  o.cg.source = std::filesystem::path(input).filename().string();
  std::string asmText;
  if (!Compile(input, nullptr, o, asmText)) return 1;

  if (output.empty()) output = std::filesystem::path(input).replace_extension(".z80").string();
  if (output == "-") printf("%s", asmText.c_str());
  else {
    std::ofstream f(output);
    if (!f) { fprintf(stderr, "zazy: cannot write %s\n", output.c_str()); return 1; }
    f << asmText;
  }
  return 0;
}
