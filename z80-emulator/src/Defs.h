#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <future>
#include "include/Typelist.h"
#include "include/Foreach.h"
#include "lib/olcPixelGameEngine.h"
#include "include/LuaScript.h"

#define HISTORY_SIZE 100

enum Colors {
		GREY, DARK_GREY, VERY_DARK_GREY,
		RED, DARK_RED, VERY_DARK_RED,
		YELLOW, DARK_YELLOW, VERY_DARK_YELLOW,
		ORANGE, DARK_ORANGE, VERY_DARK_ORANGE,
		GREEN, DARK_GREEN, VERY_DARK_GREEN,
		CYAN, DARK_CYAN, VERY_DARK_CYAN,
		BLUE, DARK_BLUE, VERY_DARK_BLUE,
		MAGENTA, DARK_MAGENTA, VERY_DARK_MAGENTA,
		WHITE, BLACK, BLANK
};

typedef std::pair<olc::vi2d, olc::vi2d> DimensionT;
typedef TypeList<
  AnyType<GREY,              std::string>, TypeList<
  AnyType<DARK_GREY,         std::string>, TypeList<
  AnyType<VERY_DARK_GREY,    std::string>, TypeList<

  AnyType<RED,               std::string>, TypeList<
  AnyType<DARK_RED,          std::string>, TypeList<
  AnyType<VERY_DARK_RED,     std::string>, TypeList<

  AnyType<ORANGE,            std::string>, TypeList<
  AnyType<DARK_ORANGE,       std::string>, TypeList<
  AnyType<VERY_DARK_ORANGE,  std::string>, TypeList<

  AnyType<YELLOW,            std::string>, TypeList<
  AnyType<DARK_YELLOW,       std::string>, TypeList<
  AnyType<VERY_DARK_YELLOW,  std::string>, TypeList<

  AnyType<GREEN,             std::string>, TypeList<
  AnyType<DARK_GREEN,        std::string>, TypeList<
  AnyType<VERY_DARK_GREEN,   std::string>, TypeList<

  AnyType<CYAN,              std::string>, TypeList<
  AnyType<DARK_CYAN,         std::string>, TypeList<
  AnyType<VERY_DARK_CYAN,    std::string>, TypeList<

  AnyType<BLUE,              std::string>, TypeList<
  AnyType<DARK_BLUE,         std::string>, TypeList<
  AnyType<VERY_DARK_BLUE,    std::string>, TypeList<

  AnyType<MAGENTA,           std::string>, TypeList<
  AnyType<DARK_MAGENTA,      std::string>, TypeList<
  AnyType<VERY_DARK_MAGENTA, std::string>, TypeList<

  AnyType<WHITE,             std::string>, TypeList<
  AnyType<BLACK,             std::string>, TypeList<
  AnyType<BLANK,             std::string>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>
    ColorNames;

typedef TypeList<
  AnyType<GREY,              olc::Pixel>, TypeList<
  AnyType<DARK_GREY,         olc::Pixel>, TypeList<
  AnyType<VERY_DARK_GREY,    olc::Pixel>, TypeList<

  AnyType<RED,               olc::Pixel>, TypeList<
  AnyType<DARK_RED,          olc::Pixel>, TypeList<
  AnyType<VERY_DARK_RED,     olc::Pixel>, TypeList<

  AnyType<ORANGE,            olc::Pixel>, TypeList<
  AnyType<DARK_ORANGE,       olc::Pixel>, TypeList<
  AnyType<VERY_DARK_ORANGE,  olc::Pixel>, TypeList<

  AnyType<YELLOW,            olc::Pixel>, TypeList<
  AnyType<DARK_YELLOW,       olc::Pixel>, TypeList<
  AnyType<VERY_DARK_YELLOW,  olc::Pixel>, TypeList<

  AnyType<GREEN,             olc::Pixel>, TypeList<
  AnyType<DARK_GREEN,        olc::Pixel>, TypeList<
  AnyType<VERY_DARK_GREEN,   olc::Pixel>, TypeList<

  AnyType<CYAN,              olc::Pixel>, TypeList<
  AnyType<DARK_CYAN,         olc::Pixel>, TypeList<
  AnyType<VERY_DARK_CYAN,    olc::Pixel>, TypeList<

  AnyType<BLUE,              olc::Pixel>, TypeList<
  AnyType<DARK_BLUE,         olc::Pixel>, TypeList<
  AnyType<VERY_DARK_BLUE,    olc::Pixel>, TypeList<

  AnyType<MAGENTA,           olc::Pixel>, TypeList<
  AnyType<DARK_MAGENTA,      olc::Pixel>, TypeList<
  AnyType<VERY_DARK_MAGENTA, olc::Pixel>, TypeList<

  AnyType<WHITE,             olc::Pixel>, TypeList<
  AnyType<BLACK,             olc::Pixel>, TypeList<
  AnyType<BLANK,             olc::Pixel>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>
    ColorList;

