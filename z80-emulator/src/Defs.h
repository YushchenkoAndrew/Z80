#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <future>
#include "include/Typelist.h"
#include "include/Foreach.h"
#include "lib/olcPixelGameEngine.h"
#include "include/LuaScript.h"

/**
 * Overwrite TypeList from Typelist.h to have ability to save value
 */
template<class T>
struct TypeList<T, olc::Pixel> {
	olc::Pixel val;
	inline TypeList() {}

  olc::Pixel& operator = (olc::Pixel c) { return val = c; }
  template<class U> olc::Pixel& operator = (TypeList<U, olc::Pixel>& list) { return val = list.val; }

  olc::Pixel& operator ~ () { return val; }
};

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
typedef TypeList<Colors, std::string> ColorSTR;
typedef TypeList<Colors, olc::Pixel> ColorT;
typedef TypeList<
  AnyType<GREY,              ColorSTR>, TypeList<
  AnyType<DARK_GREY,         ColorSTR>, TypeList<
  AnyType<VERY_DARK_GREY,    ColorSTR>, TypeList<

  AnyType<RED,               ColorSTR>, TypeList<
  AnyType<DARK_RED,          ColorSTR>, TypeList<
  AnyType<VERY_DARK_RED,     ColorSTR>, TypeList<

  AnyType<ORANGE,            ColorSTR>, TypeList<
  AnyType<DARK_ORANGE,       ColorSTR>, TypeList<
  AnyType<VERY_DARK_ORANGE,  ColorSTR>, TypeList<

  AnyType<YELLOW,            ColorSTR>, TypeList<
  AnyType<DARK_YELLOW,       ColorSTR>, TypeList<
  AnyType<VERY_DARK_YELLOW,  ColorSTR>, TypeList<

  AnyType<GREEN,             ColorSTR>, TypeList<
  AnyType<DARK_GREEN,        ColorSTR>, TypeList<
  AnyType<VERY_DARK_GREEN,   ColorSTR>, TypeList<

  AnyType<CYAN,              ColorSTR>, TypeList<
  AnyType<DARK_CYAN,         ColorSTR>, TypeList<
  AnyType<VERY_DARK_CYAN,    ColorSTR>, TypeList<

  AnyType<BLUE,              ColorSTR>, TypeList<
  AnyType<DARK_BLUE,         ColorSTR>, TypeList<
  AnyType<VERY_DARK_BLUE,    ColorSTR>, TypeList<

  AnyType<MAGENTA,           ColorSTR>, TypeList<
  AnyType<DARK_MAGENTA,      ColorSTR>, TypeList<
  AnyType<VERY_DARK_MAGENTA, ColorSTR>, TypeList<

  AnyType<WHITE,             ColorSTR>, TypeList<
  AnyType<BLACK,             ColorSTR>, TypeList<
  AnyType<BLANK,             ColorSTR>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>
    ColorNames;

typedef TypeList<
  AnyType<GREY,              ColorT>, TypeList<
  AnyType<DARK_GREY,         ColorT>, TypeList<
  AnyType<VERY_DARK_GREY,    ColorT>, TypeList<

  AnyType<RED,               ColorT>, TypeList<
  AnyType<DARK_RED,          ColorT>, TypeList<
  AnyType<VERY_DARK_RED,     ColorT>, TypeList<

  AnyType<ORANGE,            ColorT>, TypeList<
  AnyType<DARK_ORANGE,       ColorT>, TypeList<
  AnyType<VERY_DARK_ORANGE,  ColorT>, TypeList<

  AnyType<YELLOW,            ColorT>, TypeList<
  AnyType<DARK_YELLOW,       ColorT>, TypeList<
  AnyType<VERY_DARK_YELLOW,  ColorT>, TypeList<

  AnyType<GREEN,             ColorT>, TypeList<
  AnyType<DARK_GREEN,        ColorT>, TypeList<
  AnyType<VERY_DARK_GREEN,   ColorT>, TypeList<

  AnyType<CYAN,              ColorT>, TypeList<
  AnyType<DARK_CYAN,         ColorT>, TypeList<
  AnyType<VERY_DARK_CYAN,    ColorT>, TypeList<

  AnyType<BLUE,              ColorT>, TypeList<
  AnyType<DARK_BLUE,         ColorT>, TypeList<
  AnyType<VERY_DARK_BLUE,    ColorT>, TypeList<

  AnyType<MAGENTA,           ColorT>, TypeList<
  AnyType<DARK_MAGENTA,      ColorT>, TypeList<
  AnyType<VERY_DARK_MAGENTA, ColorT>, TypeList<

  AnyType<WHITE,             ColorT>, TypeList<
  AnyType<BLACK,             ColorT>, TypeList<
  AnyType<BLANK,             ColorT>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>
    ColorList;

