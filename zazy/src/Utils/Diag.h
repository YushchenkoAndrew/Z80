#pragma once
#include "Utils.h"

namespace Zazy {

/**
 * One place for every error / warning of every stage. Line numbers stored in tokens refer to
 * the preprocessed text, `origins` maps them back to the real file and line.
 */
struct Origin {
  std::string file;
  int32_t line = 0;
};

class Diag {
public:
  static std::vector<Origin>& origins() { static std::vector<Origin> o; return o; }
  static std::vector<std::string>& messages() { static std::vector<std::string> m; return m; }
  static int& errorCount() { static int c = 0; return c; }
  static bool& quiet() { static bool q = false; return q; }

  static void reset() { origins().clear(); messages().clear(); errorCount() = 0; }

  static std::string where(int32_t line, int32_t col) {
    const auto& o = origins();
    if (line >= 1 && line <= (int32_t)o.size()) {
      return o[line - 1].file + ":" + std::to_string(o[line - 1].line) + ":" + std::to_string(col);
    }
    return "line " + std::to_string(line) + ":" + std::to_string(col);
  }

  static void error(int32_t line, int32_t col, const std::string& msg) {
    messages().push_back(where(line, col) + ": error: " + msg);
    errorCount()++;
  }

  static void warning(int32_t line, int32_t col, const std::string& msg) {
    messages().push_back(where(line, col) + ": warning: " + msg);
  }

  static void plain(const std::string& msg) { messages().push_back(msg); }

  static void flush(FILE* out = stderr) {
    for (auto& m : messages()) fprintf(out, "%s\n", m.c_str());
    messages().clear();
  }
};
};
