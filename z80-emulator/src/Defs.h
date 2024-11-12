#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include <future>
#include "include/Typelist.h"
#include "include/Foreach.h"
#include "lib/olcPixelGameEngine.h"
#include "lib/olcPGEX_Sound.h"
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

  olc::Pixel& operator* () { return val; }
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

typedef TypeList<
  TypeList<Int2Type<olc::Key::Q>, Int2Type<'q'>>, TypeList<
  TypeList<Int2Type<olc::Key::W>, Int2Type<'w'>>, TypeList<
  TypeList<Int2Type<olc::Key::E>, Int2Type<'e'>>, TypeList<
  TypeList<Int2Type<olc::Key::R>, Int2Type<'r'>>, TypeList<
  TypeList<Int2Type<olc::Key::T>, Int2Type<'t'>>, TypeList<
  TypeList<Int2Type<olc::Key::Y>, Int2Type<'y'>>, TypeList<
  TypeList<Int2Type<olc::Key::U>, Int2Type<'u'>>, TypeList<
  TypeList<Int2Type<olc::Key::I>, Int2Type<'i'>>, TypeList<
  TypeList<Int2Type<olc::Key::O>, Int2Type<'o'>>, TypeList<
  TypeList<Int2Type<olc::Key::P>, Int2Type<'p'>>, TypeList<
  TypeList<Int2Type<olc::Key::A>, Int2Type<'a'>>, TypeList<
  TypeList<Int2Type<olc::Key::S>, Int2Type<'s'>>, TypeList<
  TypeList<Int2Type<olc::Key::D>, Int2Type<'d'>>, TypeList<
  TypeList<Int2Type<olc::Key::F>, Int2Type<'f'>>, TypeList<
  TypeList<Int2Type<olc::Key::G>, Int2Type<'g'>>, TypeList<
  TypeList<Int2Type<olc::Key::H>, Int2Type<'h'>>, TypeList<
  TypeList<Int2Type<olc::Key::J>, Int2Type<'j'>>, TypeList<
  TypeList<Int2Type<olc::Key::K>, Int2Type<'k'>>, TypeList<
  TypeList<Int2Type<olc::Key::L>, Int2Type<'l'>>, TypeList<
  TypeList<Int2Type<olc::Key::Z>, Int2Type<'z'>>, TypeList<
  TypeList<Int2Type<olc::Key::X>, Int2Type<'x'>>, TypeList<
  TypeList<Int2Type<olc::Key::C>, Int2Type<'c'>>, TypeList<
  TypeList<Int2Type<olc::Key::V>, Int2Type<'v'>>, TypeList<
  TypeList<Int2Type<olc::Key::B>, Int2Type<'b'>>, TypeList<
  TypeList<Int2Type<olc::Key::N>, Int2Type<'n'>>, TypeList<
  TypeList<Int2Type<olc::Key::M>, Int2Type<'m'>>, TypeList<

  TypeList<Int2Type<olc::Key::K0>, Int2Type<'0'>>, TypeList<
  TypeList<Int2Type<olc::Key::K1>, Int2Type<'1'>>, TypeList<
  TypeList<Int2Type<olc::Key::K2>, Int2Type<'2'>>, TypeList<
  TypeList<Int2Type<olc::Key::K3>, Int2Type<'3'>>, TypeList<
  TypeList<Int2Type<olc::Key::K4>, Int2Type<'4'>>, TypeList<
  TypeList<Int2Type<olc::Key::K5>, Int2Type<'5'>>, TypeList<
  TypeList<Int2Type<olc::Key::K6>, Int2Type<'6'>>, TypeList<
  TypeList<Int2Type<olc::Key::K7>, Int2Type<'7'>>, TypeList<
  TypeList<Int2Type<olc::Key::K8>, Int2Type<'8'>>, TypeList<
  TypeList<Int2Type<olc::Key::K9>, Int2Type<'9'>>, TypeList<

  TypeList<Int2Type<olc::Key::NP0>, Int2Type<'0'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP1>, Int2Type<'1'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP2>, Int2Type<'2'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP3>, Int2Type<'3'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP4>, Int2Type<'4'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP5>, Int2Type<'5'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP6>, Int2Type<'6'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP7>, Int2Type<'7'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP8>, Int2Type<'8'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP9>, Int2Type<'9'>>, TypeList<

  TypeList<Int2Type<olc::Key::TAB>, Int2Type<'\t'>>, TypeList<
  TypeList<Int2Type<olc::Key::BACK>, Int2Type<'\0'>>, TypeList<
  TypeList<Int2Type<olc::Key::DEL>, Int2Type<'\0'>>, TypeList<
  TypeList<Int2Type<olc::Key::SPACE>, Int2Type<' '>>, TypeList<
  TypeList<Int2Type<olc::Key::ENTER>, Int2Type<'\n'>>, TypeList<
  TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<'\0'>>, TypeList<

  TypeList<Int2Type<olc::Key::UP>, Int2Type<'\0'>>, TypeList<
  TypeList<Int2Type<olc::Key::DOWN>, Int2Type<'\0'>>, TypeList<
  TypeList<Int2Type<olc::Key::RIGHT>, Int2Type<'\0'>>, TypeList<
  TypeList<Int2Type<olc::Key::LEFT>, Int2Type<'\0'>>, TypeList<

  TypeList<Int2Type<olc::Key::OEM_1>, Int2Type<';'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_2>, Int2Type<'/'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_3>, Int2Type<'`'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_4>, Int2Type<'['>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_5>, Int2Type<'\\'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_6>, Int2Type<']'>>, TypeList<
  TypeList<Int2Type<olc::Key::OEM_7>, Int2Type<'\''>>, TypeList<

  TypeList<Int2Type<olc::Key::NP_DIV>, Int2Type<'/'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP_MUL>, Int2Type<'*'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP_ADD>, Int2Type<'+'>>, TypeList<
  TypeList<Int2Type<olc::Key::NP_SUB>, Int2Type<'-'>>, TypeList<

  TypeList<Int2Type<olc::Key::EQUALS>, Int2Type<'='>>, TypeList<
  TypeList<Int2Type<olc::Key::PERIOD>, Int2Type<'.'>>, TypeList<
  TypeList<Int2Type<olc::Key::COMMA>, Int2Type<','>>, TypeList<
  TypeList<Int2Type<olc::Key::MINUS>, Int2Type<'-'>>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   KeyEvent; 

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
    AnyType<DARK_GREEN,        ColorSTR>::GetValue() = "DARK_GREEN";        AnyType<DARK_GREEN,        ColorT>::GetValue() = olc::Pixel(0x2A, 0xAE, 0x4F);
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
  NEW_DEBUG_MODE_CALLBACK, ATTACH_DEBUG_MODE_CALLBACK, DETACH_DEBUG_MODE_CALLBACK,
  NEXT_DEBUG_STEP_CALLBACK, DEBUG_RESET_CALLBACK,

  EDITOR_SELECT_CALLBACK, EDITOR_SELECT_LINE_CALLBACK, MEMORY_SELECT_CALLBACK, PANEL_SELECT_CALLBACK,
  
  CMD_UPDATE_CALLBACK, CMD_EXEC_CALLBACK, PROGRAM_EXIT
};

