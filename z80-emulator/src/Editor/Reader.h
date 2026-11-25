#pragma once

#include "src/Editor/Highlighter/Binary.h"
#include "src/Editor/Highlighter/Defs.h"
#include "src/Editor/Highlighter/Highlighter.h"
#include "src/Interpreter/Lexer.h"
#include "src/Utils/Utils.h"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

namespace Editor {

class Reader {
public:
  static std::pair<std::string, std::unique_ptr<Syntax::Highlighter>> Read(const std::filesystem::path& path) {
    auto ext = path.extension().string();

    if (ext == ".png") return { Png(path), nullptr };
    if (ext == ".asm" || ext == ".z80") return { Text(path), std::make_unique<Interpreter::Lexer>() };
    if (ext == ".bin" || ext == ".rom") return { Binary(path), std::make_unique<Syntax::Binary>()};

    return { Text(path), nullptr };
  }

private:
  static std::string Text(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};

    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
  }

  static std::string Binary(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};

    uint8_t byte; int32_t index = 0; std::string result = Syntax::Header::BINARY; 
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
      if ((index & 0x0F) == 0) result += "\n" + Utils::Int2Hex(index, 5);
      result += " " + Utils::Int2Hex(byte); index++;
    }

    return result;
  }

  static std::string Png(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};

    uint8_t byte; int32_t index = 0; std::string result = Syntax::Header::BINARY; 
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
      if ((index & 0x0F) == 0) result += "\n" + Utils::Int2Hex(index, 5);
      result += " " + Utils::Int2Hex(byte); index++;
    }

    return result;
  }
};

};