class Defs {

public:
  static void Init() {
    AnyType<GREY,              ColorSTR>::GetValue() = "GRAY";              AnyType<GREY,              ColorT>::GetValue() = olc::Pixel(0xAC, 0xB6, 0xC7);
    AnyType<DARK_GREY,         ColorSTR>::GetValue() = "DARK_GREY";         AnyType<DARK_GREY,         ColorT>::GetValue() = olc::Pixel(0x3A, 0x45, 0x56);
    AnyType<VERY_DARK_GREY,    ColorSTR>::GetValue() = "VERY_DARK_GREY";    AnyType<VERY_DARK_GREY,    ColorT>::GetValue() = olc::Pixel(0x18, 0x23, 0x2D);

    AnyType<RED,               ColorSTR>::GetValue() = "RED";               AnyType<RED,               ColorT>::GetValue() = olc::Pixel(0XFF, 0x86, 0x63);
    AnyType<DARK_RED,          ColorSTR>::GetValue() = "DARK_RED";          AnyType<DARK_RED,          ColorT>::GetValue() = olc::Pixel(128, 0, 0);
    AnyType<VERY_DARK_RED,     ColorSTR>::GetValue() = "VERY_DARK_RED";     AnyType<VERY_DARK_RED,     ColorT>::GetValue() = olc::Pixel(64, 0, 0);

    AnyType<ORANGE,            ColorSTR>::GetValue() = "ORANGE";            AnyType<ORANGE,            ColorT>::GetValue() = olc::Pixel(0xE0, 0x9C, 0x5F);
    AnyType<DARK_ORANGE,       ColorSTR>::GetValue() = "DARK_ORANGE";       AnyType<DARK_ORANGE,       ColorT>::GetValue() = olc::Pixel(0xD3, 0x7D, 0x09);
    AnyType<VERY_DARK_ORANGE,  ColorSTR>::GetValue() = "VERY_DARK_ORANGE";  AnyType<VERY_DARK_ORANGE,  ColorT>::GetValue() = olc::Pixel(0x9D, 0x02, 0x08);

    AnyType<YELLOW,            ColorSTR>::GetValue() = "YELLOW";            AnyType<YELLOW,            ColorT>::GetValue() = olc::Pixel(0xEC, 0xCE, 0x8A);
    AnyType<DARK_YELLOW,       ColorSTR>::GetValue() = "DARK_YELLOW";       AnyType<DARK_YELLOW,       ColorT>::GetValue() = olc::Pixel(0xD7, 0xAD, 0x5D);
    AnyType<VERY_DARK_YELLOW,  ColorSTR>::GetValue() = "VERY_DARK_YELLOW";  AnyType<VERY_DARK_YELLOW,  ColorT>::GetValue() = olc::Pixel(64, 64, 0);

    AnyType<GREEN,             ColorSTR>::GetValue() = "GREEN";             AnyType<GREEN,             ColorT>::GetValue() = olc::Pixel(0x00, 0xC9, 0x5A);
    AnyType<DARK_GREEN,        ColorSTR>::GetValue() = "DARK_GREEN";        AnyType<DARK_GREEN,        ColorT>::GetValue() = olc::Pixel(0x26, 0xA8, 0x52);
    AnyType<VERY_DARK_GREEN,   ColorSTR>::GetValue() = "VERY_DARK_GREEN";   AnyType<VERY_DARK_GREEN,   ColorT>::GetValue() = olc::Pixel(0, 64, 0);

    AnyType<CYAN,              ColorSTR>::GetValue() = "CYAN";              AnyType<CYAN,              ColorT>::GetValue() = olc::Pixel(0x2E, 0xBF, 0xD5);
    AnyType<DARK_CYAN,         ColorSTR>::GetValue() = "DARK_CYAN";         AnyType<DARK_CYAN,         ColorT>::GetValue() = olc::Pixel(0, 128, 128);
    AnyType<VERY_DARK_CYAN,    ColorSTR>::GetValue() = "VERY_DARK_CYAN";    AnyType<VERY_DARK_CYAN,    ColorT>::GetValue() = olc::Pixel(0, 64, 64);

    AnyType<BLUE,              ColorSTR>::GetValue() = "BLUE";              AnyType<BLUE,              ColorT>::GetValue() = olc::Pixel(0x4C, 0xB8, 0xFE);
    AnyType<DARK_BLUE,         ColorSTR>::GetValue() = "DARK_BLUE";         AnyType<DARK_BLUE,         ColorT>::GetValue() = olc::Pixel(0, 0, 128);
    AnyType<VERY_DARK_BLUE,    ColorSTR>::GetValue() = "VERY_DARK_BLUE";    AnyType<VERY_DARK_BLUE,    ColorT>::GetValue() = olc::Pixel(0, 0, 64);

    AnyType<MAGENTA,           ColorSTR>::GetValue() = "MAGENTA";           AnyType<MAGENTA,           ColorT>::GetValue() = olc::Pixel(0xCC, 0x75, 0xEC);
    AnyType<DARK_MAGENTA,      ColorSTR>::GetValue() = "DARK_MAGENTA";      AnyType<DARK_MAGENTA,      ColorT>::GetValue() = olc::Pixel(128, 0, 128);
    AnyType<VERY_DARK_MAGENTA, ColorSTR>::GetValue() = "VERY_DARK_MAGENTA"; AnyType<VERY_DARK_MAGENTA, ColorT>::GetValue() = olc::Pixel(64, 0, 64);

    AnyType<WHITE,             ColorSTR>::GetValue() = "WHITE";             AnyType<WHITE,             ColorT>::GetValue() = olc::Pixel(0xDB, 0xE5, 0xD5);
    AnyType<BLACK,             ColorSTR>::GetValue() = "BLACK";             AnyType<BLACK,             ColorT>::GetValue() = olc::Pixel(0x14, 0x1A, 0x1F);
    AnyType<BLANK,             ColorSTR>::GetValue() = "BLANK";             AnyType<BLANK,             ColorT>::GetValue() = olc::Pixel(0, 0, 0, 0);
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