class Defs {

public:
  static void Init() {
    AnyType<GREY,              std::string>::GetValue() = "GRAY";              AnyType<GREY,              olc::Pixel>::GetValue() = olc::Pixel(0xAC, 0xB6, 0xC7);
    AnyType<DARK_GREY,         std::string>::GetValue() = "DARK_GREY";         AnyType<DARK_GREY,         olc::Pixel>::GetValue() = olc::Pixel(0x3A, 0x45, 0x56);
    AnyType<VERY_DARK_GREY,    std::string>::GetValue() = "VERY_DARK_GREY";    AnyType<VERY_DARK_GREY,    olc::Pixel>::GetValue() = olc::Pixel(0x18, 0x23, 0x2D);

    AnyType<RED,               std::string>::GetValue() = "RED";               AnyType<RED,               olc::Pixel>::GetValue() = olc::Pixel(0XFF, 0x86, 0x63);
    AnyType<DARK_RED,          std::string>::GetValue() = "DARK_RED";          AnyType<DARK_RED,          olc::Pixel>::GetValue() = olc::Pixel(128, 0, 0);
    AnyType<VERY_DARK_RED,     std::string>::GetValue() = "VERY_DARK_RED";     AnyType<VERY_DARK_RED,     olc::Pixel>::GetValue() = olc::Pixel(64, 0, 0);

    AnyType<ORANGE,            std::string>::GetValue() = "ORANGE";            AnyType<ORANGE,            olc::Pixel>::GetValue() = olc::Pixel(0xE0, 0x9C, 0x5F);
    AnyType<DARK_ORANGE,       std::string>::GetValue() = "DARK_ORANGE";       AnyType<DARK_ORANGE,       olc::Pixel>::GetValue() = olc::Pixel(0xD3, 0x7D, 0x09);
    AnyType<VERY_DARK_ORANGE,  std::string>::GetValue() = "VERY_DARK_ORANGE";  AnyType<VERY_DARK_ORANGE,  olc::Pixel>::GetValue() = olc::Pixel(64, 32, 0);

    AnyType<YELLOW,            std::string>::GetValue() = "YELLOW";            AnyType<YELLOW,            olc::Pixel>::GetValue() = olc::Pixel(0xEC, 0xCE, 0x8A);
    AnyType<DARK_YELLOW,       std::string>::GetValue() = "DARK_YELLOW";       AnyType<DARK_YELLOW,       olc::Pixel>::GetValue() = olc::Pixel(0xD7, 0xAD, 0x5D);
    AnyType<VERY_DARK_YELLOW,  std::string>::GetValue() = "VERY_DARK_YELLOW";  AnyType<VERY_DARK_YELLOW,  olc::Pixel>::GetValue() = olc::Pixel(64, 64, 0);

    AnyType<GREEN,             std::string>::GetValue() = "GREEN";             AnyType<GREEN,             olc::Pixel>::GetValue() = olc::Pixel(0, 255, 0);
    AnyType<DARK_GREEN,        std::string>::GetValue() = "DARK_GREEN";        AnyType<DARK_GREEN,        olc::Pixel>::GetValue() = olc::Pixel(0, 128, 0);
    AnyType<VERY_DARK_GREEN,   std::string>::GetValue() = "VERY_DARK_GREEN";   AnyType<VERY_DARK_GREEN,   olc::Pixel>::GetValue() = olc::Pixel(0, 64, 0);

    AnyType<CYAN,              std::string>::GetValue() = "CYAN";              AnyType<CYAN,              olc::Pixel>::GetValue() = olc::Pixel(0x2E, 0xBF, 0xD5);
    AnyType<DARK_CYAN,         std::string>::GetValue() = "DARK_CYAN";         AnyType<DARK_CYAN,         olc::Pixel>::GetValue() = olc::Pixel(0, 128, 128);
    AnyType<VERY_DARK_CYAN,    std::string>::GetValue() = "VERY_DARK_CYAN";    AnyType<VERY_DARK_CYAN,    olc::Pixel>::GetValue() = olc::Pixel(0, 64, 64);

    AnyType<BLUE,              std::string>::GetValue() = "BLUE";              AnyType<BLUE,              olc::Pixel>::GetValue() = olc::Pixel(0x4C, 0xB8, 0xFE);
    AnyType<DARK_BLUE,         std::string>::GetValue() = "DARK_BLUE";         AnyType<DARK_BLUE,         olc::Pixel>::GetValue() = olc::Pixel(0, 0, 128);
    AnyType<VERY_DARK_BLUE,    std::string>::GetValue() = "VERY_DARK_BLUE";    AnyType<VERY_DARK_BLUE,    olc::Pixel>::GetValue() = olc::Pixel(0, 0, 64);

    AnyType<MAGENTA,           std::string>::GetValue() = "MAGENTA";           AnyType<MAGENTA,           olc::Pixel>::GetValue() = olc::Pixel(0xCC, 0x75, 0xEC);
    AnyType<DARK_MAGENTA,      std::string>::GetValue() = "DARK_MAGENTA";      AnyType<DARK_MAGENTA,      olc::Pixel>::GetValue() = olc::Pixel(128, 0, 128);
    AnyType<VERY_DARK_MAGENTA, std::string>::GetValue() = "VERY_DARK_MAGENTA"; AnyType<VERY_DARK_MAGENTA, olc::Pixel>::GetValue() = olc::Pixel(64, 0, 64);

    AnyType<WHITE,             std::string>::GetValue() = "WHITE";             AnyType<WHITE,             olc::Pixel>::GetValue() = olc::Pixel(0xDB, 0xE5, 0xD5);
    AnyType<BLACK,             std::string>::GetValue() = "BLACK";             AnyType<BLACK,             olc::Pixel>::GetValue() = olc::Pixel(0x14, 0x1A, 0x1F);
    AnyType<BLANK,             std::string>::GetValue() = "BLANK";             AnyType<BLANK,             olc::Pixel>::GetValue() = olc::Pixel(0, 0, 0, 0);
  }

