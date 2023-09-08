#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include "include/Typelist.h"
#include "include/Foreach.h"
#include "lib/olcPixelGameEngine.h"
#include "include/LuaScript.h"

enum Colors {
		GREY, DARK_GREY, VERY_DARK_GREY,
		RED, DARK_RED, VERY_DARK_RED,
		YELLOW, DARK_YELLOW, VERY_DARK_YELLOW,
		GREEN, DARK_GREEN, VERY_DARK_GREEN,
		CYAN, DARK_CYAN, VERY_DARK_CYAN,
		BLUE, DARK_BLUE, VERY_DARK_BLUE,
		MAGENTA, DARK_MAGENTA, VERY_DARK_MAGENTA,
		WHITE, BLACK, BLANK
};

enum ModeT { NORMAL, INSERT };

class Defs {

public:
  static void Init() {
    AnyType<GREY,              std::string>::GetValue() = "GRAY";              AnyType<GREY,              olc::Pixel>::GetValue() = olc::Pixel(192, 192, 192);
    AnyType<DARK_GREY,         std::string>::GetValue() = "DARK_GREY";         AnyType<DARK_GREY,         olc::Pixel>::GetValue() = olc::Pixel(128, 128, 128);
    AnyType<VERY_DARK_GREY,    std::string>::GetValue() = "VERY_DARK_GREY";    AnyType<VERY_DARK_GREY,    olc::Pixel>::GetValue() = olc::Pixel(64, 64, 64);

    AnyType<RED,               std::string>::GetValue() = "RED";               AnyType<RED,               olc::Pixel>::GetValue() = olc::Pixel(255, 0, 0);
    AnyType<DARK_RED,          std::string>::GetValue() = "DARK_RED";          AnyType<DARK_RED,          olc::Pixel>::GetValue() = olc::Pixel(128, 0, 0);
    AnyType<VERY_DARK_RED,     std::string>::GetValue() = "VERY_DARK_RED";     AnyType<VERY_DARK_RED,     olc::Pixel>::GetValue() = olc::Pixel(64, 0, 0);

    AnyType<YELLOW,            std::string>::GetValue() = "YELLOW";            AnyType<YELLOW,            olc::Pixel>::GetValue() = olc::Pixel(255, 255, 0);
    AnyType<DARK_YELLOW,       std::string>::GetValue() = "DARK_YELLOW";       AnyType<DARK_YELLOW,       olc::Pixel>::GetValue() = olc::Pixel(128, 128, 0);
    AnyType<VERY_DARK_YELLOW,  std::string>::GetValue() = "VERY_DARK_YELLOW";  AnyType<VERY_DARK_YELLOW,  olc::Pixel>::GetValue() = olc::Pixel(64, 64, 0);

    AnyType<GREEN,             std::string>::GetValue() = "GREEN";             AnyType<GREEN,             olc::Pixel>::GetValue() = olc::Pixel(0, 255, 0);
    AnyType<DARK_GREEN,        std::string>::GetValue() = "DARK_GREEN";        AnyType<DARK_GREEN,        olc::Pixel>::GetValue() = olc::Pixel(0, 128, 0);
    AnyType<VERY_DARK_GREEN,   std::string>::GetValue() = "VERY_DARK_GREEN";   AnyType<VERY_DARK_GREEN,   olc::Pixel>::GetValue() = olc::Pixel(0, 64, 0);

    AnyType<CYAN,              std::string>::GetValue() = "CYAN";              AnyType<CYAN,              olc::Pixel>::GetValue() = olc::Pixel(0, 255, 255);
    AnyType<DARK_CYAN,         std::string>::GetValue() = "DARK_CYAN";         AnyType<DARK_CYAN,         olc::Pixel>::GetValue() = olc::Pixel(0, 128, 128);
    AnyType<VERY_DARK_CYAN,    std::string>::GetValue() = "VERY_DARK_CYAN";    AnyType<VERY_DARK_CYAN,    olc::Pixel>::GetValue() = olc::Pixel(0, 64, 64);

    AnyType<BLUE,              std::string>::GetValue() = "BLUE";              AnyType<BLUE,              olc::Pixel>::GetValue() = olc::Pixel(0, 0, 255);
    AnyType<DARK_BLUE,         std::string>::GetValue() = "DARK_BLUE";         AnyType<DARK_BLUE,         olc::Pixel>::GetValue() = olc::Pixel(0, 0, 128);
    AnyType<VERY_DARK_BLUE,    std::string>::GetValue() = "VERY_DARK_BLUE";    AnyType<VERY_DARK_BLUE,    olc::Pixel>::GetValue() = olc::Pixel(0, 0, 64);

    AnyType<MAGENTA,           std::string>::GetValue() = "MAGENTA";           AnyType<MAGENTA,           olc::Pixel>::GetValue() = olc::Pixel(255, 0, 255);
    AnyType<DARK_MAGENTA,      std::string>::GetValue() = "DARK_MAGENTA";      AnyType<DARK_MAGENTA,      olc::Pixel>::GetValue() = olc::Pixel(128, 0, 128);
    AnyType<VERY_DARK_MAGENTA, std::string>::GetValue() = "VERY_DARK_MAGENTA"; AnyType<VERY_DARK_MAGENTA, olc::Pixel>::GetValue() = olc::Pixel(64, 0, 64);

    AnyType<WHITE,             std::string>::GetValue() = "WHITE";             AnyType<WHITE,             olc::Pixel>::GetValue() = olc::Pixel(255, 255, 255);
    AnyType<BLACK,             std::string>::GetValue() = "BLACK";             AnyType<BLACK,             olc::Pixel>::GetValue() = olc::Pixel(0, 0, 0);
    AnyType<BLANK,             std::string>::GetValue() = "BLANK";             AnyType<BLANK,             olc::Pixel>::GetValue() = olc::Pixel(0, 0, 0, 0);

    AnyType<-1, ModeT>::GetValue() =  ModeT::NORMAL;
  }

  static void Load(LuaScript* config) {
    // TODO: Update colors

  }
};