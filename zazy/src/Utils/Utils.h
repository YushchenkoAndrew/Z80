#pragma once
#include <memory>
#include <future>
#include <string>
#include <string.h>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <deque>
#include <functional>
#include <algorithm>
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include "include/Typelist.h"
#include "include/Foreach.h"

namespace Utils {

  inline bool IsDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool IsBinDigit(const char &c) { return c == '0' || c == '1'; }
  inline bool IsOctDigit(const char &c) { return c >= '0' && c <= '7'; }
  inline bool IsHexDigit(const char &c) { return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
  inline bool IsAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
  inline bool IsAlphaDigit(const char &c) { return IsAlpha(c) || IsDigit(c); }
  inline std::string Uppercase(std::string& src) { std::string dst; for (auto& c : src) dst += toupper(c); return dst; }
  inline std::string Upper(std::string src) { for (auto& c : src) c = toupper(c); return src; }

  inline uint8_t Hex2Int(const char c) {
    if (IsDigit(c)) return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 0x0A;
    if (c >= 'A' && c <= 'F') return c - 'A' + 0x0A;
    return 0;
  }

  inline std::string Hex(uint32_t v, int width = 2) {
    char buf[16]; snprintf(buf, sizeof(buf), "0x%0*X", width, v);
    return buf;
  }

  inline std::string Trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    return b == std::string::npos ? "" : s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
  }

  inline std::string ReadFile(const std::filesystem::path& p, bool* ok = nullptr) {
    std::ifstream f(p, std::ios::binary);
    if (ok) *ok = (bool)f;
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
  }
};
using namespace Utils;
