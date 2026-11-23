#pragma once

#include "src/Utils/Utils.h"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace Editor {

class Reader {
public:
  static std::string Read(const std::filesystem::path& path) {
    auto ext = path.extension().string();

    if (ext == ".bin" || ext == ".rom") return Binary(path);

    std::ifstream file(path, std::ios::binary);
    if (!file) return {};

    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
  }

private:
  static std::string Binary(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};

    uint8_t byte; int32_t index = 0; std::string result = "      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"; 
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
      if ((index & 0x0F) == 0) result += "\n" + Utils::Int2Hex(index, 5);
      result += " " + Utils::Int2Hex(byte); index++;
    }


    printf("%s\n", result.c_str());
    return result;
  }
};

};