class PixelGameEngine : public olc::PixelGameEngine {
  public:
  enum ModeT { NORMAL, DEBUG };

  virtual void Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>) = 0;
  virtual void Event(Int2Type<ATTACH_DEBUG_MODE_CALLBACK>) = 0;
  virtual void Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>) = 0;

  virtual void Event(Int2Type<DEBUG_RESET_CALLBACK>) = 0;
  virtual void Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>) = 0;

  virtual void Event(Int2Type<EDITOR_SELECT_CALLBACK>, olc::vi2d) = 0;
  virtual void Event(Int2Type<EDITOR_SELECT_LINE_CALLBACK>, olc::vi2d) = 0;
  virtual void Event(Int2Type<PANEL_SELECT_CALLBACK>, int32_t) = 0;
  virtual void Event(Int2Type<MEMORY_SELECT_CALLBACK>, int32_t) = 0;

  virtual void Event(Int2Type<CMD_EXEC_CALLBACK>) = 0;
  virtual void Event(Int2Type<CMD_UPDATE_CALLBACK>, std::string) = 0;
  virtual void Event(Int2Type<PROGRAM_EXIT>) = 0;

  virtual ModeT GetMode() = 0;

  void FillRectDither(olc::vi2d pos, olc::vi2d size, olc::Pixel color = olc::WHITE, int32_t scale = 1, int32_t flag = 0) {
    for (int32_t i = 0, k = flag; i < size.y; i += scale, k ^= 1) {
      for (int32_t j = k * scale; j < size.x; j += scale << 1) {
        FillRect(pos + olc::vi2d(j, i), olc::vi2d(scale, scale), color);
      }
    }
  }
};


