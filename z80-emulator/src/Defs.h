#pragma once

#include "include/Typelist.h"
#include "lib/olcPixelGameEngine.h"

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

namespace Color {
static const olc::Pixel
    GREY(0xAC, 0xB6, 0xC7),   DARK_GREY(0x3A, 0x45, 0x56),   VERY_DARK_GREY(0x18, 0x23, 0x2D),
    RED(0XFF, 0x86, 0x63),    DARK_RED(128, 0, 0),           VERY_DARK_RED(64, 0, 0),
    ORANGE(0xE0, 0x9C, 0x5F), DARK_ORANGE(0xD3, 0x7D, 0x09), VERY_DARK_ORANGE(0x9D, 0x02, 0x08),
    YELLOW(0xEC, 0xCE, 0x8A), DARK_YELLOW(0xD7, 0xAD, 0x5D), VERY_DARK_YELLOW(64, 64, 0),
    GREEN(0x00, 0xC9, 0x5A),  DARK_GREEN(0x2A, 0xAE, 0x4F),  VERY_DARK_GREEN(0, 64, 0),
    CYAN(0x2E, 0xBF, 0xD5),   DARK_CYAN(0, 128, 128),        VERY_DARK_CYAN(0, 64, 64),
    BLUE(0x4C, 0xB8, 0xFE),   DARK_BLUE(0, 0, 128),          VERY_DARK_BLUE(0, 0, 64),
    MAGENTA(0xCC, 0x75, 0xEC),DARK_MAGENTA(128, 0, 128),     VERY_DARK_MAGENT(64, 0, 64),
    WHITE(0xDB, 0xE5, 0xD5),  BLACK(0x14, 0x1A, 0x1F),       BLANK(0, 0, 0, 0);
};


typedef std::pair<olc::vi2d, olc::vi2d> DimensionT;

typedef TypeList<
  Int2Type<olc::Key::SPACE>,TypeList<
  Int2Type<olc::Key::TAB>,  TypeList<
  Int2Type<olc::Key::SHIFT>,TypeList<
  Int2Type<olc::Key::CTRL>, TypeList<
  Int2Type<olc::Key::INS>,  TypeList<
  Int2Type<olc::Key::DEL>,  TypeList<
  Int2Type<olc::Key::HOME>, TypeList<
  Int2Type<olc::Key::END>,  TypeList<
  Int2Type<olc::Key::PGUP>, TypeList<
  Int2Type<olc::Key::PGDN>, TypeList<

  Int2Type<olc::Key::BACK>,  TypeList<
  Int2Type<olc::Key::ESCAPE>,TypeList<
  Int2Type<olc::Key::RETURN>,TypeList<
  Int2Type<olc::Key::ENTER>, TypeList<
  Int2Type<olc::Key::PAUSE>, TypeList<
  Int2Type<olc::Key::SCROLL>,TypeList<

  Int2Type<olc::Key::A>, TypeList<
  Int2Type<olc::Key::B>, TypeList<
  Int2Type<olc::Key::C>, TypeList<
  Int2Type<olc::Key::D>, TypeList<
  Int2Type<olc::Key::E>, TypeList<
  Int2Type<olc::Key::F>, TypeList<
  Int2Type<olc::Key::G>, TypeList<
  Int2Type<olc::Key::H>, TypeList<
  Int2Type<olc::Key::I>, TypeList<
  Int2Type<olc::Key::J>, TypeList<
  Int2Type<olc::Key::K>, TypeList<
  Int2Type<olc::Key::L>, TypeList<
  Int2Type<olc::Key::M>, TypeList<
  Int2Type<olc::Key::N>, TypeList<
  Int2Type<olc::Key::O>, TypeList<
  Int2Type<olc::Key::P>, TypeList<
  Int2Type<olc::Key::Q>, TypeList<
  Int2Type<olc::Key::R>, TypeList<
  Int2Type<olc::Key::S>, TypeList<
  Int2Type<olc::Key::T>, TypeList<
  Int2Type<olc::Key::U>, TypeList<
  Int2Type<olc::Key::V>, TypeList<
  Int2Type<olc::Key::W>, TypeList<
  Int2Type<olc::Key::X>, TypeList<
  Int2Type<olc::Key::Y>, TypeList<
  Int2Type<olc::Key::Z>, TypeList<


  Int2Type<olc::Key::K0>, TypeList<
  Int2Type<olc::Key::K1>, TypeList<
  Int2Type<olc::Key::K2>, TypeList<
  Int2Type<olc::Key::K3>, TypeList<
  Int2Type<olc::Key::K4>, TypeList<
  Int2Type<olc::Key::K5>, TypeList<
  Int2Type<olc::Key::K6>, TypeList<
  Int2Type<olc::Key::K7>, TypeList<
  Int2Type<olc::Key::K8>, TypeList<
  Int2Type<olc::Key::K9>, TypeList<

  Int2Type<olc::Key::NP0>, TypeList<
  Int2Type<olc::Key::NP1>, TypeList<
  Int2Type<olc::Key::NP2>, TypeList<
  Int2Type<olc::Key::NP3>, TypeList<
  Int2Type<olc::Key::NP4>, TypeList<
  Int2Type<olc::Key::NP5>, TypeList<
  Int2Type<olc::Key::NP6>, TypeList<
  Int2Type<olc::Key::NP7>, TypeList<
  Int2Type<olc::Key::NP8>, TypeList<
  Int2Type<olc::Key::NP9>, TypeList<

  Int2Type<olc::Key::OEM_1>, TypeList<
  Int2Type<olc::Key::OEM_2>, TypeList<
  Int2Type<olc::Key::OEM_3>, TypeList<
  Int2Type<olc::Key::OEM_4>, TypeList<
  Int2Type<olc::Key::OEM_5>, TypeList<
  Int2Type<olc::Key::OEM_6>, TypeList<
  Int2Type<olc::Key::OEM_7>, TypeList<
  Int2Type<olc::Key::OEM_8>, TypeList<

  Int2Type<olc::Key::UP>,   TypeList<
  Int2Type<olc::Key::DOWN>, TypeList<
  Int2Type<olc::Key::RIGHT>,TypeList<
  Int2Type<olc::Key::LEFT>, TypeList<

  Int2Type<olc::Key::NP_MUL>, TypeList<
  Int2Type<olc::Key::NP_DIV>, TypeList<
  Int2Type<olc::Key::NP_ADD>, TypeList<
  Int2Type<olc::Key::NP_SUB>, TypeList<

  Int2Type<olc::Key::EQUALS>,TypeList<
  Int2Type<olc::Key::COMMA>, TypeList<
  Int2Type<olc::Key::PERIOD>,TypeList<
  Int2Type<olc::Key::MINUS>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   KeyEvent; 