  static void Load(LuaScript* luaConfig) {
    // TODO: Update colors
    // luaConfig->GetTableValue<bool>(nullptr, "sColors");

    // // printf("%d\n", luaConfig->Size());
    // luaConfig->Iterate(nullptr, [&]() {
    //   AnyType<-1, std::string>::GetValue() = luaConfig->GetValue(Type2Type<const char *>());
    //   int32_t index = foreach<ColorNames, AnyType<-1, std::string>>::Value2Key();
    //   luaConfig->Pop();

    //   printf("%s\n", AnyType<-1, std::string>::GetValue().c_str());

    //   // AnyType<-1, int32_t>::GetValue() = index;
    //   // auto t = foreach<ColorList, AnyType<-1, int32_t>>::Key2Value();
    //   // t.g = 255;
    //   AnyType<RED, olc::Pixel>::GetValue() = olc::Pixel(245, 100, 100);

    //   // printf("%d %d %d\n", t.b, t.g, t.r);

    //   luaConfig->Pop();
    // });

    // luaConfig->Pop();

    // const int32_t nScreenWidth = luaConfig.GetTableValue<int32_t>(nullptr, "nScreenWidth");
    // const int32_t nScreenHeight = luaConfig.GetTableValue<int32_t>(nullptr, "nScreenHeight");
    // const int32_t nPixel = luaConfig.GetTableValue<int32_t>(nullptr, "nPixel");

  }
};

enum Events {
  EDITOR_CALLBACK, MEMORY_CALLBACK,

  MEMORY_SELECT_CALLBACK, PANEL_SELECT_CALLBACK,
};

class PixelGameEngine : public olc::PixelGameEngine {
  public:

  virtual void Event(Int2Type<EDITOR_CALLBACK>) = 0;
  virtual void Event(Int2Type<MEMORY_CALLBACK>) = 0;

  virtual void Event(Int2Type<PANEL_SELECT_CALLBACK>, int32_t) = 0;
  virtual void Event(Int2Type<MEMORY_SELECT_CALLBACK>, int32_t) = 0;
};