typedef ListT<
  Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::TAB>,  Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::INS>,  Int2Type<olc::Key::DEL>,  Int2Type<olc::Key::HOME>, Int2Type<olc::Key::END>,  Int2Type<olc::Key::PGUP>, Int2Type<olc::Key::PGDN>, Int2Type<olc::Key::BACK>,  Int2Type<olc::Key::ESCAPE>, Int2Type<olc::Key::RETURN>, Int2Type<olc::Key::ENTER>, Int2Type<olc::Key::PAUSE>, Int2Type<olc::Key::SCROLL>,

  Int2Type<olc::Key::A>, Int2Type<olc::Key::B>, Int2Type<olc::Key::C>, Int2Type<olc::Key::D>, Int2Type<olc::Key::E>, Int2Type<olc::Key::F>, Int2Type<olc::Key::G>, Int2Type<olc::Key::H>, Int2Type<olc::Key::I>, Int2Type<olc::Key::J>, Int2Type<olc::Key::K>, Int2Type<olc::Key::L>, Int2Type<olc::Key::M>, Int2Type<olc::Key::N>, Int2Type<olc::Key::O>, Int2Type<olc::Key::P>, Int2Type<olc::Key::Q>, Int2Type<olc::Key::R>, Int2Type<olc::Key::S>, Int2Type<olc::Key::T>, Int2Type<olc::Key::U>, Int2Type<olc::Key::V>, Int2Type<olc::Key::W>, Int2Type<olc::Key::X>, Int2Type<olc::Key::Y>, Int2Type<olc::Key::Z>, 

  Int2Type<olc::Key::K0>,    Int2Type<olc::Key::K1>,    Int2Type<olc::Key::K2>,    Int2Type<olc::Key::K3>,    Int2Type<olc::Key::K4>,    Int2Type<olc::Key::K5>,    Int2Type<olc::Key::K6>,    Int2Type<olc::Key::K7>,  Int2Type<olc::Key::K8>,  Int2Type<olc::Key::K9>, 
  Int2Type<olc::Key::NP0>,   Int2Type<olc::Key::NP1>,   Int2Type<olc::Key::NP2>,   Int2Type<olc::Key::NP3>,   Int2Type<olc::Key::NP4>,   Int2Type<olc::Key::NP5>,   Int2Type<olc::Key::NP6>,   Int2Type<olc::Key::NP7>, Int2Type<olc::Key::NP8>, Int2Type<olc::Key::NP9>, 
  Int2Type<olc::Key::OEM_1>, Int2Type<olc::Key::OEM_2>, Int2Type<olc::Key::OEM_3>, Int2Type<olc::Key::OEM_4>, Int2Type<olc::Key::OEM_5>, Int2Type<olc::Key::OEM_6>, Int2Type<olc::Key::OEM_7>, Int2Type<olc::Key::OEM_8>,

  Int2Type<olc::Key::UP>,     Int2Type<olc::Key::DOWN>,   Int2Type<olc::Key::RIGHT>,  Int2Type<olc::Key::LEFT>, 
  Int2Type<olc::Key::NP_MUL>, Int2Type<olc::Key::NP_DIV>, Int2Type<olc::Key::NP_ADD>, Int2Type<olc::Key::NP_SUB>, 
  Int2Type<olc::Key::EQUALS>, Int2Type<olc::Key::COMMA>,  Int2Type<olc::Key::PERIOD>, Int2Type<olc::Key::MINUS>
>::type KeyEventT; 


typedef ListT<
  TypeList<Int2Type<0>, ListT<Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::Z>>>
>::type TestT;


class Defs {

public:
  static void Init() {}

  // static void Load(LuaScript* luaConfig) {
  static void Load(void* luaConfig) {
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

  KEYBOARD_EVENT_SUBSCRIBE, KEYBOARD_EVENT_UNSUBSCRIBE,

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

  virtual void Event(Int2Type<KEYBOARD_EVENT_SUBSCRIBE>, void*) = 0;
  virtual void Event(Int2Type<KEYBOARD_EVENT_UNSUBSCRIBE>, void*) = 0;

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


